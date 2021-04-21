#include "flyft/composite_external_potential.h"

#include <algorithm>
#include <iostream>

namespace flyft
{

CompositeExternalPotential::CompositeExternalPotential()
    {
    }

CompositeExternalPotential::CompositeExternalPotential(std::shared_ptr<ExternalPotential> potential)
    {
    addPotential(potential);
    }

CompositeExternalPotential::CompositeExternalPotential(const std::vector<std::shared_ptr<ExternalPotential>>& potentials)
    {
    for (const auto& f : potentials)
        {
        addPotential(f);
        }
    }

void CompositeExternalPotential::addPotential(std::shared_ptr<ExternalPotential> potential)
    {
    if(std::find(potentials_.begin(), potentials_.end(), potential) == potentials_.end())
        {
        potentials_.push_back(potential);
        }
    }

void CompositeExternalPotential::removePotential(std::shared_ptr<ExternalPotential> potential)
    {
    auto it = std::find(potentials_.begin(), potentials_.end(), potential);
    if (it != potentials_.end())
        {
        potentials_.erase(it);
        }
    }

void CompositeExternalPotential::clearPotentials()
    {
    potentials_.clear();
    }

const std::vector<std::shared_ptr<ExternalPotential>>& CompositeExternalPotential::getPotentials()
    {
    return potentials_;
    }

void CompositeExternalPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // size temporary array
    auto mesh = state->getMesh();
    if (!Vtmp_)
        {
        Vtmp_ = std::make_shared<Field>(mesh->shape());
        }
    Vtmp_->reshape(mesh->shape());

    // fill total potential with zeros
    auto data = V->data();
    std::fill(data,data+mesh->shape(),0.);

    for (const auto& potential : potentials_)
        {
        std::cout << "here" << std::endl;
        potential->potential(Vtmp_,type,state);
        auto tmp = Vtmp_->data();
        for (int idx=0; idx < mesh->shape(); ++idx)
            {
            data[idx] += tmp[idx];
            }
        }
    }

}
