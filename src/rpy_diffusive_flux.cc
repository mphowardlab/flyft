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
    if (!dynamic_cast<const SphericalMesh*>(mesh))
        {
        throw std::invalid_argument("Spherical geometry required");
        }

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
                       
                        //BD flux calculation
                        if(i==j)
                            {
                            auto dmu_ex = 0.0;
                            if (mu_ex_j) dmu_ex += mesh->gradient(idx,mu_ex_j);
                            if (V_j) dmu_ex += mesh->gradient(idx,V_j);
                            double D = 1/(6*M_PI*viscosity_*a_j);
                            flux_i(idx) += -D*(mesh->gradient(idx,rho_j) + rho_x*dmu_ex);
                            }
                       
                        //RPY flux calculation   
                        const double d_ij = a_i + a_j;
                        
                        //To remove the concern about the lower bound value spill over the buffer sites
                        double lower_ig = ceil((x-d_ij-mesh->lower_bound())/mesh->step());
                        const int ig_low = mesh->bin(std::max(lower_ig, 0.));
                        const int ig_high = floor((mesh->upper_bound()-x+d_ij)/mesh->step());
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
                         
                            flux_i(idx) += -rho_x * ig * mesh->step();
                            }
                        }
                    }
                else
                    {
                    flux_i(idx) = 0.;
                    }
                }
            }
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
    if(viscosity <=0)  
        {
        throw std::invalid_argument("Viscosity cannot be zero");
        }
    viscosity_ = viscosity;
    }
    
int RPYDiffusiveFlux::determineBufferShape(std::shared_ptr<State> state, const std::string& /*type*/)
    {
    int buffer = 0;
    double large_R = 0; 
    for(const auto &i : state->getTypes()) 
        {
        const auto a_i = 0.5*diameters_(i);
        if(a_i>large_R)
            {
            large_R = a_i;
            }
        buffer = a_i/2+large_R/2;
        }
    return buffer;
    }
}
