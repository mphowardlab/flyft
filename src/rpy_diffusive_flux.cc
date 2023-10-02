#include "flyft/rpy_diffusive_flux.h"
#include "flyft/spherical_mesh.h"

#include <exception>

namespace flyft
{

RPYDiffusiveFlux::RPYDiffusiveFlux()
    : viscosity_(1.0)
    {
    }
   
void RPYDiffusiveFlux::compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    setup(grand,state);
    
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
    state->syncFields();

    // compute fluxes on the left edge of the volumes (exclude the first point)
    const auto mesh = state->getMesh()->local().get();
    if (!dynamic_cast<const SphericalMesh*>(mesh))
        {
        throw std::invalid_argument("Spherical geometry required");
        }

    for (const auto &i : state->getTypes())
        {
        const double a_i = 0.5 * diameters_(i);
        auto rho_i = state->getField(i)->const_view();
        auto flux_i = fluxes_(i)->view();
        const double D_i = 1/(6*M_PI*viscosity_*a_i);

        // fill flux with zeros initially
        std::fill(flux_i.begin(), flux_i.end(), 0.);

        // RPY flux from all species
        for (const auto &j : state->getTypes())
            {
            const double a_j = 0.5 * diameters_(j);
            auto rho_j = state->getField(j)->const_view();
            auto mu_ex_j = (excess) ? excess->getDerivative(j)->const_view() : Field::ConstantView();
            auto V_j = (external) ? external->getDerivative(j)->const_view() : Field::ConstantView();
            for (int idx = 0; idx < mesh->shape(); ++idx)
                {
                if (V_j)
                    {
                    bool Vidx_inf = std::isinf(V_j(idx));
                    if (Vidx_inf)
                        {
                        if (V_j(idx) > 0 && rho_j(idx) > 0)
                            {
                            throw std::invalid_argument("Can't be V=+inf and have positive rho");
                            }
                        else if(V_j(idx) < 0)
                            {
                            throw std::invalid_argument("Can't have V=-inf ever, it is a mass sink");
                            }
                        }
                    if (Vidx_inf || std::isinf(V_j(idx-1)))
                        {
                        flux_i(idx) = 0.;
                        continue;
                        }
                    }

                const auto x = mesh->lower_bound(idx);
                
                /* Considering symmetry about the center of the sphere and concentration gradient 
                along the radial direction only, the flux at the center of the sphere is 0, i.e. 
                density flux going into the center is equal to density flux out of the center. 
                If this condition is not met, the symmetry assumption would be violated.*/
                
                if (x==0)
                    {
                    flux_i(idx) = 0.;
                    continue;
                    }
                auto rho_x = mesh->interpolate(x, rho_i);
              
                //BD flux calculation
                if(i==j)
                    {
                    auto dmu_ex = 0.0;
                    if (mu_ex_j) dmu_ex += mesh->gradient(idx,mu_ex_j);
                    if (V_j) dmu_ex += mesh->gradient(idx,V_j);
                    flux_i(idx) += -D_i*(mesh->gradient(idx,rho_j) + rho_x*dmu_ex);
                    }
 
                //RPY flux calculation   
                const double d_ij = a_i + a_j;
                
                //To remove the concern about the lower bound value spill over the buffer sites
                const int ig_low = std::ceil((std::abs(x-d_ij)-mesh->lower_bound())/mesh->step());
                const int ig_high = mesh->bin(x+d_ij);
                double ig = 0.;
                for (int ig_idx = ig_low; ig_idx < ig_high; ++ig_idx)
                    {
                    const auto y = mesh->lower_bound(ig_idx);
                    // total gradient of chemical potential
                    double rho_dmu = mesh->gradient(ig_idx, rho_j);
                    auto rho_y = mesh->interpolate(y, rho_j);
                    if (mu_ex_j)
                        rho_dmu += rho_y * mesh->gradient(ig_idx, mu_ex_j);
                    if (V_j)
                        rho_dmu += rho_y * mesh->gradient(ig_idx, V_j);
                    
                    const double M = ((a_i * a_i + 3 * a_i * a_j + a_j * a_j) * 
                                (d_ij - x - y) * (d_ij + x - y) * (d_ij - x + y) 
                                * (d_ij + x + y)) / (24 * x * x * viscosity_ * d_ij * d_ij * d_ij);
                    ig += M * rho_dmu;
                    }
                flux_i(idx) += -rho_x * ig * mesh->step();
                }
            }
        state->getMesh()->startSync(fluxes_(i));
        }
    
    // finalize all flux communication
    for (const auto& i : state->getTypes())
        {
        state->getMesh()->endSync(fluxes_(i));
        }
    }

TypeMap<double>& RPYDiffusiveFlux::getDiameters()
    {
    return diameters_;   
    }

const TypeMap<double>& RPYDiffusiveFlux::getDiameters() const
    {
    return diameters_;   
    }

double RPYDiffusiveFlux::getViscosity() const
    {
    return viscosity_;
    }

void RPYDiffusiveFlux::setViscosity(double viscosity)
    {
    if(viscosity <= 0)
        {
        throw std::invalid_argument("Viscosity must be positive");
        }
    viscosity_ = viscosity;
    }
    
int RPYDiffusiveFlux::determineBufferShape(std::shared_ptr<State> state, const std::string& type)
    {
    double max_diameter = 0;
    auto mesh = state->getMesh()->full().get();
    for(const auto &i : state->getTypes()) 
    {
        const auto d_i = diameters_(i);
        if(d_i > max_diameter)
            {
            max_diameter = d_i;
            }
    }
    return mesh->asShape(0.5*(diameters_(type)+max_diameter));
    }
}
