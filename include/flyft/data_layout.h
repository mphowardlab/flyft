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
        explicit DataLayout(int shape_);

        int operator()(int idx) const;
        int shape() const;
        int size() const;

        bool operator==(const DataLayout& other) const;
        bool operator!=(const DataLayout& other) const;

    private:
        int shape_;
    };

}

#endif // FLYFT_DATA_LAYOUT_H_
