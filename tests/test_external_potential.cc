#include "flyft/field.h"
#include "flyft/hard_wall_potential.h"
#include "flyft/mesh.h"
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

    auto mesh = std::make_shared<const flyft::Mesh>(5.0,25);
    auto state = std::make_shared<flyft::State>(mesh,types);
    auto Vlo = std::make_shared<flyft::HardWallPotential>(0.5, true);
    auto Vhi = std::make_shared<flyft::HardWallPotential>(4.5, false);
    Vlo->setDiameters(diameters);
    Vhi->setDiameters(diameters);

    for (const auto& t : state->getTypes())
        {
        auto field = state->getField(t);
        std::fill(field->data(), field->data()+mesh->shape(), 0.1);
        }

    Vlo->compute(state);
    Vhi->compute(state);
    for (int idx=0; idx < mesh->shape(); ++idx)
        {
        std::cout << mesh->coordinate(idx);
        for (const auto& t : state->getTypes())
            {
            std::cout << " " << Vlo->getDerivative(t)->data()[idx];
            }
        for (const auto& t : state->getTypes())
            {
            std::cout << " " << Vhi->getDerivative(t)->data()[idx];
            }
        std::cout << std::endl;
        }

    return 0;
    }
