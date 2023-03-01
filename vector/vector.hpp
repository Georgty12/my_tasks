#pragma once
#include <stdexcept>
#include <exception>
#include <type_traits>
#include <memory>
#include <new>
template <typename T>
class Vector {
public:
    // simple usings
    using ValueType = T;
    using Pointer = T*;
    using ConstPointer = const T*;
    using Reference = T&;
    using ConstReference = const T&;
    using SizeType = size_t;
  
    // Rule of 5 and simple constructors
    Vector() {}
    ~Vector() {
        if (data_ != nullptr) {
            for (size_t i = 0; i < size_; ++i) {
                (data_ + i)->~T();
            }
        }
        delete[] reinterpret_cast<int8_t*>(data_);
        data_ = nullptr;
    }
    Vector(const Vector& other) :
    data_(nullptr)
    {
        if (other.data_ != nullptr) {
            data_ = reinterpret_cast<T*>(new int8_t[other.capacity_ * sizeof(T)]);
            size_t i = 0;
            try {
                for (; i < other.size_; ++i) {
                    new (data_ + i) T(other.data_[i]);
                }
            }catch (...) {
                for (size_t j = 0; j < i; ++j) {
                    (data_ + j)->~T();
                    
                }
    
                delete[] reinterpret_cast<int8_t*>(data_);
                data_ = nullptr;
                throw;
            }
            size_ = other.size_;
            capacity_ = other.capacity_;
        }
        else
            Vector();
    }
    
    Vector(Vector&& other)
    {        
        Swap(other);
    }

    Vector& operator=(const Vector& other) 
    {
        if (this == &other)
            return *this;
        Vector tmp = other;
        Swap(tmp);
        return *this;
    }
    Vector& operator=(Vector&& other)
    {
        if (this == &other)
            return *this;
        for (size_t i = 0; i < size_; ++i) 
            (data_ + i)->~T();
        delete[] reinterpret_cast<int8_t*>(data_);
        data_ = nullptr;

        data_ = other.data_;
        other.data_ = nullptr;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.size_ = 0;
        other.capacity_ = 0;
        return *this;
    }

    Vector(size_t n) : size_(n), capacity_(n){
        if (n != 0) {
            size_t i = 0;
            data_ = reinterpret_cast<T*>(new int8_t[n * sizeof(T)]);
            try {
                for (; i < n; ++i) {
                   new (data_ + i) T();
                }
            } catch (...)
            {
                for (size_t j = 0; j < i; ++j) {
                    (data_ + j)->~T();
                }
                delete[] reinterpret_cast<int8_t*>(data_);
                throw;
            }
            
        }
    }
    Vector(size_t n, const T& elem) : data_(nullptr), size_(n), capacity_(n){
        if (n != 0) {
            size_t i = 0;
            data_ = reinterpret_cast<T*>(new int8_t[n * sizeof(T)]);
            try {
                
                for (; i < n; ++i) {
                   new (data_ + i) T(std::move(elem));
                }
            } catch (...)
            {
                for (size_t j = 0; j < i; ++j) {
                    (data_ + j)->~T();
                }
                delete[] reinterpret_cast<int8_t*>(data_);
                data_ = nullptr;
                throw;
            }
            
        }
    }
    Vector(const std::initializer_list<T> &list) :data_(nullptr), size_(list.size()), capacity_(list.size()){
        if (size_ != 0) {
            data_ = reinterpret_cast<T*>(new int8_t[size_ * sizeof(T)]);
            size_t i = 0;
            try {
                for (auto it = list.begin(); it != list.end(); ++it) {
                    new (data_ + i) T(*it);
                    ++i;
                }
            }
            catch(...) {
                for (size_t j = 0; j < i; ++j)
                    (data_ + j)->~T();
                delete[] reinterpret_cast<int8_t*>(data_);
                data_ = nullptr;
                throw;    
            }
        }
    }

    // Simple Methods
    void Swap (Vector& other) {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

   
    size_t Size() const{
        return size_;
    }
    size_t Capacity() const{
        return capacity_;
    }
    T* Data() {
        return data_;
    }
    const T* Data() const{
        return data_;
    }
    T& Front() {
        return data_[0];
    }
    const T& Front() const{
        return data_[0];
    }
    T& Back() {
        return data_[size_ - 1];
    }
    const T& Back() const {
        return data_[size_ - 1];
    }

    T& At(size_t index) {
        if (index >= size_)
            throw std::out_of_range("xxx");
        return data_[index];    
    }
    const T& At(size_t index) const {
        if (index >= size_)
            throw std::out_of_range("xxx");
        return data_[index];  
    }
     T& operator[] (size_t index) {
        return data_[index];
    }
    const T& operator[] (size_t index) const{
        return data_[index];
    }
    bool Empty() const {
        return (size_ == 0);
    }
    void Clear() {
        for (size_t i = 0; i < size_; ++i)
            (data_ + i)->~T();
        size_ = 0;
        
    }
    void Resize(size_t N) {
        if (N < size_) {
                for (size_t i = N; i < size_; ++i)
                    (data_ + i)->~T();   
                size_ = N;
                return;      
        }
        T* newarr = reinterpret_cast<T*> (new int8_t[N * sizeof(T)]);
        for (size_t i = 0; i < N - size_; ++i) {
            try {
                new (newarr + i) T();
            } catch(...) {
                for (size_t j = 0; j < i; ++j) 
                    (newarr + j)->~T();
                delete[] reinterpret_cast<int8_t*>(newarr);    
                throw;    
            }
        }
        if (N > capacity_)
            Reserve(N);
        for (size_t i = size_; i < N; ++i) {
            try{
                new (data_ + i) T();
            } catch(...) {
                for (size_t j = size_; j < i; ++j)
                    (data_ + j)->~T();
                for (size_t i = 0; i < N - size_; ++i) {
                    (newarr + i)->~T();
                }
                delete[] reinterpret_cast<int8_t*>(newarr);    
                throw;    
            }
        }
        for (size_t i = 0; i < N - size_; ++i) {
            (newarr + i)->~T();
        }
        delete[] reinterpret_cast<int8_t*>(newarr);
        size_ = N;    
    }
    void Resize(size_t N, const T& elem) {
        if (N < size_) {
                for (size_t i = N; i < size_; ++i)
                    (data_ + i)->~T();   
                size_ = N;
                return;      
        }
        T* newarr = reinterpret_cast<T*> ( new int8_t[N * sizeof(T)]);
        for (size_t i = 0; i < N - size_; ++i) {
            try {
                new (newarr + i) T(std::move_if_noexcept(elem));
            }catch(...) {
                for (size_t j = 0; j < size_; ++j) {
                    (newarr + j)->~T();
                }
                
                throw;
            }
        }
        if (N > capacity_){           
            Reserve(N);
        }
        for (size_t i = size_; i < N; ++i) {
            try{
                new (data_ + i) T(std::move_if_noexcept(newarr[i - size_]));
            } catch(...) {
                for (size_t j = size_; j < i; ++j)
                    (data_ + j)->~T();
                for (size_t i = 0; i < N - size_; ++i) {
                    (newarr + i)->~T();
                }
                delete[] reinterpret_cast<int8_t*>(newarr);
                throw;    
            }
        }
        for (size_t i = 0; i < N - size_; ++i) {
            (newarr + i)->~T();
        }
        delete[] reinterpret_cast<int8_t*>(newarr);
        size_ = N;    
        // if (N > capacity_) {
            
        //     // Reserve(N);
        //     if (data_ != nullptr){
        //         size_t i = size_;
        //         try{ 
        //             for (; i < N; ++i) {
        //                 new(data_ + i) T(std::move_if_noexcept(elem));
        //             }
        //         } catch(...) {
        //             for (size_t j = size_; j < i; ++j) {
        //                 (data_ + j)->~T();
        //             }
        //             throw;
        //         }
        //     }
        // }
        // else if (N < capacity_){
        //     if (N > size_) {
        //         size_t i = size_;
        //         try {
        //             for (; i < N; ++i) {
        //                 new (data_ + i) T(std::move_if_noexcept(elem));
        //             }
        //         } catch(...) {
        //             for (size_t j = size_; j < i; ++j)
        //                 (data_ + j)->~T();
        //             throw;    
        //         }
        //     }   
        //     else if (N < size_) {
        //         for (size_t i = N; i < size_; ++i)
        //             (data_ + i)->~T();   
        //         size_ = N;
        //         return;      
        //     }
        // }
        
    }
    void Reserve(const size_t size) {
        if (size <= capacity_)
            return;
        T* newarr = reinterpret_cast<T*> (new int8_t[size * sizeof(T)]);;
        size_t i = 0;
        for (; i < size_; ++i)  {
            try {    
                new(newarr + i) T(std::move_if_noexcept(data_[i]));
            }
            catch(...) {
                for (size_t j = 0; j < i; ++j) {
                    // new (data_ + j) T(std::move_if_noexcept(newarr[i]));
                    (newarr  + j)->~T();
                }
                delete[] reinterpret_cast<int8_t*>(newarr);    
                newarr = nullptr;
                throw;
            }
        }
        for (size_t i = 0; i < size_; ++i)
            (data_ + i)->~T();
        delete[] reinterpret_cast<int8_t*>(data_);
        data_ = newarr;
        capacity_ = size;
    }
    
    void ShrinkToFit() {
        Vector tmp;
        tmp.Reserve(size_);
        size_t i = 0;
        try{
            for (; i < size_; ++i) {
                tmp.PushBack(std::move_if_noexcept(data_[i]));
            }
        } catch(...) {
            for (size_t j = 0; j < i; ++j) {
                PopBack();
            }
            throw;
        }
        Swap(tmp);
    }

    void PushBack(const T& elem) {
        EmplaceBack(std::forward<const T>(elem));
    }
    void PushBack(T&& elem) {
        EmplaceBack(std::forward<T>(elem));
    }
    void PopBack() noexcept {
        if (size_ > 0){
            (data_ + size_ - 1)->~T(); 
            size_--;
        }
    }
    template <typename... Args>
    void EmplaceBack(Args... args) {
        if (size_ == capacity_) {
            Reserve(std::max((size_t)1, capacity_ * 2));
        }        
        new (data_ + size_) T(std::forward<Args>(args)...);
        size_++;
    }

    // iterators realization
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;
    
    template<typename InputIt,
        typename = typename std::iterator_traits<InputIt>::iterator_category>
    Vector(InputIt begin,InputIt end) : size_( end - begin), capacity_(end - begin)
    {
        if (begin != end) {
            size_t i = 0;
            T* newarr = reinterpret_cast<T*> (new int8_t[capacity_ * sizeof(T)]);
            while (begin != end) {
                try {
                    new (newarr + i) T(*begin);
                } catch(...) {
                    for (size_t j = 0; j < i; ++j) 
                        (newarr + j)->~T();
                    delete[] reinterpret_cast<int8_t*>(newarr);
                    size_ = 0;
                    capacity_ = 0;
                    throw;            
                }
                ++begin;
                i++;
            }
            data_ = newarr;
        }
    }

    
    Iterator begin() {
        return data_;
    }
    
    ConstIterator begin() const{
        return data_;
    }

    Iterator end() {
        return data_ + size_;
    }
    ConstIterator end() const {
        return data_ + size_;
    }
    ConstIterator cbegin() const{
        return data_;
    }
    ConstIterator cend() const{
        return data_ + size_;
    }
    ReverseIterator rbegin() {
        return ReverseIterator(end());
    }
    ConstReverseIterator rbegin() const{
        return ConstReverseIterator(end());
    }
    ReverseIterator rend() {
        return ReverseIterator(begin());
    }
    ConstReverseIterator rend() const{
        return ConstReverseIterator(begin());
    }
    ConstReverseIterator crbegin() const {
        return ConstReverseIterator(end());
    }
    ConstReverseIterator crend() const {
        return ConstReverseIterator(begin());
    }
private:
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;    
};