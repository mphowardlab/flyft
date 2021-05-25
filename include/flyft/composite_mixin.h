#ifndef FLYFT_COMPOSITE_MIXIN_H_
#define FLYFT_COMPOSITE_MIXIN_H_

#include <algorithm>
#include <memory>
#include <vector>

template<typename T>
class CompositeMixin
    {
    public:
        CompositeMixin()
            {
            }

        CompositeMixin(std::shared_ptr<T> object)
            {
            addObject(object);
            }

        CompositeMixin(const std::vector<std::shared_ptr<T>>& objects)
            {
            for (const auto& o : objects)
                {
                addObject(o);
                }
            }

        void addObject(std::shared_ptr<T> object)
            {
            if(std::find(objects_.begin(), objects_.end(), object) == objects_.end())
                {
                objects_.push_back(object);
                }
            }

        void removeObject(std::shared_ptr<T> object)
            {
            auto it = std::find(objects_.begin(), objects_.end(), object);
            if (it != objects_.end())
                {
                objects_.erase(it);
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
