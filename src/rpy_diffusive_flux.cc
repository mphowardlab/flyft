#include "flyft/rpy_diffusive_flux.h"
#include "flyft/spherical_mesh.h"

#include <algorithm>
#include <exception>
#include <tuple>

namespace flyft
{

RPYDiffusiveFlux::RPYDiffusiveFlux()
    : viscosity_(1.0)
    {
    mobility_ = std::make_shared<GridInterpolator>("/scratch2/mzk0148/projects/ddft_droplet/data/rdf_surrogate/m.dat"); 
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
    
    const double max_x = std::get<1>(g.getBounds());
    const double cutoff = std::get<3>(g.getBounds());
    const double max_density = (6/M_PI)*std::get<5>(g.getBounds());
   
    for (const auto &i : state->getTypes())
        {
        if(diameters_(i) != 1)
            {
            throw std::invalid_argument("Diameter greater than 1 not supported");
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
 
                //To remove the concern about the lower bound value spill over the buffer sites
                const int ig_low_near_center = std::ceil(((d_ij-x)-mesh->lower_bound())/mesh->step());
                const int ig_low = (x >= 1) ? (0): ig_low_near_center;
                const int ig_high = mesh->bin(x + cutoff);
                
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

                    const double dx = y-x;
                    const double mean_rho = (rho_y+rho_x)/2;
                    const double mean_rho_int = std::min(mean_rho, max_density);
                    const double mean_phi = mean_rho_int * (M_PI/6);
                    const double M = g(x_int, dx, mean_phi);
                    
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
    
    const GridInterpolator& g = *mobility_;
    const double cutoff = std::get<1>(g.getBounds());
    
    for(const auto &i : state->getTypes()) 
        {
        const auto d_i = diameters_(i);
        if(d_i > max_diameter)
            {
            max_diameter = d_i;
            }
        }
    return mesh->asShape(cutoff*0.5*(diameters_(type)+max_diameter));
    }
}
