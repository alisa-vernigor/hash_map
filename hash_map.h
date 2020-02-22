#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <list>
#include <stdexcept>
#include <vector>

template <class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    using iterator =
        typename std::list<std::pair<const KeyType, ValueType>>::iterator;
    using const_iterator =
        typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;

private:
    const size_t kMinimalSize = 8;
    const size_t kAlpha = 2;
    std::vector<std::list<
        typename std::list<std::pair<const KeyType, ValueType>>::iterator>>
        table_;
    std::list<std::pair<const KeyType, ValueType>> elements_list_;
    Hash hasher_;
    size_t table_size_;
    size_t elements_number_;

public:
    HashMap(const Hash hash_function = Hash()) :
        hasher_(hash_function),
        table_size_(kMinimalSize),
        elements_number_(0) {
        table_.resize(table_size_);
    }

    template <typename IteratorType>
    HashMap(IteratorType begin, IteratorType end,
        const Hash hash_function = Hash()) :
        hasher_(hash_function),
        table_size_(kMinimalSize),
        elements_number_(0) {
        table_.resize(table_size_);

        insert_all_elements(begin, end);
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list,
        const Hash& hash_function = Hash()) : 
        HashMap(list.begin(), list.end(), hash_function) {
    }

    HashMap(const HashMap& other) {
        *this = other;
    }

    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            clear();
            hasher_ = other.hasher_;
            table_size_ = other.table_size_;
            table_.resize(table_size_);

            insert_all_elements(other.elements_list_.begin(), other.elements_list_.end());
        }
        return *this;
    }

    Hash hash_function() const {
        return hasher_;
    }

    bool empty() const {
        return elements_number_ == 0;
    }

    size_t size() const {
        return elements_number_;
    }

    void insert(std::pair<KeyType, ValueType> element) {
        if (find(element.first) == end()) {
            size_t table_index = hasher_(element.first) % table_size_;
            elements_list_.push_back(element);
            table_[table_index].push_back(std::prev(elements_list_.end()));
            elements_number_++;

            if (elements_number_ >= kAlpha * table_size_) {
                table_resize();
            }
        }
    }

    template <typename IteratorType>
    void insert_all_elements(IteratorType begin, IteratorType end) {
        for (auto element = begin; element != end; element++)
            insert(*element);
    }

    ValueType& operator[](KeyType key) {
        insert({ key, {} });
        return find(key)->second;
    }

    const ValueType& at(KeyType key) const {
        auto element = find(key);

        if (element != end()) {
            return element->second;
        }

        throw std::out_of_range("There is no such element in the HashMap.");
    }

    iterator begin() {
        return elements_list_.begin();
    }

    iterator end() {
        return elements_list_.end();
    }

    const_iterator begin() const {
        return const_cast<HashMap*>(this)->begin();
    }

    const_iterator end() const {
        return const_cast<HashMap*>(this)->end();
    }

    const_iterator find(KeyType key) const {
        return const_cast<HashMap*>(this)->find(key);
    }

    iterator find(KeyType key) {
        size_t table_index = hasher_(key) % table_size_;

        for (auto&& element : table_[table_index]) {
            if (element->first == key) {
                return element;
            }
        }

        return end();
    }

    void erase(KeyType key) {
        size_t table_index = hasher_(key) % table_size_;

        for (auto element = table_[table_index].begin();
            element != table_[table_index].end();
            element++)
            if ((*element)->first == key) {
                elements_list_.erase(*element);
                table_[table_index].erase(element);
                elements_number_--;
                break;
            }
    }

    void table_resize() {
        table_size_ *= kAlpha;
        table_.clear();
        table_.resize(table_size_);

        for (auto element = elements_list_.begin();
            element != elements_list_.end();
            element++) {
            table_[hasher_(element->first) % table_size_].push_back(element);
        }
    }

    void clear() {
        table_size_ = kMinimalSize;
        elements_number_ = 0;
        elements_list_.clear();
        table_.clear();
        table_.resize(kMinimalSize);
    }
};
#endif
