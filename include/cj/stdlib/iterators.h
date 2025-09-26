/**
 * @file iterators.h
 * @brief Iterator system for CJ language standard library
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_STDLIB_ITERATORS_H
#define CJ_STDLIB_ITERATORS_H

#include "../common.h"
#include <iterator>
#include <functional>

namespace cj {
namespace stdlib {
namespace iterators {

/**
 * @brief Iterator categories
 */
enum class IteratorCategory {
    INPUT,
    OUTPUT,
    FORWARD,
    BIDIRECTIONAL,
    RANDOM_ACCESS
};

/**
 * @brief Iterator traits for type information
 */
template<typename Iterator>
struct IteratorTraits {
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
    using category = typename Iterator::category;
};

// Specialization for raw pointers
template<typename T>
struct IteratorTraits<T*> {
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    using category = IteratorCategory;
};

/**
 * @brief Base iterator class
 */
template<typename ValueType, IteratorCategory Category = IteratorCategory::FORWARD>
class Iterator {
public:
    using value_type = ValueType;
    using difference_type = std::ptrdiff_t;
    using pointer = ValueType*;
    using reference = ValueType&;
    using category = Category;
    
    virtual ~Iterator() = default;
    
    virtual reference operator*() = 0;
    virtual pointer operator->() = 0;
    virtual Iterator& operator++() = 0;
    
    virtual bool operator==(const Iterator& other) const = 0;
    virtual bool operator!=(const Iterator& other) const { return !(*this == other); }
};

/**
 * @brief Range-based iterator adapter
 */
template<typename Container>
class RangeIterator {
private:
    using ContainerIterator = typename Container::iterator;
    ContainerIterator current_;
    ContainerIterator end_;

public:
    using value_type = typename Container::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = typename Container::value_type*;
    using reference = typename Container::value_type&;
    
    RangeIterator(ContainerIterator current, ContainerIterator end)
        : current_(current), end_(end) {}
    
    reference operator*() { return *current_; }
    pointer operator->() { return &(*current_); }
    
    RangeIterator& operator++() {
        ++current_;
        return *this;
    }
    
    RangeIterator operator++(int) {
        RangeIterator temp = *this;
        ++current_;
        return temp;
    }
    
    bool operator==(const RangeIterator& other) const {
        return current_ == other.current_;
    }
    
    bool operator!=(const RangeIterator& other) const {
        return current_ != other.current_;
    }
    
    bool AtEnd() const { return current_ == end_; }
};

/**
 * @brief Filtering iterator
 */
template<typename Iterator, typename Predicate>
class FilterIterator {
private:
    Iterator current_;
    Iterator end_;
    Predicate predicate_;
    
    void FindNext() {
        while (current_ != end_ && !predicate_(*current_)) {
            ++current_;
        }
    }

public:
    using value_type = typename IteratorTraits<Iterator>::value_type;
    using difference_type = typename IteratorTraits<Iterator>::difference_type;
    using pointer = typename IteratorTraits<Iterator>::pointer;
    using reference = typename IteratorTraits<Iterator>::reference;
    
    FilterIterator(Iterator current, Iterator end, Predicate pred)
        : current_(current), end_(end), predicate_(pred) {
        FindNext();
    }
    
    reference operator*() { return *current_; }
    pointer operator->() { return &(*current_); }
    
    FilterIterator& operator++() {
        ++current_;
        FindNext();
        return *this;
    }
    
    FilterIterator operator++(int) {
        FilterIterator temp = *this;
        ++current_;
        FindNext();
        return temp;
    }
    
    bool operator==(const FilterIterator& other) const {
        return current_ == other.current_;
    }
    
    bool operator!=(const FilterIterator& other) const {
        return current_ != other.current_;
    }
};

/**
 * @brief Transform iterator (map)
 */
template<typename Iterator, typename Transform>
class TransformIterator {
private:
    Iterator current_;
    Transform transform_;

public:
    using value_type = std::invoke_result_t<Transform, typename IteratorTraits<Iterator>::reference>;
    using difference_type = typename IteratorTraits<Iterator>::difference_type;
    using pointer = value_type*;
    using reference = value_type;
    
    TransformIterator(Iterator current, Transform transform)
        : current_(current), transform_(transform) {}
    
    reference operator*() { return transform_(*current_); }
    
    TransformIterator& operator++() {
        ++current_;
        return *this;
    }
    
    TransformIterator operator++(int) {
        TransformIterator temp = *this;
        ++current_;
        return temp;
    }
    
    bool operator==(const TransformIterator& other) const {
        return current_ == other.current_;
    }
    
    bool operator!=(const TransformIterator& other) const {
        return current_ != other.current_;
    }
};

/**
 * @brief Enumerate iterator (with index)
 */
template<typename Iterator>
class EnumerateIterator {
private:
    Iterator current_;
    Size index_;

public:
    using value_type = std::pair<Size, typename IteratorTraits<Iterator>::reference>;
    using difference_type = typename IteratorTraits<Iterator>::difference_type;
    using pointer = value_type*;
    using reference = value_type;
    
    EnumerateIterator(Iterator current, Size index = 0)
        : current_(current), index_(index) {}
    
    reference operator*() { return std::make_pair(index_, *current_); }
    
    EnumerateIterator& operator++() {
        ++current_;
        ++index_;
        return *this;
    }
    
    EnumerateIterator operator++(int) {
        EnumerateIterator temp = *this;
        ++current_;
        ++index_;
        return temp;
    }
    
    bool operator==(const EnumerateIterator& other) const {
        return current_ == other.current_;
    }
    
    bool operator!=(const EnumerateIterator& other) const {
        return current_ != other.current_;
    }
};

/**
 * @brief Zip iterator (iterate over multiple containers)
 */
template<typename Iterator1, typename Iterator2>
class ZipIterator {
private:
    Iterator1 current1_;
    Iterator2 current2_;

public:
    using value_type = std::pair<typename IteratorTraits<Iterator1>::reference,
                                typename IteratorTraits<Iterator2>::reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type;
    
    ZipIterator(Iterator1 current1, Iterator2 current2)
        : current1_(current1), current2_(current2) {}
    
    reference operator*() { return std::make_pair(*current1_, *current2_); }
    
    ZipIterator& operator++() {
        ++current1_;
        ++current2_;
        return *this;
    }
    
    ZipIterator operator++(int) {
        ZipIterator temp = *this;
        ++current1_;
        ++current2_;
        return temp;
    }
    
    bool operator==(const ZipIterator& other) const {
        return current1_ == other.current1_ && current2_ == other.current2_;
    }
    
    bool operator!=(const ZipIterator& other) const {
        return !(*this == other);
    }
};

/**
 * @brief Reverse iterator
 */
template<typename Iterator>
class ReverseIterator {
private:
    Iterator current_;

public:
    using value_type = typename IteratorTraits<Iterator>::value_type;
    using difference_type = typename IteratorTraits<Iterator>::difference_type;
    using pointer = typename IteratorTraits<Iterator>::pointer;
    using reference = typename IteratorTraits<Iterator>::reference;
    
    explicit ReverseIterator(Iterator current) : current_(current) {}
    
    reference operator*() { 
        Iterator temp = current_;
        return *--temp;
    }
    
    pointer operator->() { 
        Iterator temp = current_;
        return &(*--temp);
    }
    
    ReverseIterator& operator++() {
        --current_;
        return *this;
    }
    
    ReverseIterator operator++(int) {
        ReverseIterator temp = *this;
        --current_;
        return temp;
    }
    
    ReverseIterator& operator--() {
        ++current_;
        return *this;
    }
    
    ReverseIterator operator--(int) {
        ReverseIterator temp = *this;
        ++current_;
        return temp;
    }
    
    bool operator==(const ReverseIterator& other) const {
        return current_ == other.current_;
    }
    
    bool operator!=(const ReverseIterator& other) const {
        return current_ != other.current_;
    }
};

/**
 * @brief Range class for iterator-based operations
 */
template<typename Iterator>
class Range {
private:
    Iterator begin_;
    Iterator end_;

public:
    using iterator = Iterator;
    using value_type = typename IteratorTraits<Iterator>::value_type;
    
    Range(Iterator begin, Iterator end) : begin_(begin), end_(end) {}
    
    Iterator begin() const { return begin_; }
    Iterator end() const { return end_; }
    
    bool Empty() const { return begin_ == end_; }
    
    Size Size() const {
        Size count = 0;
        for (auto it = begin_; it != end_; ++it) {
            ++count;
        }
        return count;
    }
    
    // Functional operations
    template<typename Predicate>
    auto Filter(Predicate pred) const {
        return Range<FilterIterator<Iterator, Predicate>>{
            FilterIterator<Iterator, Predicate>{begin_, end_, pred},
            FilterIterator<Iterator, Predicate>{end_, end_, pred}
        };
    }
    
    template<typename Transform>
    auto Map(Transform transform) const {
        return Range<TransformIterator<Iterator, Transform>>{
            TransformIterator<Iterator, Transform>{begin_, transform},
            TransformIterator<Iterator, Transform>{end_, transform}
        };
    }
    
    auto Enumerate() const {
        return Range<EnumerateIterator<Iterator>>{
            EnumerateIterator<Iterator>{begin_, 0},
            EnumerateIterator<Iterator>{end_, Size(-1)}
        };
    }
    
    auto Reverse() const {
        return Range<ReverseIterator<Iterator>>{
            ReverseIterator<Iterator>{end_},
            ReverseIterator<Iterator>{begin_}
        };
    }
    
    template<typename OtherIterator>
    auto Zip(const Range<OtherIterator>& other) const {
        return Range<ZipIterator<Iterator, OtherIterator>>{
            ZipIterator<Iterator, OtherIterator>{begin_, other.begin()},
            ZipIterator<Iterator, OtherIterator>{end_, other.end()}
        };
    }
    
    // Reduction operations
    template<typename T, typename BinaryOp>
    T Reduce(T initial, BinaryOp op) const {
        T result = initial;
        for (auto it = begin_; it != end_; ++it) {
            result = op(result, *it);
        }
        return result;
    }
    
    template<typename BinaryOp>
    value_type Reduce(BinaryOp op) const {
        if (Empty()) {
            throw std::runtime_error("Cannot reduce empty range");
        }
        
        auto it = begin_;
        value_type result = *it;
        ++it;
        
        for (; it != end_; ++it) {
            result = op(result, *it);
        }
        
        return result;
    }
    
    // Find operations
    template<typename Predicate>
    Iterator Find(Predicate pred) const {
        for (auto it = begin_; it != end_; ++it) {
            if (pred(*it)) {
                return it;
            }
        }
        return end_;
    }
    
    Iterator Find(const value_type& value) const {
        return Find([&value](const value_type& item) { return item == value; });
    }
    
    // Check operations
    template<typename Predicate>
    bool Any(Predicate pred) const {
        return Find(pred) != end_;
    }
    
    template<typename Predicate>
    bool All(Predicate pred) const {
        for (auto it = begin_; it != end_; ++it) {
            if (!pred(*it)) {
                return false;
            }
        }
        return true;
    }
    
    template<typename Predicate>
    Size Count(Predicate pred) const {
        Size count = 0;
        for (auto it = begin_; it != end_; ++it) {
            if (pred(*it)) {
                ++count;
            }
        }
        return count;
    }
    
    Size Count(const value_type& value) const {
        return Count([&value](const value_type& item) { return item == value; });
    }
};

// Helper functions for creating ranges
template<typename Container>
auto MakeRange(Container& container) {
    return Range<typename Container::iterator>{container.begin(), container.end()};
}

template<typename Container>
auto MakeRange(const Container& container) {
    return Range<typename Container::const_iterator>{container.begin(), container.end()};
}

template<typename Iterator>
auto MakeRange(Iterator begin, Iterator end) {
    return Range<Iterator>{begin, end};
}

// Generator iterator for creating sequences
template<typename T>
class GeneratorIterator {
private:
    T current_;
    T step_;
    T end_;

public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    
    GeneratorIterator(T start, T step, T end)
        : current_(start), step_(step), end_(end) {}
    
    T operator*() const { return current_; }
    
    GeneratorIterator& operator++() {
        current_ += step_;
        return *this;
    }
    
    GeneratorIterator operator++(int) {
        GeneratorIterator temp = *this;
        current_ += step_;
        return temp;
    }
    
    bool operator==(const GeneratorIterator& other) const {
        return current_ >= end_ && other.current_ >= other.end_;
    }
    
    bool operator!=(const GeneratorIterator& other) const {
        return !(*this == other);
    }
};

// Helper functions for creating generator ranges
template<typename T>
auto Range(T end) {
    return MakeRange(GeneratorIterator<T>{T{0}, T{1}, end}, 
                    GeneratorIterator<T>{end, T{1}, end});
}

template<typename T>
auto Range(T start, T end, T step = T{1}) {
    return MakeRange(GeneratorIterator<T>{start, step, end}, 
                    GeneratorIterator<T>{end, step, end});
}

} // namespace iterators
} // namespace stdlib
} // namespace cj

#endif // CJ_STDLIB_ITERATORS_H