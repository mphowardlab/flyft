#ifndef FLYFT_TYPE_MAP_H_
#define FLYFT_TYPE_MAP_H_

#include "flyft/tracked_object.h"

#include <string>
#include <unordered_map>

namespace flyft
{

template<typename T>
class TypeMap : public TrackedObject
    {
    private:
        using data_type = std::unordered_map<std::string,T>;

    public:
        using key_type = typename data_type::key_type;
        using mapped_type = typename data_type::mapped_type;
        using value_type = typename data_type::value_type;
        using size_type = typename data_type::size_type;
        using difference_type = typename data_type::difference_type;
        using hasher = typename data_type::hasher;
        using key_equal = typename data_type::key_equal;
        using allocator_type = typename data_type::allocator_type;
        using reference = typename data_type::reference;
        using const_reference = typename data_type::const_reference;
        using pointer = typename data_type::pointer;
        using const_pointer = typename data_type::const_pointer;
        using iterator = typename data_type::iterator;
        using const_iterator = typename data_type::const_iterator;

        key_type make_key(const std::string& i) const
            {
            return i;
            }

        mapped_type& operator[](const key_type& key)
            {
            token_.stageAndCommit();
            return data_[key];
            }

        const mapped_type& operator()(const std::string& i) const
            {
            return get(i);
            }

        const mapped_type& get(const std::string& i) const
            {
            return data_.at(make_key(i));
            }

        iterator begin()
            {
            token_.stageAndCommit();
            return data_.begin();
            }

        iterator end()
            {
            token_.stageAndCommit();
            return data_.end();
            }

        const_iterator cbegin() const
            {
            return data_.cbegin();
            }

        const_iterator cend() const
            {
            return data_.cend();
            }

        bool empty() const
            {
            return data_.empty();
            }

        size_type size() const
            {
            return data_.size();
            }

        void clear()
            {
            data_.clear();
            token_.stageAndCommit();
            }

        template<class... Args>
        std::pair<iterator,bool> emplace(Args&&... args)
            {
            token_.stageAndCommit();
            return data_.emplace(std::forward<Args>(args)...);
            }

        iterator find(const key_type& key)
            {
            token_.stageAndCommit();
            return data_.find(make_key(key));
            }

        const_iterator find(const key_type& key) const
            {
            return data_.find(make_key(key));
            }

        bool contains(const key_type& key) const
            {
            return (data_.find(make_key(key)) != data_.cend());
            }

        iterator erase(const_iterator pos)
            {
            token_.stageAndCommit();
            return data_.erase(pos);
            }

    private:
        data_type data_;

    };

}

#endif // FLYFT_TYPE_MAP_H_
