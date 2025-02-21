#include "flyft/data_layout.h"

#include <utility>
#include <vector>

namespace flyft
    {

//! Default constructor
DataLayout::DataLayout()
    {
    const std::vector<int>& shape_ = {0, 0};
    }
//! Overloading DataLayout constructor
DataLayout::DataLayout(const std::vector<int>& shape) : shape_(shape) {}

int DataLayout::operator()(const std::vector<int>& idx) const
    {
    //! Strided indexing scheme for converting
    //! N dimensional array into one dimensional array
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
