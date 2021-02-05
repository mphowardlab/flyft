#include "flyft/field.h"
#include "flyft/hard_wall_potential.h"
#include "flyft/mesh.h"
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

    for (int i=0; i < state->getNumFields(); ++i)
        {
        auto field = state->getField(i);
        std::fill(field->data(), field->data()+mesh->shape(), 0.1);
        }

    auto Vlo = std::make_shared<flyft::HardWallPotential>(0.5, true, diameters);
    auto Vhi = std::make_shared<flyft::HardWallPotential>(4.5, false, diameters);
    Vlo->compute(state);
    Vhi->compute(state);
    for (int idx=0; idx < mesh->shape(); ++idx)
        {
        std::cout << mesh->coordinate(idx);
        for (int i=0; i < state->getNumFields(); ++i)
            {
            std::cout << " " << Vlo->getDerivative(i)->data()[idx];
            }
        for (int i=0; i < state->getNumFields(); ++i)
            {
            std::cout << " " << Vhi->getDerivative(i)->data()[idx];
            }
        std::cout << std::endl;
        }

    return 0;
    }
