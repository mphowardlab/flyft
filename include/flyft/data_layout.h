#ifndef FLYFT_DATA_LAYOUT_H_
#define FLYFT_DATA_LAYOUT_H_

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
}

#endif // FLYFT_DATA_LAYOUT_H_
