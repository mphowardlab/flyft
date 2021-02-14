#include "flyft/composite_functional.h"
#include "flyft/field.h"
#include "flyft/hard_wall_potential.h"
#include "flyft/mesh.h"
#include "flyft/piccard_iteration.h"
#include "flyft/rosenfeld_fmt.h"
#include "flyft/state.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

int main()
    {
    const double L = 20.0;
    std::vector<std::string> types {"A"};
    flyft::TypeMap<double> diameters {{"A",1.0}};
    flyft::TypeMap<double> etas {{"A",0.4257}};

    auto mesh = std::make_shared<const flyft::Mesh>(L,1000);
    auto state = std::make_shared<flyft::State>(mesh,types);

    // initialize density profiles
    for (const auto& t : state->getTypes())
        {
        const double d = diameters[t];
        const double rho = 6.*etas[t]/(M_PI*d*d*d);

        // fill initial density
        auto data = state->getField(t)->data();
        std::fill(data, data+mesh->shape(), rho);

        // set diameter and average N
        state->setDiameter(t,d);
//         state->setConstraint(t,L*rho,flyft::State::Constraint::N);
        }

    auto fmt = std::make_shared<flyft::RosenfeldFMT>();

    auto Vext = std::make_shared<flyft::CompositeFunctional>();
    Vext->addFunctional(std::make_shared<flyft::HardWallPotential>(0.,true));
    Vext->addFunctional(std::make_shared<flyft::HardWallPotential>(L,false));

    auto slv = std::make_shared<flyft::PiccardIteration>(fmt,Vext,0.01,10000,1.e-12);
    auto conv = slv->solve(state);

    if (!conv)
        {
        std::cout << "Failed to converge" << std::endl;
        return 1;
        }

    for (int idx=0; idx < mesh->shape(); ++idx)
        {
        std::cout << mesh->coordinate(idx);
        for (const auto& t : state->getTypes())
            {
            std::cout << " " << state->getField(t)->data()[idx];
            }
        std::cout << std::endl;
        }

    return 0;
    }
