#include "flyft/boublik_hard_sphere_functional.h"

#include <cmath>

namespace flyft
{

BoublikHardSphereFunctional::BoublikHardSphereFunctional()
    : diameters_(std::make_shared<TypeMap<double>>())
    {
    depends_.add(diameters_);
    }

void BoublikHardSphereFunctional::compute(std::shared_ptr<State> state)
    {
    setup(state);
    if (!needsCompute(state))
        {
        return;
        }

    auto types = state->getTypes();
    const auto mesh = *state->getMesh()->local();

    // process maps into indexed arrays for quicker access inside loop
    const auto num_types = types.size();
    std::vector<Field::ConstantView> fields(num_types);
    std::vector<Field::View> derivs(num_types);
    std::vector<double> diams(num_types);
    for (size_t i=0; i < num_types; ++i)
        {
        const auto type_i = types[i];
        fields[i] = state->getField(type_i)->const_view();
        derivs[i] = derivatives_(type_i)->view();
        diams[i] = diameters_->get(type_i);
        }

    // reset energy to zero before accumulating
    value_ = 0.0;

    if (num_types > 1)
        {
        // General Boublik equation for more than 1 type
        double xi[4];
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) private(xi) firstprivate(num_types,mesh) \
        shared(fields,derivs,diams) reduction(+:value_)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            // compute scaled particle variables
            for (int m=0; m < 4; ++m)
                {
                xi[m] = 0.;
                }
            for (size_t i=0; i < num_types; ++i)
                {
                const auto rhoi = fields[i](idx);
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
                    derivs[i](idx) = -logvf + di*(c1+di*(c2+di*c3));
                    }

                // compute free energy
                energy = mesh.step()*(6./M_PI)*((xi2_3/xi3_2-xi[0])*logvf + 3.*xi[1]*xi[2]/vf + xi2_3/(xi[3]*vf_2));
                }
            else
                {
                for (size_t i=0; i < num_types; ++i)
                    {
                    derivs[i](idx) = 0.;
                    }
                energy = 0.;
                }
            value_ += energy;
            }
        }
    else
        {
        // Carnahan-Starling simplification for 1 type
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(fields,derivs,diams) reduction(+:value_)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            const auto rho = fields[0](idx);
            const auto d = diams[0];
            const auto eta = M_PI*rho*d*d*d/6.;

            // the excess terms will only be nonzero if nonzero density and diameter
            bool has_excess = (rho > 0. && d > 0.);
            double energy;
            if (has_excess)
                {
                const auto vf = 1.-eta;
                const auto vf_2 = vf*vf;
                const auto vf_3 = vf_2*vf;

                // compute chemical potential
                derivs[0](idx) = eta*(8.+eta*(-9.+eta*3.))/vf_3;

                // compute free energy
                energy = mesh.step()*rho*eta*(4.-3.*eta)/vf_2;
                }
            else
                {
                derivs[0](idx) = 0.;
                energy = 0.;
                }
            value_ += energy;
            }
        }

    value_ = state->getCommunicator()->sum(value_);

    finalize(state);
    }

std::shared_ptr<TypeMap<double>> BoublikHardSphereFunctional::diameters()
    {
    return diameters_;
    }

std::shared_ptr<const TypeMap<double>> BoublikHardSphereFunctional::diameters() const
    {
    return diameters_;
    }

}
