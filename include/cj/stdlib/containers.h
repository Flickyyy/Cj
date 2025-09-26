/**
 * @file containers.h
 * @brief Standard library containers for CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_STDLIB_CONTAINERS_H
#define CJ_STDLIB_CONTAINERS_H

#include "../common.h"
#include "../types/type_system.h"
#include "iterators.h"
#include <initializer_list>
#include <algorithm>

namespace cj {
namespace stdlib {

/**
 * @brief Dynamic array container (vector)
 */
template<typename T>
class Vector {
private:
    T* data_;
    Size size_;
    Size capacity_;
    
    void Grow() {
        Size new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
        Reserve(new_capacity);
    }
    
    void Destroy() {
        if (data_) {
            for (Size i = 0; i < size_; ++i) {
                data_[i].~T();
            }
            std::free(data_);
        }
    }

public:
    using value_type = T;
    using iterator = T*;
    using const_iterator = const T*;
    
    // Constructors
    Vector() : data_(nullptr), size_(0), capacity_(0) {}
    
    explicit Vector(Size size) : data_(nullptr), size_(0), capacity_(0) {
        Resize(size);
    }
    
    Vector(Size size, const T& value) : data_(nullptr), size_(0), capacity_(0) {
        Resize(size, value);
    }
    
    Vector(std::initializer_list<T> init) : data_(nullptr), size_(0), capacity_(0) {
        Reserve(init.size());
        for (const auto& item : init) {
            PushBack(item);
        }
    }
    
    // Copy constructor
    Vector(const Vector& other) : data_(nullptr), size_(0), capacity_(0) {
        Reserve(other.size_);
        for (Size i = 0; i < other.size_; ++i) {
            PushBack(other.data_[i]);
        }
    }
    
    // Move constructor
    Vector(Vector&& other) noexcept 
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    
    // Destructor
    ~Vector() { Destroy(); }
    
    // Assignment operators
    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Clear();
            Reserve(other.size_);
            for (Size i = 0; i < other.size_; ++i) {
                PushBack(other.data_[i]);
            }
        }
        return *this;
    }
    
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            Destroy();
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }
    
    // Element access
    T& operator[](Size index) { return data_[index]; }
    const T& operator[](Size index) const { return data_[index]; }
    
    T& At(Size index) {
        if (index >= size_) {
            throw std::out_of_range("Vector index out of range");
        }
        return data_[index];
    }
    
    const T& At(Size index) const {
        if (index >= size_) {
            throw std::out_of_range("Vector index out of range");
        }
        return data_[index];
    }
    
    T& Front() { return data_[0]; }
    const T& Front() const { return data_[0]; }
    
    T& Back() { return data_[size_ - 1]; }
    const T& Back() const { return data_[size_ - 1]; }
    
    T* Data() { return data_; }
    const T* Data() const { return data_; }
    
    // Capacity
    Size Size() const { return size_; }
    Size Capacity() const { return capacity_; }
    bool Empty() const { return size_ == 0; }
    
    void Reserve(Size capacity) {
        if (capacity > capacity_) {
            T* new_data = static_cast<T*>(std::malloc(capacity * sizeof(T)));
            if (!new_data) {
                throw std::bad_alloc();
            }
            
            for (Size i = 0; i < size_; ++i) {
                new (new_data + i) T(std::move(data_[i]));
                data_[i].~T();
            }
            
            std::free(data_);
            data_ = new_data;
            capacity_ = capacity;
        }
    }
    
    void ShrinkToFit() {
        if (capacity_ > size_) {
            T* new_data = static_cast<T*>(std::malloc(size_ * sizeof(T)));
            for (Size i = 0; i < size_; ++i) {
                new (new_data + i) T(std::move(data_[i]));
                data_[i].~T();
            }
            std::free(data_);
            data_ = new_data;
            capacity_ = size_;
        }
    }
    
    // Modifiers
    void Clear() {
        for (Size i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = 0;
    }
    
    void PushBack(const T& value) {
        if (size_ >= capacity_) {
            Grow();
        }
        new (data_ + size_) T(value);
        ++size_;
    }
    
    void PushBack(T&& value) {
        if (size_ >= capacity_) {
            Grow();
        }
        new (data_ + size_) T(std::move(value));
        ++size_;
    }
    
    template<typename... Args>
    void EmplaceBack(Args&&... args) {
        if (size_ >= capacity_) {
            Grow();
        }
        new (data_ + size_) T(std::forward<Args>(args)...);
        ++size_;
    }
    
    void PopBack() {
        if (size_ > 0) {
            --size_;
            data_[size_].~T();
        }
    }
    
    void Resize(Size new_size) {
        if (new_size > capacity_) {
            Reserve(new_size);
        }
        
        while (size_ < new_size) {
            new (data_ + size_) T();
            ++size_;
        }
        
        while (size_ > new_size) {
            --size_;
            data_[size_].~T();
        }
    }
    
    void Resize(Size new_size, const T& value) {
        if (new_size > capacity_) {
            Reserve(new_size);
        }
        
        while (size_ < new_size) {
            new (data_ + size_) T(value);
            ++size_;
        }
        
        while (size_ > new_size) {
            --size_;
            data_[size_].~T();
        }
    }
    
    // Iterators
    iterator begin() { return data_; }
    const_iterator begin() const { return data_; }
    const_iterator cbegin() const { return data_; }
    
    iterator end() { return data_ + size_; }
    const_iterator end() const { return data_ + size_; }
    const_iterator cend() const { return data_ + size_; }
};

/**
 * @brief Doubly-linked list container
 */
template<typename T>
class List {
private:
    struct Node {
        T data;
        Node* next;
        Node* prev;
        
        template<typename... Args>
        Node(Args&&... args) : data(std::forward<Args>(args)...), next(nullptr), prev(nullptr) {}
    };
    
    Node* head_;
    Node* tail_;
    Size size_;
    
    void DestroyAll() {
        Node* current = head_;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

public:
    using value_type = T;
    
    class iterator {
    private:
        Node* node_;
        
    public:
        iterator(Node* node) : node_(node) {}
        
        T& operator*() { return node_->data; }
        const T& operator*() const { return node_->data; }
        
        T* operator->() { return &node_->data; }
        const T* operator->() const { return &node_->data; }
        
        iterator& operator++() {
            node_ = node_->next;
            return *this;
        }
        
        iterator operator++(int) {
            iterator temp = *this;
            node_ = node_->next;
            return temp;
        }
        
        iterator& operator--() {
            node_ = node_->prev;
            return *this;
        }
        
        iterator operator--(int) {
            iterator temp = *this;
            node_ = node_->prev;
            return temp;
        }
        
        bool operator==(const iterator& other) const { return node_ == other.node_; }
        bool operator!=(const iterator& other) const { return node_ != other.node_; }
    };
    
    using const_iterator = const iterator;
    
    // Constructors
    List() : head_(nullptr), tail_(nullptr), size_(0) {}
    
    List(std::initializer_list<T> init) : head_(nullptr), tail_(nullptr), size_(0) {
        for (const auto& item : init) {
            PushBack(item);
        }
    }
    
    // Copy constructor
    List(const List& other) : head_(nullptr), tail_(nullptr), size_(0) {
        for (const auto& item : other) {
            PushBack(item);
        }
    }
    
    // Move constructor
    List(List&& other) noexcept 
        : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = other.tail_ = nullptr;
        other.size_ = 0;
    }
    
    // Destructor
    ~List() { DestroyAll(); }
    
    // Assignment operators
    List& operator=(const List& other) {
        if (this != &other) {
            Clear();
            for (const auto& item : other) {
                PushBack(item);
            }
        }
        return *this;
    }
    
    List& operator=(List&& other) noexcept {
        if (this != &other) {
            DestroyAll();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            other.head_ = other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    // Element access
    T& Front() { return head_->data; }
    const T& Front() const { return head_->data; }
    
    T& Back() { return tail_->data; }
    const T& Back() const { return tail_->data; }
    
    // Capacity
    Size Size() const { return size_; }
    bool Empty() const { return size_ == 0; }
    
    // Modifiers
    void Clear() { DestroyAll(); }
    
    void PushFront(const T& value) {
        Node* new_node = new Node(value);
        if (Empty()) {
            head_ = tail_ = new_node;
        } else {
            new_node->next = head_;
            head_->prev = new_node;
            head_ = new_node;
        }
        ++size_;
    }
    
    void PushFront(T&& value) {
        Node* new_node = new Node(std::move(value));
        if (Empty()) {
            head_ = tail_ = new_node;
        } else {
            new_node->next = head_;
            head_->prev = new_node;
            head_ = new_node;
        }
        ++size_;
    }
    
    void PushBack(const T& value) {
        Node* new_node = new Node(value);
        if (Empty()) {
            head_ = tail_ = new_node;
        } else {
            tail_->next = new_node;
            new_node->prev = tail_;
            tail_ = new_node;
        }
        ++size_;
    }
    
    void PushBack(T&& value) {
        Node* new_node = new Node(std::move(value));
        if (Empty()) {
            head_ = tail_ = new_node;
        } else {
            tail_->next = new_node;
            new_node->prev = tail_;
            tail_ = new_node;
        }
        ++size_;
    }
    
    void PopFront() {
        if (!Empty()) {
            Node* old_head = head_;
            head_ = head_->next;
            if (head_) {
                head_->prev = nullptr;
            } else {
                tail_ = nullptr;
            }
            delete old_head;
            --size_;
        }
    }
    
    void PopBack() {
        if (!Empty()) {
            Node* old_tail = tail_;
            tail_ = tail_->prev;
            if (tail_) {
                tail_->next = nullptr;
            } else {
                head_ = nullptr;
            }
            delete old_tail;
            --size_;
        }
    }
    
    // Iterators
    iterator begin() { return iterator(head_); }
    const_iterator begin() const { return const_iterator(head_); }
    const_iterator cbegin() const { return const_iterator(head_); }
    
    iterator end() { return iterator(nullptr); }
    const_iterator end() const { return const_iterator(nullptr); }
    const_iterator cend() const { return const_iterator(nullptr); }
};

/**
 * @brief Double-ended queue (deque)
 */
template<typename T>
class Deque {
private:
    static constexpr Size BLOCK_SIZE = 64;
    
    struct Block {
        alignas(T) char data[BLOCK_SIZE * sizeof(T)];
        
        T* GetData() { return reinterpret_cast<T*>(data); }
        const T* GetData() const { return reinterpret_cast<const T*>(data); }
    };
    
    Vector<Block*> blocks_;
    Size front_block_;
    Size front_index_;
    Size back_block_;
    Size back_index_;
    Size size_;
    
    void AddBlockFront() {
        blocks_.insert(blocks_.begin(), new Block());
        ++front_block_;
        ++back_block_;
    }
    
    void AddBlockBack() {
        blocks_.PushBack(new Block());
    }

public:
    using value_type = T;
    using iterator = T*; // Simplified for now
    using const_iterator = const T*;
    
    // Constructor
    Deque() : front_block_(0), front_index_(BLOCK_SIZE / 2), 
              back_block_(0), back_index_(BLOCK_SIZE / 2), size_(0) {
        blocks_.PushBack(new Block());
    }
    
    // Destructor
    ~Deque() {
        Clear();
        for (Block* block : blocks_) {
            delete block;
        }
    }
    
    // Element access
    T& operator[](Size index) {
        // Simplified implementation
        Size total_front = front_block_ * BLOCK_SIZE + front_index_;
        Size actual_index = total_front + index;
        Size block_index = actual_index / BLOCK_SIZE;
        Size element_index = actual_index % BLOCK_SIZE;
        return blocks_[block_index]->GetData()[element_index];
    }
    
    const T& operator[](Size index) const {
        Size total_front = front_block_ * BLOCK_SIZE + front_index_;
        Size actual_index = total_front + index;
        Size block_index = actual_index / BLOCK_SIZE;
        Size element_index = actual_index % BLOCK_SIZE;
        return blocks_[block_index]->GetData()[element_index];
    }
    
    T& Front() { return blocks_[front_block_]->GetData()[front_index_]; }
    const T& Front() const { return blocks_[front_block_]->GetData()[front_index_]; }
    
    T& Back() { 
        Size index = back_index_ > 0 ? back_index_ - 1 : BLOCK_SIZE - 1;
        Size block = back_index_ > 0 ? back_block_ : back_block_ - 1;
        return blocks_[block]->GetData()[index]; 
    }
    const T& Back() const { 
        Size index = back_index_ > 0 ? back_index_ - 1 : BLOCK_SIZE - 1;
        Size block = back_index_ > 0 ? back_block_ : back_block_ - 1;
        return blocks_[block]->GetData()[index]; 
    }
    
    // Capacity
    Size Size() const { return size_; }
    bool Empty() const { return size_ == 0; }
    
    // Modifiers
    void Clear() {
        // Destroy all elements
        while (!Empty()) {
            PopBack();
        }
    }
    
    void PushFront(const T& value) {
        if (front_index_ == 0) {
            AddBlockFront();
            front_index_ = BLOCK_SIZE;
        }
        --front_index_;
        new (blocks_[front_block_]->GetData() + front_index_) T(value);
        ++size_;
    }
    
    void PushBack(const T& value) {
        if (back_index_ >= BLOCK_SIZE) {
            AddBlockBack();
            ++back_block_;
            back_index_ = 0;
        }
        new (blocks_[back_block_]->GetData() + back_index_) T(value);
        ++back_index_;
        ++size_;
    }
    
    void PopFront() {
        if (!Empty()) {
            blocks_[front_block_]->GetData()[front_index_].~T();
            ++front_index_;
            --size_;
            if (front_index_ >= BLOCK_SIZE && front_block_ < back_block_) {
                ++front_block_;
                front_index_ = 0;
            }
        }
    }
    
    void PopBack() {
        if (!Empty()) {
            if (back_index_ == 0) {
                --back_block_;
                back_index_ = BLOCK_SIZE;
            }
            --back_index_;
            blocks_[back_block_]->GetData()[back_index_].~T();
            --size_;
        }
    }
};

/**
 * @brief Queue container (FIFO)
 */
template<typename T>
class Queue {
private:
    List<T> container_;

public:
    using value_type = T;
    
    // Element access
    T& Front() { return container_.Front(); }
    const T& Front() const { return container_.Front(); }
    
    T& Back() { return container_.Back(); }
    const T& Back() const { return container_.Back(); }
    
    // Capacity
    Size Size() const { return container_.Size(); }
    bool Empty() const { return container_.Empty(); }
    
    // Modifiers
    void Push(const T& value) { container_.PushBack(value); }
    void Push(T&& value) { container_.PushBack(std::move(value)); }
    
    template<typename... Args>
    void Emplace(Args&&... args) { container_.EmplaceBack(std::forward<Args>(args)...); }
    
    void Pop() { container_.PopFront(); }
    
    void Clear() { container_.Clear(); }
};

/**
 * @brief Stack container (LIFO)
 */
template<typename T>
class Stack {
private:
    Vector<T> container_;

public:
    using value_type = T;
    
    // Element access
    T& Top() { return container_.Back(); }
    const T& Top() const { return container_.Back(); }
    
    // Capacity
    Size Size() const { return container_.Size(); }
    bool Empty() const { return container_.Empty(); }
    
    // Modifiers
    void Push(const T& value) { container_.PushBack(value); }
    void Push(T&& value) { container_.PushBack(std::move(value)); }
    
    template<typename... Args>
    void Emplace(Args&&... args) { container_.EmplaceBack(std::forward<Args>(args)...); }
    
    void Pop() { container_.PopBack(); }
    
    void Clear() { container_.Clear(); }
};

/**
 * @brief Heap container (priority queue)
 */
template<typename T, typename Compare = std::less<T>>
class Heap {
private:
    Vector<T> container_;
    Compare comp_;
    
    void HeapifyUp(Size index) {
        while (index > 0) {
            Size parent = (index - 1) / 2;
            if (!comp_(container_[parent], container_[index])) {
                break;
            }
            std::swap(container_[parent], container_[index]);
            index = parent;
        }
    }
    
    void HeapifyDown(Size index) {
        Size size = container_.Size();
        while (true) {
            Size largest = index;
            Size left = 2 * index + 1;
            Size right = 2 * index + 2;
            
            if (left < size && comp_(container_[largest], container_[left])) {
                largest = left;
            }
            if (right < size && comp_(container_[largest], container_[right])) {
                largest = right;
            }
            
            if (largest != index) {
                std::swap(container_[index], container_[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }

public:
    using value_type = T;
    
    // Constructors
    Heap() = default;
    explicit Heap(const Compare& comp) : comp_(comp) {}
    
    // Element access
    const T& Top() const { return container_.Front(); }
    
    // Capacity
    Size Size() const { return container_.Size(); }
    bool Empty() const { return container_.Empty(); }
    
    // Modifiers
    void Push(const T& value) {
        container_.PushBack(value);
        HeapifyUp(container_.Size() - 1);
    }
    
    void Push(T&& value) {
        container_.PushBack(std::move(value));
        HeapifyUp(container_.Size() - 1);
    }
    
    template<typename... Args>
    void Emplace(Args&&... args) {
        container_.EmplaceBack(std::forward<Args>(args)...);
        HeapifyUp(container_.Size() - 1);
    }
    
    void Pop() {
        if (!Empty()) {
            std::swap(container_[0], container_[container_.Size() - 1]);
            container_.PopBack();
            if (!Empty()) {
                HeapifyDown(0);
            }
        }
    }
    
    void Clear() { container_.Clear(); }
};

/**
 * @brief UTF-8 string class
 */
class String {
private:
    Vector<UInt8> data_;
    Size length_; // Number of UTF-8 characters (not bytes)
    
    Size UTF8CharLength(UInt8 first_byte) const {
        if ((first_byte & 0x80) == 0) return 1;      // 0xxxxxxx
        if ((first_byte & 0xE0) == 0xC0) return 2;  // 110xxxxx
        if ((first_byte & 0xF0) == 0xE0) return 3;  // 1110xxxx
        if ((first_byte & 0xF8) == 0xF0) return 4;  // 11110xxx
        return 0; // Invalid UTF-8
    }
    
    void UpdateLength() {
        length_ = 0;
        for (Size i = 0; i < data_.Size(); ) {
            Size char_len = UTF8CharLength(data_[i]);
            if (char_len == 0) break; // Invalid UTF-8
            i += char_len;
            ++length_;
        }
    }

public:
    using value_type = UInt8;
    using iterator = UInt8*;
    using const_iterator = const UInt8*;
    
    // Constructors
    String() : length_(0) {
        data_.PushBack(0); // Null terminator
    }
    
    String(const char* str) : length_(0) {
        if (str) {
            while (*str) {
                data_.PushBack(static_cast<UInt8>(*str++));
            }
        }
        data_.PushBack(0); // Null terminator
        UpdateLength();
    }
    
    String(const String& other) : data_(other.data_), length_(other.length_) {}
    
    String(String&& other) noexcept : data_(std::move(other.data_)), length_(other.length_) {
        other.length_ = 0;
    }
    
    // Assignment
    String& operator=(const String& other) {
        if (this != &other) {
            data_ = other.data_;
            length_ = other.length_;
        }
        return *this;
    }
    
    String& operator=(String&& other) noexcept {
        if (this != &other) {
            data_ = std::move(other.data_);
            length_ = other.length_;
            other.length_ = 0;
        }
        return *this;
    }
    
    String& operator=(const char* str) {
        data_.Clear();
        length_ = 0;
        if (str) {
            while (*str) {
                data_.PushBack(static_cast<UInt8>(*str++));
            }
        }
        data_.PushBack(0); // Null terminator
        UpdateLength();
        return *this;
    }
    
    // Element access
    UInt8& operator[](Size index) { return data_[index]; }
    const UInt8& operator[](Size index) const { return data_[index]; }
    
    // Capacity
    Size Size() const { return data_.Size() - 1; } // Exclude null terminator
    Size Length() const { return length_; } // UTF-8 character count
    bool Empty() const { return data_.Size() <= 1; }
    
    // String operations
    const char* CStr() const { return reinterpret_cast<const char*>(data_.Data()); }
    
    String operator+(const String& other) const {
        String result(*this);
        result += other;
        return result;
    }
    
    String& operator+=(const String& other) {
        if (!other.Empty()) {
            data_.PopBack(); // Remove null terminator
            for (Size i = 0; i < other.Size(); ++i) {
                data_.PushBack(other.data_[i]);
            }
            data_.PushBack(0); // Add null terminator
            UpdateLength();
        }
        return *this;
    }
    
    bool operator==(const String& other) const {
        return data_ == other.data_;
    }
    
    bool operator!=(const String& other) const {
        return !(*this == other);
    }
    
    bool operator<(const String& other) const {
        return std::lexicographical_compare(data_.begin(), data_.end() - 1,
                                          other.data_.begin(), other.data_.end() - 1);
    }
    
    // Iterators
    iterator begin() { return data_.Data(); }
    const_iterator begin() const { return data_.Data(); }
    const_iterator cbegin() const { return data_.Data(); }
    
    iterator end() { return data_.Data() + data_.Size() - 1; } // Exclude null terminator
    const_iterator end() const { return data_.Data() + data_.Size() - 1; }
    const_iterator cend() const { return data_.Data() + data_.Size() - 1; }
    
    // String-specific operations
    String Substring(Size start, Size length = SIZE_MAX) const {
        // This is a simplified implementation
        String result;
        Size end_pos = std::min(start + length, Size());
        for (Size i = start; i < end_pos; ++i) {
            result.data_.PushBack(data_[i]);
        }
        result.data_.PushBack(0);
        result.UpdateLength();
        return result;
    }
    
    Size Find(const String& substr) const {
        // Simplified implementation
        const char* pos = std::strstr(CStr(), substr.CStr());
        return pos ? static_cast<Size>(pos - CStr()) : SIZE_MAX;
    }
};

} // namespace stdlib
} // namespace cj

#endif // CJ_STDLIB_CONTAINERS_H