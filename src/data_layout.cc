#include "flyft/data_layout.h"

#include <utility>
#include <vector>

namespace flyft
    {

DataLayout::DataLayout() {}

DataLayout::DataLayout(const std::vector<int>& shape) : shape_(shape) {}

int DataLayout::operator()(const std::vector<int>& idx) const
    {
    int N = idx.size();
    int one_d_idx = 0;
    int stride = 1;
    for (int k = N - 1; k >= 0; --k)
        {
        one_d_idx += idx[k] * stride;
        stride *= shape_[k];
        }

    return one_d_idx;
    }

std::vector<int> DataLayout::shape() const
    {
    return shape_;
    }

int DataLayout::size() const
    {
    return std::accumulate(shape_.begin(), shape_.end(), 1, std::multiplies<int>());
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
