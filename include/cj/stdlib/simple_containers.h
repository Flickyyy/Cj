/**
 * @file simple_containers.h
 * @brief Simple working containers for CJ language
 */

#ifndef CJ_SIMPLE_CONTAINERS_H
#define CJ_SIMPLE_CONTAINERS_H

#include "../common.h"
#include <vector>
#include <string>
#include <memory>

namespace cj {
namespace stdlib {

/**
 * @brief Simple vector implementation using C++ std::vector
 */
template<typename T>
class SimpleVector {
private:
    std::vector<T> data_;

public:
    SimpleVector() = default;
    SimpleVector(std::initializer_list<T> init) : data_(init) {}
    
    void Push(const T& value) { data_.push_back(value); }
    void Pop() { if (!data_.empty()) data_.pop_back(); }
    
    T& operator[](std::size_t index) { return data_[index]; }
    const T& operator[](std::size_t index) const { return data_[index]; }
    
    std::size_t GetSize() const { return data_.size(); }
    bool Empty() const { return data_.empty(); }
    void Clear() { data_.clear(); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
};

/**
 * @brief Simple string implementation
 */
class SimpleString {
private:
    std::string data_;

public:
    SimpleString() = default;
    SimpleString(const char* str) : data_(str) {}
    SimpleString(const std::string& str) : data_(str) {}
    
    const char* CStr() const { return data_.c_str(); }
    std::size_t Length() const { return data_.length(); }
    bool Empty() const { return data_.empty(); }
    
    SimpleString operator+(const SimpleString& other) const {
        return SimpleString(data_ + other.data_);
    }
    
    SimpleString& operator+=(const SimpleString& other) {
        data_ += other.data_;
        return *this;
    }
    
    bool operator==(const SimpleString& other) const {
        return data_ == other.data_;
    }
    
    char operator[](std::size_t index) const { return data_[index]; }
};

/**
 * @brief Simple stack implementation
 */
template<typename T>
class SimpleStack {
private:
    SimpleVector<T> data_;

public:
    void Push(const T& value) { data_.Push(value); }
    void Pop() { data_.Pop(); }
    
    T& Top() { return data_[data_.GetSize() - 1]; }
    const T& Top() const { return data_[data_.GetSize() - 1]; }
    
    std::size_t GetSize() const { return data_.GetSize(); }
    bool Empty() const { return data_.Empty(); }
};

/**
 * @brief Simple queue implementation
 */
template<typename T>
class SimpleQueue {
private:
    SimpleVector<T> data_;
    std::size_t front_index_;

public:
    SimpleQueue() : front_index_(0) {}
    
    void Push(const T& value) { data_.Push(value); }
    
    void Pop() {
        if (!Empty()) {
            front_index_++;
            // Clean up when queue becomes mostly empty
            if (front_index_ > data_.GetSize() / 2) {
                SimpleVector<T> new_data;
                for (std::size_t i = front_index_; i < data_.GetSize(); ++i) {
                    new_data.Push(data_[i]);
                }
                data_ = std::move(new_data);
                front_index_ = 0;
            }
        }
    }
    
    T& Front() { return data_[front_index_]; }
    const T& Front() const { return data_[front_index_]; }
    
    std::size_t GetSize() const { return data_.GetSize() - front_index_; }
    bool Empty() const { return front_index_ >= data_.GetSize(); }
};

} // namespace stdlib
} // namespace cj

#endif // CJ_SIMPLE_CONTAINERS_H