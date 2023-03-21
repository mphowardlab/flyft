#include "flyft/brownian_diffusive_flux.h"

namespace flyft
{

static double calculateFlux(int idx,
                            double D,
                            const Field::ConstantView& rho,
                            const Field::ConstantView& mu_ex,
                            const Field::ConstantView& V,
                            const Mesh* mesh)
    {
    // handle infinite external potentials carefully, as there should be no flux in those directions
    double flux;
    bool no_flux = false;
    if (V)
        {
        bool Vidx_inf = std::isinf(V(idx));
        if (Vidx_inf)
            {
            if (V(idx) > 0 && rho(idx) > 0)
                {
                // ERROR: can't be V=+inf and have positive rho
                }
            else if(V(idx) < 0)
                {
                // ERROR: can't have V=-inf ever, it is a mass sink
                }
            }

        // can't have flux along an edge normal having V = +inf at either end
        // because we just asserted there must be no density there
        no_flux = (Vidx_inf || std::isinf(V(idx-1)));
        }

    // if there is flux, assume density is continuous and interpolate
    if (!no_flux)
        {
        // average density at left edge
        auto rho_avg = mesh->interpolate(idx,rho);

        // excess contribute at left edge
        auto dmu_ex = 0.0;
        if (mu_ex) dmu_ex += mesh->gradient(idx,mu_ex);
        if (V) dmu_ex += mesh->gradient(idx,V);

        // ideal (Fickian) term + excess term
        // the ideal term is separated out so that we don't need to take
        // low density limit explicitly
        flux = -D*(mesh->gradient(idx,rho) + rho_avg*dmu_ex);
        }
    else
        {
        flux = 0.;
        }
    return flux;
    }


void BrownianDiffusiveFlux::compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    setup(grand,state);

    // evaluate functionals separately to handle ideal as special case
    auto excess = grand->getExcessFunctional();
    auto external = grand->getExternalPotential();
    if (excess)
        {
        excess->compute(state,false);
        }
    if (external)
        {
        external->compute(state,false);
        }
    // sync fields as a precaution, but this will already likely have been done by functionals
    state->syncFields();

    // compute fluxes on the left edge of the volumes (exclude the first point)
    const auto mesh = state->getMesh()->local().get();
    for (const auto& t : state->getTypes())
        {
        const auto D = diffusivities_(t);
        auto rho = state->getField(t)->const_view();
        auto mu_ex = (excess) ? excess->getDerivative(t)->const_view() : Field::ConstantView();
        auto V = (external) ? external->getDerivative(t)->const_view() : Field::ConstantView();
        auto flux = fluxes_(t)->view();

        // compute flux on edges and start sending
        int flux_buffer = fluxes_(t)->buffer_shape();
        for (int idx=0; idx < flux_buffer; ++idx)
            {
            flux(idx) = calculateFlux(idx,D,rho,mu_ex,V,mesh);
            }
        for (int idx=mesh->shape()-flux_buffer; idx < mesh->shape(); ++idx)
            {
            flux(idx) = calculateFlux(idx,D,rho,mu_ex,V,mesh);
            }
        state->getMesh()->startSync(fluxes_(t));

        // compute flux on interior points
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(D,mesh) \
        shared(rho,mu_ex,V,flux,flux_buffer)
        #endif
        for (int idx=flux_buffer; idx < mesh->shape()-flux_buffer; ++idx)
            {
            flux(idx) = calculateFlux(idx,D,rho,mu_ex,V,mesh);
            }
        }

    // finalize all flux communication
    for (const auto& t : state->getTypes())
        {
        state->getMesh()->endSync(fluxes_(t));
        }
    }

int BrownianDiffusiveFlux::determineBufferShape(std::shared_ptr<State> /*state*/, const std::string& /*type*/)
    {
    return 1;
    }

}
