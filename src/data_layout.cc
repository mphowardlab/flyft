#include "flyft/data_layout.h"
#include <utility>

namespace flyft
{

DataLayout::DataLayout()
    : shape_(0)
    {
    }

DataLayout::DataLayout(int shape)
    : shape_(shape)
    {
    }

int DataLayout::operator()(int idx) const
    {
    return idx;
    }

int DataLayout::shape() const
    {
    return shape_;
    }

int DataLayout::size() const
    {
    return shape();
    }

bool DataLayout::operator==(const DataLayout& other) const
    {
    return (shape_ == other.shape_);
    }

bool DataLayout::operator!=(const DataLayout& other) const
    {
    return !(*this == other);
    }

}
