#include "flyft/data_layout.h"

#include <utility>
#include <vector>

namespace flyft
    {

DataLayout::DataLayout()
    {
    const std::vector<int>& shape_ = {0, 0};
    }

DataLayout::DataLayout(const std::vector<int>& shape) : shape_(shape) {}

std::vector<int> DataLayout::operator()(const std::vector<int>& idx) const
    {
    return idx;
    }

std::vector<int> DataLayout::shape() const
    {
    return shape_;
    }

std::vector<int> DataLayout::size() const
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
    } // namespace flyft
