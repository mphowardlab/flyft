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

    // functionals
    auto omega = std::make_shared<flyft::GrandPotential>();

    auto fmt = std::make_shared<flyft::RosenfeldFMT>();
    fmt->setDiameters(diameters);
    omega->setExcessFunctional(fmt);

    auto Vext = std::make_shared<flyft::CompositeFunctional>();
    auto Vlo = std::make_shared<flyft::HardWallPotential>(0.,true);
    auto Vhi = std::make_shared<flyft::HardWallPotential>(L,false);
    Vlo->setDiameters(diameters);
    Vhi->setDiameters(diameters);
    Vext->addFunctional(Vlo);
    Vext->addFunctional(Vhi);
    omega->setExternalPotential(Vext);

    // initialize density profiles
    for (const auto& t : state->getTypes())
        {
        const double d = diameters[t];
        const double rho = 6.*etas[t]/(M_PI*d*d*d);

        // fill initial density
        auto data = state->getField(t)->data();
        std::fill(data, data+mesh->shape(), rho);

        // set diameter and average N
        omega->setConstraint(t,(L-d)*rho);
        omega->setConstraintType(t,flyft::GrandPotential::Constraint::N);
        }

    auto slv = std::make_shared<flyft::PiccardIteration>(omega,0.01,10000,1.e-12);
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
