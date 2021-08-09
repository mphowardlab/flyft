#include "flyft/brownian_diffusive_flux.h"

namespace flyft
{

void BrownianDiffusiveFlux::compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    setup(grand,state);

    // evaluate functionals separately to handle ideal as special case
    auto excess = grand->getExcessFunctional();
    auto external = grand->getExternalPotential();
    if (excess)
        {
        excess->compute(state);
        state->syncFields(excess->getDerivatives());
        }
    if (external)
        {
        external->compute(state);
        state->syncFields(external->getDerivatives());
        }
    state->syncFields();

    // compute fluxes on the left edge of the volumes
    const auto mesh = *state->getMesh()->local();
    for (const auto& t : state->getTypes())
        {
        const auto D = diffusivities_.at(t);
        auto rho = state->getField(t)->const_view();
        auto mu_ex = (excess) ? excess->getDerivative(t)->const_view() : Field::ConstantView();
        auto V = (external) ? external->getDerivative(t)->const_view() : Field::ConstantView();
        auto flux = fluxes_.at(t)->view();

        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(D,mesh) shared(rho,mu_ex,V,flux)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            // handle infinite external potentials carefully, as there should be no flux in those directions
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
                auto rho_avg = 0.5*(rho(idx-1)+rho(idx));

                // excess contribute at left edge
                auto dmu_ex = 0.0;
                if (mu_ex) dmu_ex += (mu_ex(idx)-mu_ex(idx-1));
                if (V) dmu_ex += (V(idx)-V(idx-1));

                // ideal (Fickian) term + excess term
                // the ideal term is separated out so that we don't need to take
                // low density limit explicitly
                flux(idx) = -D*((rho(idx)-rho(idx-1))/mesh.step() + rho_avg*dmu_ex/mesh.step());
                }
            else
                {
                flux(idx) = 0.;
                }
            }
        }
    }

int BrownianDiffusiveFlux::determineBufferShape(std::shared_ptr<State> /*state*/, const std::string& /*type*/)
    {
    return 1;
    }

}
