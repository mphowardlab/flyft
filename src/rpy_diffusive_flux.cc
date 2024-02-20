#include "flyft/rpy_diffusive_flux.h"
#include "flyft/spherical_mesh.h"

#include <exception>
#include <tuple>
#include <fstream>

namespace flyft
{

RPYDiffusiveFlux::RPYDiffusiveFlux()
    : viscosity_(1.0)
    {
    mobility_ = std::make_shared<GridInterpolator>("/home/che_h2/mzk0148/m.dat"); 
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
    
    if(state->getNumFields() > 1)
        {
        throw std::invalid_argument("Only one type of particle is supported");
        }
        
    const GridInterpolator& g = *mobility_;
    
    const double max_x = std::get<0>(g.getUpperBounds());
    const double cutoff = std::get<1>(g.getUpperBounds());
    const double max_density = std::get<2>(g.getUpperBounds());
    
    if(std::get<1>(g.getLowerBounds()) != -cutoff)
        {
        throw std::invalid_argument("Cutoff bounds for integration are not valid");
        }
        
    for (const auto &i : state->getTypes())
        {
        if(diameters_(i) != 1)
            {
            throw std::invalid_argument("Diameters should be 1 for all types");
            }
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
                const auto x = mesh->lower_bound(idx);
                
                /* Considering symmetry about the center of the sphere and concentration gradient 
                along the radial direction only, the flux at the center of the sphere is 0, i.e. 
                density flux going into the center is equal to density flux out of the center. 
                If this condition is not met, the symmetry assumption would be violated.*/
                
                if (x==0)
                    {
                    continue;
                    }
                auto rho_x = mesh->interpolate(x, rho_i); 
                //BD flux calculation
                if(i==j)
                    {
                    auto dmu_ex = 0.0;
                    if (mu_ex_j) dmu_ex += mesh->gradient(idx,mu_ex_j);
                    if (V_j)
                        {
                        if (std::isinf(V_j(idx)))
                            {
                            throw std::invalid_argument("RPY is incompatible with infinite potentials");
                            }
                        dmu_ex += mesh->gradient(idx,V_j);
                        }
                    flux_i(idx) += -D_i*(mesh->gradient(idx,rho_j) + rho_x*dmu_ex);
                    }
 
                //RPY flux calculation   
                const double d_ij = a_i + a_j;
                const double x_int = std::min<double>(x, max_x);
 
                double ig = 0.;
                const int cutoff_bins = mesh->asShape(cutoff);
                for (int ig_idx = idx - cutoff_bins; ig_idx < idx + cutoff_bins; ++ig_idx)
                    {
                    double y = mesh->lower_bound(ig_idx);
                    if (y < ((x >= 1)? 0 : d_ij - x))
                        {
                        continue;
                        }
                    // total gradient of chemical potential
                    double rho_dmu = mesh->gradient(ig_idx, rho_j);
                    auto rho_y = mesh->interpolate(y, rho_j);
                    if (mu_ex_j)
                        rho_dmu += rho_y * mesh->gradient(ig_idx, mu_ex_j);
                    if (V_j)
                        rho_dmu += rho_y * mesh->gradient(ig_idx, V_j);
                        
                    const double dx = std::max(std::min(y-x, cutoff), -cutoff);
                    const double mean_rho_int = std::max(std::min((rho_y+rho_x)/2, max_density), 0.);
                    const double M = g(x_int, dx, mean_rho_int);
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
    
int RPYDiffusiveFlux::determineBufferShape(std::shared_ptr<State> state, const std::string& /* type */)
    {
    auto mesh = state->getMesh()->full().get();
    const GridInterpolator& g = *mobility_;
    const double cutoff = std::get<1>(g.getUpperBounds()); 
    return mesh->asShape(cutoff) + 1;
    }
}
