#include "flyft/brownian_diffusive_flux.h"

namespace flyft
{

void BrownianDiffusiveFlux::compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    if (!validateConstraints(grand,state))
        {
        // ERROR: need constant N in "grand" potential
        }
    allocate(state);

    // evaluate functionals separately to handle ideal as special case
    auto excess = grand->getExcessFunctional();
    auto external = grand->getExternalPotential();
    if (excess)
        excess->compute(state);
    if (external)
        external->compute(state);

    // compute fluxes on the left edge of the volumes
    const auto mesh = *state->getMesh();
    for (const auto& t : state->getTypes())
        {
        const auto D = diffusivities_.at(t);
        auto rho = state->getField(t)->cbegin();
        auto mu_ex = (excess) ? excess->getDerivative(t)->cbegin() : nullptr;
        auto V = (external) ? external->getDerivative(t)->cbegin() : nullptr;
        auto flux = fluxes_.at(t)->begin();

        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(D,mesh) shared(rho,mu_ex,V,flux)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            const int self = mesh(idx);
            const int left = mesh(idx-1);

            // handle infinite external potentials carefully, as there should be no flux in those directions
            bool no_flux = false;
            if (V)
                {
                bool Vidx_inf = std::isinf(V[self]);
                if (Vidx_inf)
                    {
                    if (V[self] > 0 && rho[self] > 0)
                        {
                        // ERROR: can't be V=+inf and have positive rho
                        }
                    else if(V[self] < 0)
                        {
                        // ERROR: can't have V=-inf ever, it is a mass sink
                        }
                    }

                // can't have flux along an edge normal having V = +inf at either end
                // because we just asserted there must be no density there
                no_flux = (Vidx_inf || std::isinf(V[left]));
                }

            // if there is flux, assume density is continuous and interpolate
            if (!no_flux)
                {
                // average density at left edge
                auto rho_avg = 0.5*(rho[left]+rho[self]);

                // excess contribute at left edge
                auto dmu_ex = 0.0;
                if (mu_ex) dmu_ex += (mu_ex[self]-mu_ex[left]);
                if (V) dmu_ex += (V[self]-V[left]);

                // ideal (Fickian) term + excess term
                // the ideal term is separated out so that we don't need to take
                // low density limit explicitly
                flux[self] = -D*((rho[self]-rho[left])/mesh.step() + rho_avg*dmu_ex/mesh.step());
                }
            else
                {
                flux[self] = 0.;
                }
            }
        }
    }

}
