#include "flyft/spherical_mesh.h"
#include "flyft/rpy_diffusive_flux.h"

#include <exception>

namespace flyft
{
   
void RPYDiffusiveFlux::compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    setup(grand,state);
    state->syncFields();
    
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
    
    
    // compute fluxes on the left edge of the volumes (exclude the first point)
    const auto mesh = state->getMesh()->local().get();
    if (dynamic_cast<const SphericalMesh*>(mesh) != nullptr)
        {
        for (const auto &i : state->getTypes())
            {
            const double a_i = 0.5 * diameters_(i);
            auto rho_i = state->getField(i)->const_view();
            auto flux_i = fluxes_(i)->view();

            // fill flux with zeros initially
            std::fill(flux_i.begin(), flux_i.end(), 0.);

            // RPY flux from all species
            for (const auto &j : state->getTypes())
                {
                const double a_j = 0.5 * diameters_(j);
                auto rho_j = state->getField(j)->const_view();
                auto mu_ex_j = (excess) ? excess->getDerivative(j)->const_view() : Field::ConstantView();
                auto V_j = (external) ? external->getDerivative(j)->const_view() : Field::ConstantView();
                bool no_flux = false;
                for (int idx = 0; idx < mesh->shape(); ++idx)
                    {
                    if (V_j)
                        {
                        bool Vidx_inf = std::isinf(V_j(idx));
                        if (Vidx_inf)
                            {
                            if (V_j(idx) > 0 && rho_j(idx) > 0)
                                {
                                // ERROR: can't be V=+inf and have positive rho
                                }
                            else if(V_j(idx) < 0)
                                {
                                // ERROR: can't have V=-inf ever, it is a mass sink
                                }
                            }
                        no_flux = (Vidx_inf || std::isinf(V_j(idx-1)));
                        }
                    if (!no_flux)
                        {
                        const auto x = mesh->lower_bound(idx);
                        if (x==0)
                            {
                            flux_i(idx) = 0;
                            }
                        else
                            {
                            auto rho_x = mesh->interpolate(x, rho_i);

                            const double d_ij = a_i + a_j;
                            const int ig_low = mesh->bin(std::max(x - d_ij, 0.));
                            const int ig_high = mesh->bin(x + d_ij);
                            double ig = 0.;
                            for (int ig_idx = ig_low; ig_idx <= ig_high; ++ig_idx)
                                {
                                const auto y = mesh->lower_bound(ig_idx);
                                // total gradient of chemical potential
                                double rho_dmu = mesh->gradient(ig_idx, rho_j);
                                auto rho_y = mesh->interpolate(y, rho_j);
                                if (mu_ex_j)
                                    rho_dmu += rho_y * mesh->gradient(ig_idx, mu_ex_j);
                                if (V_j)
                                    rho_dmu += rho_y * mesh->gradient(ig_idx, V_j);
                                // M will need the viscosity too
                                const double M = ((a_i * a_i + 3 * a_i * a_j + a_j * a_j) * 
                                            (d_ij - x - y) * (d_ij + x - y) * (d_ij - x + y) 
                                            * (d_ij + x + y)) / (24 * x * x * viscosity_ * d_ij * d_ij * d_ij);
                                ig += M * rho_dmu;
                                }
                            // is this sign right? might be backwards
                            flux_i(idx) += -rho_x * ig * mesh->step();
                            }
                        }
                    }
                }
            }
        }
    else
        {
        throw std::invalid_argument("Geometry is not valid for RPY hydrodynamics flux calculation");
        }
    }

int RPYDiffusiveFlux::determineBufferShape(std::shared_ptr<State> /*state*/, const std::string& /*type*/)
    {
    return 1;
    }

double RPYDiffusiveFlux::getViscosity()
    {
        return viscosity_;
    }

double RPYDiffusiveFlux::setViscosity(double viscosity)
    {
        if(viscosity==0)  
            {
            throw std::invalid_argument("Viscosity cannot be zero");
            }
        else
            {
            viscosity_ = viscosity;
            return viscosity_; 
            }
    }

}
