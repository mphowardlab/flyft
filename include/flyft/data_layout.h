#ifndef FLYFT_DATA_LAYOUT_H_
#define FLYFT_DATA_LAYOUT_H_

#include <iterator>
#include <type_traits>

namespace flyft
{
class DataLayout
    {
    public:
        DataLayout();
        DataLayout(int shape_, int buffer_shape_);
        DataLayout(const DataLayout& other);
        DataLayout(DataLayout&& other);
        DataLayout& operator=(const DataLayout& other);
        DataLayout& operator=(DataLayout&& other);

        int operator()(int idx) const;
        int shape() const;
        int buffer_shape() const;
        int full_shape() const;

        bool operator==(const DataLayout& other) const;
        bool operator!=(const DataLayout& other) const;

    private:
        int shape_;
        int buffer_shape_;
        int full_shape_;
    };

template<typename T>
class DataIterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = int;
        using value_type = typename std::remove_reference<T>::type;
        using pointer = value_type*;
        using reference = value_type&;

        DataIterator()
            : DataIterator(nullptr,DataLayout(0,0))
            {}

        DataIterator(value_type* data, const DataLayout& layout)
            : data_(data), layout_(layout), current_offset_(0)
            {}

        reference operator()(int idx) const
            {
            return data_[layout_(idx)];
            }

        reference operator[](difference_type n) const
            {
            // TODO: add method to layout to handle 1d offsets even if data is not 1d
            return data_[layout_(current_offset_+n)];
            }

        reference operator*() const
            {
            return *get();
            }

        pointer operator->() const
            {
            return get();
            }

        pointer get() const
            {
            return data_+layout_(current_offset_);
            }

        DataIterator& operator++()
            {
            ++current_offset_;
            return *this;
            }

        DataIterator operator++(int)
            {
            DataIterator tmp(*this);
            ++current_offset_;
            return tmp;
            }

        DataIterator& operator--()
            {
            --current_offset_;
            return *this;
            }

        DataIterator operator--(int)
            {
            DataIterator tmp(*this);
            --current_offset_;
            return tmp;
            }

        DataIterator& operator+=(difference_type n)
            {
            current_offset_ += n;
            return *this;
            }

        DataIterator operator+(difference_type n) const
            {
            DataIterator tmp(*this);
            tmp += n;
            return tmp;
            }

        friend DataIterator operator+(difference_type n, const DataIterator self)
            {
            return self+n;
            }

        DataIterator& operator-=(difference_type n)
            {
            current_offset_ -= n;
            return *this;
            }

        DataIterator operator-(difference_type n) const
            {
            DataIterator tmp(*this);
            tmp -= n;
            return tmp;
            }

        difference_type operator-(const DataIterator other) const
            {
            // TODO: add method to layout to figure out distance between two elements
            return (current_offset_-other.current_offset_);
            }

        operator bool() const
            {
            return (data_ != nullptr);
            }

        bool operator==(const DataIterator& other) const
            {
            return (get() == other.get());
            }

        bool operator!=(const DataIterator& other) const
            {
            return !(*this == other);
            }

    private:
        value_type* data_;
        DataLayout layout_;
        int current_offset_;
    };

}

#endif // FLYFT_DATA_LAYOUT_H_
