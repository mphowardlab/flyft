#include "flyft/field.h"
#include "flyft/mesh.h"
#include "flyft/rosenfeld_fmt.h"
#include "flyft/state.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

int main()
    {
    auto mesh = std::make_shared<const flyft::Mesh>(5.0,25);
    auto state = std::make_shared<flyft::State>(mesh,2);

    const std::vector<double> diameters = {1.0,2.0};
    const std::vector<double> etas = {0.01,0.01};
    const std::vector<double> detas = {0.01,0.01};

    // initialize density profiles
    for (int i=0; i < state->getNumFields(); ++i)
        {
        auto field = state->getField(i);

        const double d = diameters[i];
        const double rho = 6.*etas[i]/(M_PI*d*d*d);
        const double drho = 6.*detas[i]/(M_PI*d*d*d);

        auto data = field->data();
        for (int idx=0; idx < mesh->shape(); ++idx)
            {
            const double dx = mesh->coordinate(idx)/mesh->L()-0.5;
            data[idx] = rho + drho*(4.*dx*dx);
            }
        }

    auto fmt = std::make_shared<flyft::RosenfeldFMT>(diameters);
    for (unsigned int iter=0; iter < 2; ++iter)
        {
        fmt->compute(state);
        }
    for (int idx=0; idx < mesh->shape(); ++idx)
        {
        std::cout << mesh->coordinate(idx);
        for (int i=0; i < state->getNumFields(); ++i)
            {
            std::cout << " " << fmt->getDerivative(i)->data()[idx];
            }
        std::cout << std::endl;
        }

    return 0;
    }
