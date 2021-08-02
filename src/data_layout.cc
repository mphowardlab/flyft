#include "flyft/data_layout.h"
#include <utility>

namespace flyft
{

DataLayout::DataLayout()
    : shape_(0),
      buffer_shape_(0),
      full_shape_(0)
    {
    }

DataLayout::DataLayout(int shape, int buffer_shape)
    : shape_(shape),
      buffer_shape_(buffer_shape),
      full_shape_(shape_+2*buffer_shape_)
    {
    }

DataLayout::DataLayout(const DataLayout& other)
    : shape_(other.shape_),
      buffer_shape_(other.buffer_shape_),
      full_shape_(other.full_shape_)
    {
    }

DataLayout::DataLayout(DataLayout&& other)
    : shape_(std::move(other.shape_)),
      buffer_shape_(std::move(other.buffer_shape_)),
      full_shape_(std::move(other.full_shape_))
    {
    }

DataLayout& DataLayout::operator=(const DataLayout& other)
    {
    if (&other != this)
        {
        shape_ = other.shape_;
        buffer_shape_ = other.buffer_shape_;
        full_shape_ = other.full_shape_;
        }
    return *this;
    }

DataLayout& DataLayout::operator=(DataLayout&& other)
    {
    shape_ = std::move(other.shape_);
    buffer_shape_ = std::move(other.buffer_shape_);
    full_shape_ = std::move(other.full_shape_);
    return *this;
    }

int DataLayout::operator()(int idx) const
    {
    return buffer_shape_ + idx;
    }

int DataLayout::shape() const
    {
    return shape_;
    }

int DataLayout::buffer_shape() const
    {
    return buffer_shape_;
    }

int DataLayout::full_shape() const
    {
    return full_shape_;
    }

bool DataLayout::operator==(const DataLayout& other) const
    {
    return (shape_ == other.shape_ && buffer_shape_ == other.buffer_shape_);
    }

bool DataLayout::operator!=(const DataLayout& other) const
    {
    return (shape_ != other.shape_ || buffer_shape_ != other.buffer_shape_);
    }

}
