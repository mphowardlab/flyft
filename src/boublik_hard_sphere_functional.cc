#include "flyft/boublik_hard_sphere_functional.h"

#include <cmath>

namespace flyft
{

void BoublikHardSphereFunctional::compute(std::shared_ptr<State> state)
    {
    allocate(state);

    auto types = state->getTypes();
    auto mesh = state->getMesh();

    // process maps into indexed arrays for quicker access inside loop
    const auto num_types = types.size();
    std::vector<const double*> fields(num_types);
    std::vector<double*> derivs(num_types);
    std::vector<double> diams(num_types);
    for (size_t i=0; i < num_types; ++i)
        {
        const auto type_i = types[i];
        fields[i] = state->getField(type_i)->data();
        derivs[i] = derivatives_.at(type_i)->data();
        diams[i] = diameters_.at(type_i);
        }

    // reset energy to zero before accumulating
    value_ = 0.0;

    double xi[4];
    const auto shape = mesh->shape();
    const auto dx = mesh->step();
    #ifdef FLYFT_OPENMP
    #pragma omp parallel for schedule(static) default(none) private(xi) shared(fields,derivs,diams,num_types,shape,dx) reduction(+:value_)
    #endif
    for (int idx=0; idx < shape; ++idx)
        {
        // compute scaled particle variables
        for (int m=0; m < 4; ++m)
            {
            xi[m] = 0.;
            }
        for (size_t i=0; i < num_types; ++i)
            {
            const auto rhoi = fields[i][idx];
            const auto di = diams[i];
            double xim_i = rhoi*M_PI/6.;
            for (int m=0; m < 4; ++m)
                {
                xi[m] += xim_i;
                xim_i *= di;
                }
            }

        // the excess terms will only be nonzero if xi0 > 0 (nonzero density) and any other
        // xi (e.g., xi3) > 0 (nonzero diameters)
        bool has_excess = (xi[0] > 0. && xi[3] > 0.);
        double energy;
        if (has_excess)
            {
            // compute some auxiliary variables for evaluating free energy and chemical potential
            const double vf = 1.-xi[3];
            if (vf < 0)
                {
                // local void fraction unphysical
                }
            const double logvf = std::log(vf);
            const double vf_2 = vf*vf;
            const double vf_3 = vf_2*vf;
            const double xi2_2 = xi[2]*xi[2];
            const double xi2_3 = xi2_2*xi[2];
            const double xi3_2 = xi[3]*xi[3];
            const double xi3_3 = xi3_2*xi[3];
            const double c1 = 3.*xi[2]/vf;
            const double c2 = 3.*(xi[1]/vf + xi2_2/xi3_2*logvf + xi2_2/(vf_2*xi[3]));
            const double c3 = (3.*xi[1]*xi[2]/vf_2 - (xi2_3/xi3_2-xi[0])/vf - 2.*xi2_3/xi3_3*logvf
                               - xi2_3/(xi3_2*vf_2) + 2.*xi2_3/(xi[3]*vf_3));

            // compute chemical potential
            for (size_t i=0; i < num_types; ++i)
                {
                const auto di = diams[i];
                derivs[i][idx] = -logvf + di*(c1+di*(c2+di*c3));
                }

            // compute free energy
            energy = dx*(6./M_PI)*((xi2_3/xi3_2-xi[0])*logvf + 3.*xi[1]*xi[2]/vf + xi2_3/(xi[3]*vf_2));
            }
        else
            {
            for (size_t i=0; i < num_types; ++i)
                {
                derivs[i][idx] = 0.;
                }
            energy = 0.;
            }
        value_ += energy;
        }
    }

const TypeMap<double>& BoublikHardSphereFunctional::getDiameters()
    {
    return diameters_;
    }

double BoublikHardSphereFunctional::getDiameter(const std::string& type) const
    {
    return diameters_.at(type);
    }

void BoublikHardSphereFunctional::setDiameters(const TypeMap<double>& diameters)
    {
    diameters_ = TypeMap<double>(diameters);
    }

void BoublikHardSphereFunctional::setDiameter(const std::string& type, double diameter)
    {
    if (diameter >= 0.)
        {
        diameters_[type] = diameter;
        }
    else
        {
        // error: invalid diameter
        }
    }

}
