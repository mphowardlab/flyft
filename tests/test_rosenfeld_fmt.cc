#include "flyft/field.h"
#include "flyft/mesh.h"
#include "flyft/rosenfeld_fmt.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

int main()
    {
    std::vector<std::string> types {"A","B"};
    flyft::TypeMap<double> diameters {{"A",1.0}, {"B",2.0}};
    flyft::TypeMap<double> etas {{"A",0.01},{"B",0.01}};
    flyft::TypeMap<double> detas {{"A",0.01},{"B",0.01}};

    auto mesh = std::make_shared<const flyft::Mesh>(5.0,25);
    auto state = std::make_shared<flyft::State>(mesh,types);

    state->setDiameters(diameters);

    // initialize density profiles
    for (const auto& t : state->getTypes())
        {
        auto field = state->getField(t);

        const double d = diameters[t];
        const double rho = 6.*etas[t]/(M_PI*d*d*d);
        const double drho = 6.*detas[t]/(M_PI*d*d*d);

        auto data = field->data();
        for (int idx=0; idx < mesh->shape(); ++idx)
            {
            const double dx = mesh->coordinate(idx)/mesh->L()-0.5;
            data[idx] = rho + drho*(4.*dx*dx);
            }
        }

    auto fmt = std::make_shared<flyft::RosenfeldFMT>();
    for (unsigned int iter=0; iter < 2; ++iter)
        {
        fmt->compute(state);
        }
    for (int idx=0; idx < mesh->shape(); ++idx)
        {
        std::cout << mesh->coordinate(idx);
        for (const auto& t : state->getTypes())
            {
            std::cout << " " << fmt->getDerivative(t)->data()[idx];
            }
        std::cout << std::endl;
        }

    return 0;
    }
