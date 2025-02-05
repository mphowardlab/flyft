#ifndef FLYFT_COMPOSITE_MIXIN_H_
#define FLYFT_COMPOSITE_MIXIN_H_

#include <algorithm>
#include <memory>
#include <vector>

template<typename T>
class CompositeMixin
    {
    public:
    CompositeMixin() {}

    bool addObject(std::shared_ptr<T> object)
        {
        if (std::find(objects_.begin(), objects_.end(), object) == objects_.end())
            {
            objects_.push_back(object);
            return true;
            }
        else
            {
            return false;
            }
        }

    bool removeObject(std::shared_ptr<T> object)
        {
        auto it = std::find(objects_.begin(), objects_.end(), object);
        if (it != objects_.end())
            {
            objects_.erase(it);
            return true;
            }
        else
            {
            return false;
            }
        }

    void clearObjects()
        {
        objects_.clear();
        }

    const std::vector<std::shared_ptr<T>>& getObjects()
        {
        return objects_;
        }

    protected:
    std::vector<std::shared_ptr<T>> objects_;
    };

#endif // FLYFT_COMPOSITE_MIXIN_H_
