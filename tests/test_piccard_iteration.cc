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
    const std::vector<double> etas = {0.4257};
    const std::vector<double> diameters = {1.0};

    auto mesh = std::make_shared<const flyft::Mesh>(L,1000);
    auto state = std::make_shared<flyft::State>(mesh,etas.size());

    // initialize density profiles
    for (int i=0; i < state->getNumFields(); ++i)
        {
        const double d = diameters[i];
        const double rho = 6.*etas[i]/(M_PI*d*d*d);

        // fill initial density
        auto data = state->getField(i)->data();
        std::fill(data, data+mesh->shape(), rho);

        // set diameter and average N
        state->setDiameter(i,d);
        state->setConstraint(i,L*rho,flyft::State::Constraint::N);
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
        for (int i=0; i < state->getNumFields(); ++i)
            {
            std::cout << " " << state->getField(i)->data()[idx];
            }
        std::cout << std::endl;
        }

    return 0;
    }
