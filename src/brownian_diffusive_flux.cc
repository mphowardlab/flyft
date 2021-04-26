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
    auto mesh = state->getMesh();
    for (const auto& t : state->getTypes())
        {
        const auto D = diffusivities_.at(t);
        auto rho = state->getField(t)->data();
        auto mu_ex = (excess) ? excess->getDerivative(t)->data() : nullptr;
        auto V = (external) ? external->getDerivative(t)->data() : nullptr;

        const auto dx = mesh->step();
        auto flux = fluxes_.at(t)->data();

        for (int idx=0; idx < mesh->shape(); ++idx)
            {
            // explicitly apply pbcs on the index
            // TODO: add a wrapping function to the mesh
            int left = (idx > 0) ? idx-1 : mesh->shape()-1;
            int self = idx;

            // average density at left edge
            auto rho_avg = 0.5*(rho[left]+rho[self]);

            // excess contribute at left edge
            auto dmu_ex = 0.0;
            if (mu_ex) dmu_ex += (mu_ex[self]-mu_ex[left]);
            if (V) dmu_ex += (V[self]-V[left]);

            // ideal (Fickian) term + excess term
            flux[idx] = -D*((rho[self]-rho[left])/dx + rho_avg*dmu_ex/dx);
            }
        }
    }

}
