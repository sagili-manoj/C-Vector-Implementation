#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <memory>
#include <limits>
#include <utility>
#include <initializer_list>
#include <iostream>

template <typename T>
class SimpleAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::true_type;

    template <typename U>
    struct rebind {
        using other = SimpleAllocator<U>;
    };

    SimpleAllocator() = default;
    template <typename U> SimpleAllocator(const SimpleAllocator<U>&) noexcept {}

    pointer allocate(size_type n) {
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }
    void deallocate(pointer ptr, size_type) {
        ::operator delete(ptr);
    }
    template <typename... Args>
    void construct(pointer ptr, Args&&... args) {
        ::new(ptr) T(std::forward<Args>(args)...);
    }
    void destroy(pointer ptr) {
        ptr->~T();
    }
    size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }
};

template <typename T, typename Allocator = SimpleAllocator<T>>
class Vector {
private:
    using AllocTraits = std::allocator_traits<Allocator>;
    T* data_;
    size_t size_;
    size_t capacity_;
    Allocator alloc_;

    void check_index(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Vector index out of range");
        }
    }

    void reserve_more(size_t new_capacity) {
        T* new_data = AllocTraits::allocate(alloc_, new_capacity);
        size_t constructed = 0;
        try {
            for (size_t i = 0; i < size_; ++i) {
                AllocTraits::construct(alloc_, new_data + i, std::move_if_noexcept(data_[i]));
                ++constructed;
            }
            for (size_t i = 0; i < size_; ++i) {
                AllocTraits::destroy(alloc_, data_ + i);
            }
            AllocTraits::deallocate(alloc_, data_, capacity_);
            data_ = new_data;
            capacity_ = new_capacity;
        } catch (...) {
            for (size_t i = 0; i < constructed; ++i) {
                AllocTraits::destroy(alloc_, new_data + i);
            }
            AllocTraits::deallocate(alloc_, new_data, new_capacity);
            throw;
        }
    }

public:
    class ConstIterator;
    class Iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(pointer ptr) : ptr_(ptr) {}
        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        Iterator& operator++() { ++ptr_; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++ptr_; return tmp; }
        Iterator& operator--() { --ptr_; return *this; }
        Iterator operator--(int) { Iterator tmp = *this; --ptr_; return tmp; }
        Iterator operator+(difference_type n) const { return Iterator(ptr_ + n); }
        Iterator operator-(difference_type n) const { return Iterator(ptr_ - n); }
        difference_type operator-(const Iterator& other) const { return ptr_ - other.ptr_; }
        bool operator==(const Iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const Iterator& other) const { return ptr_ != other.ptr_; }
        bool operator==(const ConstIterator& other) const;
        bool operator!=(const ConstIterator& other) const;

    private:
        friend class Vector;
        pointer ptr_;
    };

    class ConstIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        ConstIterator(const T* ptr) : ptr_(ptr) {}
        ConstIterator(const Iterator& other) : ptr_(other.ptr_) {}
        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        ConstIterator& operator++() { ++ptr_; return *this; }
        ConstIterator operator++(int) { ConstIterator tmp = *this; ++ptr_; return tmp; }
        ConstIterator& operator--() { --ptr_; return *this; }
        ConstIterator operator--(int) { ConstIterator tmp = *this; --ptr_; return tmp; }
        ConstIterator operator+(difference_type n) const { return ConstIterator(ptr_ + n); }
        ConstIterator operator-(difference_type n) const { return ConstIterator(ptr_ - n); }
        difference_type operator-(const ConstIterator& other) const { return ptr_ - other.ptr_; }
        bool operator==(const ConstIterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const ConstIterator& other) const { return ptr_ != other.ptr_; }
        bool operator==(const Iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const Iterator& other) const { return ptr_ != other.ptr_; }

    private:
        friend class Vector;
        const T* ptr_;
    };

    Vector() : data_(nullptr), size_(0), capacity_(0) {}
    
    explicit Vector(size_t n) : data_(nullptr), size_(n), capacity_(n) {
        if (n > 0) {
            data_ = AllocTraits::allocate(alloc_, n);
            try {
                for (size_t i = 0; i < n; ++i) {
                    AllocTraits::construct(alloc_, data_ + i);
                }
            } catch (...) {
                AllocTraits::deallocate(alloc_, data_, n);
                throw;
            }
        }
    }

    Vector(size_t n, const T& value) : data_(nullptr), size_(n), capacity_(n) {
        if (n > 0) {
            data_ = AllocTraits::allocate(alloc_, n);
            try {
                for (size_t i = 0; i < n; ++i) {
                    AllocTraits::construct(alloc_, data_ + i, value);
                }
            } catch (...) {
                AllocTraits::deallocate(alloc_, data_, n);
                throw;
            }
        }
    }

    template <typename InputIt>
    Vector(InputIt first, InputIt last) : data_(nullptr), size_(0), capacity_(0) {
        for (auto it = first; it != last; ++it) {
            push_back(*it);
        }
    }

    Vector(std::initializer_list<T> il) : Vector(il.begin(), il.end()) {}

    Vector(const Vector& other) : data_(nullptr), size_(other.size_), capacity_(other.size_),
        alloc_(AllocTraits::select_on_container_copy_construction(other.alloc_)) {
        if (size_ > 0) {
            data_ = AllocTraits::allocate(alloc_, size_);
            try {
                for (size_t i = 0; i < size_; ++i) {
                    AllocTraits::construct(alloc_, data_ + i, other.data_[i]);
                }
            } catch (...) {
                AllocTraits::deallocate(alloc_, data_, size_);
                throw;
            }
        }
    }

    Vector(Vector&& other) noexcept 
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_), alloc_(std::move(other.alloc_)) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~Vector() {
        for (size_t i = 0; i < size_; ++i) {
            AllocTraits::destroy(alloc_, data_ + i);
        }
        AllocTraits::deallocate(alloc_, data_, capacity_);
    }

    Vector& operator=(const Vector& other) {
        Vector tmp(other);
        std::swap(data_, tmp.data_);
        std::swap(size_, tmp.size_);
        std::swap(capacity_, tmp.capacity_);
        if constexpr (AllocTraits::propagate_on_container_copy_assignment::value) {
            alloc_ = other.alloc_;
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        Vector tmp(std::move(other));
        std::swap(data_, tmp.data_);
        std::swap(size_, tmp.size_);
        std::swap(capacity_, tmp.capacity_);
        if constexpr (AllocTraits::propagate_on_container_move_assignment::value) {
            alloc_ = std::move(other.alloc_);
        }
        return *this;
    }

    Vector& operator=(std::initializer_list<T> il) {
        Vector tmp(il);
        std::swap(data_, tmp.data_);
        std::swap(size_, tmp.size_);
        std::swap(capacity_, tmp.capacity_);
        return *this;
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (size_ == capacity_) {
            size_t new_capacity = (capacity_ == 0) ? 1 : static_cast<size_t>(capacity_ * 1.5);
            reserve_more(new_capacity);
        }
        AllocTraits::construct(alloc_, data_ + size_, std::forward<Args>(args)...);
        ++size_;
    }

    void push_back(const T& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(std::move(value)); }

    void pop_back() {
        if (size_ > 0) {
            --size_;
            AllocTraits::destroy(alloc_, data_ + size_);
        }
    }

    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }

    T& at(size_t index) {
        check_index(index);
        return data_[index];
    }
    const T& at(size_t index) const {
        check_index(index);
        return data_[index];
    }

    T& front() { return data_[0]; }
    const T& front() const { return data_[0]; }
    T& back() { return data_[size_ - 1]; }
    const T& back() const { return data_[size_ - 1]; }

    T* data() noexcept { return data_; }
    const T* data() const noexcept { return data_; }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }
    size_t max_size() const { return AllocTraits::max_size(alloc_); }

    void reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            reserve_more(new_capacity);
        }
    }

    void shrink_to_fit() {
        if (capacity_ > size_) {
            if (size_ == 0) {
                AllocTraits::deallocate(alloc_, data_, capacity_);
                data_ = nullptr;
                capacity_ = 0;
            } else {
                reserve_more(size_);
            }
        }
    }

    void resize(size_t count) {
        if (count < size_) {
            for (size_t i = count; i < size_; ++i) {
                AllocTraits::destroy(alloc_, data_ + i);
            }
            size_ = count;
        } else if (count > size_) {
            if (count > capacity_) {
                reserve_more(count);
            }
            for (size_t i = size_; i < count; ++i) {
                AllocTraits::construct(alloc_, data_ + i);
            }
            size_ = count;
        }
    }

    void resize(size_t count, const T& value) {
        if (count < size_) {
            for (size_t i = count; i < size_; ++i) {
                AllocTraits::destroy(alloc_, data_ + i);
            }
            size_ = count;
        } else if (count > size_) {
            if (count > capacity_) {
                reserve_more(count);
            }
            for (size_t i = size_; i < count; ++i) {
                AllocTraits::construct(alloc_, data_ + i, value);
            }
            size_ = count;
        }
    }

    void clear() {
        for (size_t i = 0; i < size_; ++i) {
            AllocTraits::destroy(alloc_, data_ + i);
        }
        size_ = 0;
    }

    Iterator begin() { return Iterator(data_); }
    Iterator end() { return Iterator(data_ + size_); }
    ConstIterator begin() const { return ConstIterator(data_); }
    ConstIterator end() const { return ConstIterator(data_ + size_); }
    ConstIterator cbegin() const { return ConstIterator(data_); }
    ConstIterator cend() const { return ConstIterator(data_); }

    friend bool operator==(const Vector& lhs, const Vector& rhs) {
        return lhs.size_ == rhs.size_ && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }
    friend bool operator!=(const Vector& lhs, const Vector& rhs) { return !(lhs == rhs); }
};
template <typename T, typename Allocator>
bool Vector<T, Allocator>::Iterator::operator==(const typename Vector<T, Allocator>::ConstIterator& other) const {
    return ptr_ == other.ptr_;
}

template <typename T, typename Allocator>
bool Vector<T, Allocator>::Iterator::operator!=(const typename Vector<T, Allocator>::ConstIterator& other) const {
    return ptr_ != other.ptr_;
}

template <typename T, typename Allocator>
void swap(Vector<T, Allocator>& lhs, Vector<T, Allocator>& rhs) noexcept {
    std::swap(lhs.data_, rhs.data_);
    std::swap(lhs.size_, rhs.size_);
    std::swap(lhs.capacity_, rhs.capacity_);
    if constexpr (std::allocator_traits<Allocator>::propagate_on_container_swap::value) {
        std::swap(lhs.alloc_, rhs.alloc_);
    }
}
