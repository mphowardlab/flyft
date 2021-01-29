#include "flyft/field.h"

#include <algorithm>

namespace flyft
{

Field::Field(std::shared_ptr<Mesh> mesh)
    : data_(nullptr), alloc_shape_(-1)
    {
    setMesh(mesh);
    }

Field::~Field()
    {
    if (data_) delete data_;
    }

double* Field::data()
    {
    return data_;
    }

const double* Field::data() const
    {
    return static_cast<const double*>(data_);
    }

std::shared_ptr<Mesh> Field::getMesh()
    {
    return mesh_;
    }

void Field::setMesh(std::shared_ptr<Mesh> mesh)
    {
    mesh_ = mesh;
    resize(mesh_->shape());
    }

void Field::resize(int shape)
    {
    if (shape != alloc_shape_)
        {
        if (data_ != nullptr) delete data_;
        data_ = new double[shape];
        std::fill(data_, data_+shape, 0);
        }
    }

}
