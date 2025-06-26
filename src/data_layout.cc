#include "flyft/data_layout.h"

#include <utility>

namespace flyft
    {

DataLayout::DataLayout() : shape_(nullptr), num_dimensions_(0), size_(0) {}

DataLayout::DataLayout(const int* shape, char num_dimensions)
    {
    reset(shape, num_dimensions);
    }

DataLayout::DataLayout(const DataLayout& other)
    {
    reset(other.shape_, other.num_dimensions_);
    }

DataLayout& DataLayout::operator=(const DataLayout& other)
    {
    if (this != &other)
        {
        reset(other.shape_, other.num_dimensions_);
        }
    return *this;
    }

DataLayout::DataLayout(const DataLayout&& other)
    : shape_(std::move(other.shape_)), num_dimensions_(std::move(other.num_dimensions_)),
      size_(std::move(other.size_))
    {
    }

DataLayout& DataLayout::operator=(const DataLayout&& other)
    {
    shape_ = std::move(other.shape_);
    num_dimensions_ = std::move(other.num_dimensions_);
    size_ = std::move(other.size_);
    return *this;
    }

DataLayout::~DataLayout()
    {
    delete[] shape_;
    }

size_t DataLayout::operator()(const int* multi_index) const
    {
    size_t flat_index;
    if (num_dimensions_ > 1)
        {
        // multiply out using row-major ordering
        flat_index = 0;
        size_t stride = 1;
        for (char dim = num_dimensions_ - 1; dim >= 0; --dim)
            {
            flat_index += multi_index[dim] * stride;
            stride *= shape_[dim];
            }
        }
    else
        {
        flat_index = multi_index[0];
        }
    return flat_index;
    }

size_t DataLayout::operator()(const int* multi_index, const int* offset) const
    {
    // use method without addition if there is no starting index.
    if (!offset)
        {
        return this->operator()(multi_index);
        }

    // otherwise, do calculation with starting offset added
    size_t flat_index;
    if (num_dimensions_ > 1)
        {
        // multiply out using row-major ordering
        flat_index = 0;
        size_t stride = 1;
        for (char dim = num_dimensions_ - 1; dim >= 0; --dim)
            {
            flat_index += (offset[dim] + multi_index[dim]) * stride;
            stride *= shape_[dim];
            }
        }
    else
        {
        flat_index = offset[0] + multi_index[0];
        }
    return flat_index;
    }

void DataLayout::operator()(int* multi_index, size_t flat_index) const
    {
    if (num_dimensions_ > 1)
        {
        // repeatedly floor divide the flat index by stride associated with it
        size_t idx = flat_index;
        size_t stride = size_;
        for (char dim = 0; dim < num_dimensions_ - 1; ++dim)
            {
            stride /= shape_[dim];
            multi_index[dim] = idx / stride;
            idx -= multi_index[dim] * stride;
            }
        }
    else
        {
        multi_index[0] = flat_index;
        }
    }

void DataLayout::operator()(int* multi_index, size_t flat_index, const int* offset) const
    {
    this->operator()(multi_index, flat_index);
    if (offset)
        {
        for (char dim = 0; dim < num_dimensions_ - 1; ++dim)
            {
            multi_index[dim] -= offset[dim];
            }
        }
    }

const int* DataLayout::shape() const
    {
    return shape_;
    }

char DataLayout::num_dimensions() const
    {
    return num_dimensions_;
    }

size_t DataLayout::size() const
    {
    return size_;
    }

bool DataLayout::operator==(const DataLayout& other) const
    {
    // dimensionality & size must match as first pass
    bool same_layout = (num_dimensions_ == other.num_dimensions_ && size_ == other.size_);
    if (same_layout)
        {
        // then, shape must match along all dimensions
        if (shape_ && other.shape_)
            {
            for (char i = 0; i < num_dimensions_; ++i)
                {
                if (shape_[i] != other.shape_[i])
                    {
                    same_layout = false;
                    }
                }
            }
        // or, both pointers must be null
        else if (!(!shape_ && !other.shape_))
            {
            same_layout = false;
            }
        }

    return same_layout;
    }

bool DataLayout::operator!=(const DataLayout& other) const
    {
    return !(*this == other);
    }

void DataLayout::reset(const int* shape, char num_dimensions)
    {
    if (shape && num_dimensions > 0)
        {
        // free shape if it is allocated but the wrong size
        if (shape_ && num_dimensions_ != num_dimensions)
            {
            delete shape_;
            }

        // (re-)allocate shape memory if needed
        if (!shape_)
            {
            shape_ = new int[num_dimensions];
            }

        // always copy shape values and recompute size
        num_dimensions_ = num_dimensions;
        size_ = 1;
        for (char i = 0; i < num_dimensions; ++i)
            {
            shape_[i] = shape[i];
            size_ *= shape[i];
            }
        }
    else
        {
        if (shape_)
            {
            delete shape_;
            }
        num_dimensions_ = 0;
        size_ = 0;
        }
    }

    } // namespace flyft
