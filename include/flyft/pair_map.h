#ifndef FLYFT_PAIR_MAP_H_
#define FLYFT_PAIR_MAP_H_

#include "flyft/tracked_object.h"

#include <map>
#include <string>
#include <utility>

namespace flyft
{

template<typename T>
class PairMap : public TrackedObject
    {
    private:
        // use ordered map instead of unordered map because string pair cannot be hashed automatically
        using data_type = std::map<std::pair<std::string,std::string>,T>;

    public:
        using key_type = typename data_type::key_type;
        using mapped_type = typename data_type::mapped_type;
        using value_type = typename data_type::value_type;
        using size_type = typename data_type::size_type;
        using difference_type = typename data_type::difference_type;
        using key_compare = typename data_type::key_compare;
        using allocator_type = typename data_type::allocator_type;
        using reference = typename data_type::reference;
        using const_reference = typename data_type::const_reference;
        using pointer = typename data_type::pointer;
        using const_pointer = typename data_type::const_pointer;
        using iterator = typename data_type::iterator;
        using const_iterator = typename data_type::const_iterator;
        using reverse_iterator = typename data_type::reverse_iterator;
        using const_reverse_iterator = typename data_type::const_reverse_iterator;

        key_type make_key(const std::string& i, const std::string& j) const
            {
            return (i <= j) ? key_type(i,j) : key_type(j,i);
            }

        key_type make_key(const key_type& key) const
            {
            return (key.first <= key.second) ? key : make_key(key.second,key.first);
            }

        mapped_type& operator[](const key_type& key)
            {
            token_.stageAndCommit();
            return data_[make_key(key)];
            }

        const mapped_type& operator()(const std::string& i, const std::string& j) const
            {
            return data_.at(make_key(i,j));
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

        iterator rbegin()
            {
            token_.stageAndCommit();
            return data_.rbegin();
            }

        iterator rend()
            {
            token_.stageAndCommit();
            return data_.rend();
            }

        const_iterator crbegin() const
            {
            return data_.crbegin();
            }

        const_iterator crend() const
            {
            return data_.crend();
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

#endif // FLYFT_PAIR_MAP_H_
