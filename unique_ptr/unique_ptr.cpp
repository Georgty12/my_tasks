#include <iostream>
template <typename T>
class UniquePtr {
    public:
        UniquePtr() : ptr_(nullptr){}
        UniquePtr(T* ptr) : ptr_(ptr) {}
        UniquePtr(const UniquePtr<T>& b) = delete;
        UniquePtr<T>& operator= (const UniquePtr<T>& b) = delete;

        UniquePtr(UniquePtr<T> &&b) noexcept: ptr_(b.Release()){} 
        template <typename T2>
        UniquePtr(UniquePtr<T2> &&b) noexcept {
            if (std::convertible_to<T2, T>) {
                ptr_ = b.ptr_;
                b.ptr_ = nullptr;
            }
            else 
                UniquePtr();
        }
        UniquePtr<T>& operator= (UniquePtr<T> &&b) noexcept{
            Reset(b.ptr_);
            b.ptr_ = nullptr;
            return *this;
        }
        template<typename T2>
        UniquePtr<T>& operator= (UniquePtr<T2> &&b) {
            if (std::convertible_to<T2, T>){
                Reset(b.ptr_);
                b.ptr_ = nullptr;
                return *this;
            }
            return *this;
        }
        ~UniquePtr () noexcept {
            delete ptr_;
        }


        T* Release() {
            T* tmp = ptr_;
            ptr_ = nullptr;
            return tmp;
        } 

        void Reset(T* ptr = nullptr) {
            delete ptr_;
            ptr_ = ptr;
        }

        void Swap(UniquePtr& b) {
            std::swap(ptr_, b.ptr_);
        }
        T* Get() const{
            return ptr_;
        }

        T& operator* () const{
            return *ptr_;
        }
        T* operator-> () const{
            return ptr_;
        }
        operator bool () const {
            if (ptr_ == nullptr)
                return false;
            return true;    
        }
    private:
        T* ptr_;        
};

template <typename T, typename ... Args>
UniquePtr<T> MakeUnique(Args&& ... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}