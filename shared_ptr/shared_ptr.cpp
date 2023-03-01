#include <concepts>
#include <iostream>
#include <type_traits>
template <typename T> class Control_Block {
public:
  size_t shared_count = 0;
  size_t weak_count = 0;
  Control_Block(size_t a, size_t b) : shared_count(a), weak_count(b) {}
};
template <typename T> class WeakPtr;

template <typename T> class SharedPtr {
public:
  SharedPtr() : ptr_(nullptr), cptr(nullptr) {}
  template <typename U> friend class WeakPtr;
  SharedPtr(T *ptr) : ptr_(ptr) {
    if (ptr_ != nullptr) {
      cptr = new Control_Block<T>(1, 0);
    } else
      cptr = nullptr;
  }

  SharedPtr(const SharedPtr<T> &shared_ptr) noexcept
      : ptr_(shared_ptr.ptr_), cptr(shared_ptr.cptr) {
    ++cptr->shared_count;
  }
  template <typename T2> SharedPtr(const SharedPtr<T2> &shared_ptr) {
    if (std::convertible_to<T2, T>) {
      ptr_ = shared_ptr.ptr_;
      cptr = shared_ptr.cptr;
      ++cptr->shared_count;
    } else
      SharedPtr();
  }

  SharedPtr<T> &operator=(const SharedPtr<T> &shared_ptr) noexcept {
    if (ptr_ == shared_ptr.ptr_)
      return *this;
    if (!cptr) {
    } else if (cptr->shared_count > 1)
      --cptr->shared_count;
    else if (cptr->shared_count == 1) {
      --cptr->shared_count;
      if (!cptr->weak_count) {
        delete cptr;
        cptr = nullptr;
      }
      delete ptr_;
      ptr_ = nullptr;
    }
    ptr_ = shared_ptr.ptr_;
    cptr = shared_ptr.cptr;
    if (cptr != nullptr)
      ++cptr->shared_count;
    return *this;
  }
  template <typename T2>
  SharedPtr<T> &operator=(const SharedPtr<T2> &shared_ptr) {
    if (std::convertible_to<T2, T>) {
      if (ptr_ == shared_ptr.ptr_)
        return *this;
      if (!cptr) {
      } else if (cptr->shared_count > 1)
        --cptr->shared_count;

      else if (cptr->shared_count == 1) {
        --cptr->shared_count;
        if (!cptr->weak_count) {
          delete cptr;
        }
        delete ptr_;
      }
      ptr_ = shared_ptr.ptr_;
      cptr = shared_ptr.cptr;
      if (cptr != nullptr)
        ++cptr->shared_count;
    }
    return *this;
  }
  SharedPtr(SharedPtr<T> &&shared_ptr) noexcept
      : ptr_(shared_ptr.ptr_), cptr(shared_ptr.cptr) {
    shared_ptr.ptr_ = nullptr;
    shared_ptr.cptr = nullptr;
  }
  template <typename T2> SharedPtr(SharedPtr<T2> &&shared_ptr) {
    if (std::convertible_to<T2, T>) {
      ptr_ = shared_ptr.ptr_;
      cptr = shared_ptr.cptr;
      shared_ptr.ptr_ = nullptr;
      shared_ptr.cptr = nullptr;
    }
  }

  SharedPtr(const WeakPtr<T> &wp) {
    if (wp.Expired())
      throw std::bad_weak_ptr();

    else {
      if (wp.cptr != nullptr) {
        wp.cptr->shared_count++;
        cptr = wp.cptr;
        ptr_ = wp.object;
      }
    }
  }

  SharedPtr<T> &operator=(SharedPtr<T> &&shared_ptr) noexcept {
    if (!cptr) {
    } else if (cptr->shared_count > 1)
      --cptr->shared_count;
    else if (cptr->shared_count == 1) {
      --cptr->shared_count;
      delete ptr_;
      if (!cptr->weak_count)
        delete cptr;
    }
    cptr = shared_ptr.cptr;
    ptr_ = shared_ptr.ptr_;
    shared_ptr.ptr_ = nullptr;
    shared_ptr.cptr = nullptr;
    return *this;
  }
  template <typename T2> SharedPtr<T> &operator=(SharedPtr<T2> &&shared_ptr) {
    if (std::convertible_to<T2, T>) {
      if (!cptr) {
      } else if (cptr->shared_count > 1)
        --cptr->shared_count;
      else if (cptr->shared_count == 1) {
        --cptr->shared_count;
        delete ptr_;
        if (!cptr->weak_count)
          delete cptr;
      }
      cptr = shared_ptr.cptr;
      ptr_ = shared_ptr.ptr_;
      shared_ptr.ptr_ = nullptr;
      shared_ptr.cptr = nullptr;
    }
    return *this;
  }

  ~SharedPtr() {
    if (!cptr)
      return;
    --cptr->shared_count;
    if (!cptr->shared_count) {
      delete ptr_;
      ptr_ = nullptr;
      if (!cptr->weak_count) {
        delete cptr;
        cptr = nullptr;
      }
    }
  }

  void Reset(T *ptr = nullptr) {
    if (!cptr) {
    } else if (cptr->shared_count > 1) {
      --cptr->shared_count;
    } else if (cptr->shared_count == 1) {
      delete ptr_;
      ptr_ = nullptr;
      --cptr->shared_count;
      if (cptr->weak_count == 0) {
        delete cptr;
        cptr = nullptr;
      }
    }
    ptr_ = ptr;
    if (ptr_ != nullptr)
      cptr = new Control_Block<T>(1, 0);
    else
      cptr = nullptr;
  }
  void Swap(SharedPtr<T> &b) {
    std::swap(ptr_, b.ptr_);
    std::swap(cptr, b.cptr);
  }
  T *Get() const { return ptr_; }
  size_t UseCount() const {
    if (!cptr)
      return 0;
    return cptr->shared_count;
  }
  T &operator*() const { return *ptr_; }
  T *operator->() const { return ptr_; }
  operator bool() const {
    if (ptr_ == nullptr)
      return false;
    return true;
  }

private:
  T *ptr_ = nullptr;
  Control_Block<T> *cptr = nullptr;

  template <typename U, typename... Args>
  friend SharedPtr<U> MakeShared(Args &&...args);

  // explicit SharedPtr (Control_Block<T>* cptr1) : ptr_(cptr1.object),
  // cptr(&cptr1) {
  //     if (cptr != nullptr)
  //         ++cptr->shared_count;
  //  }
};

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args &&...args) {
  // auto p = new Control_Block<T>(1, std::forward<Args>(args)...);
  return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T> class WeakPtr {
public:
  WeakPtr() {}
  template <typename U> friend class SharedPtr;

  ~WeakPtr() {
    if (cptr == nullptr)
      return;
    --cptr->weak_count;
    if (cptr->weak_count == 0)
      if (cptr->shared_count == 0) {
        delete cptr;
      }
  }

  WeakPtr(const WeakPtr<T> &wp) : cptr(wp.cptr), object(wp.object) {
    if (cptr != nullptr)
      ++cptr->weak_count;
  }
  template <typename U> WeakPtr(const WeakPtr<U> &wp) {
    if (std::convertible_to<U, T>) {
      cptr = wp.cptr;
      object = wp.object;
      if (cptr != nullptr)
        ++cptr->weak_count;
    } else
      WeakPtr<T>();
  }

  WeakPtr(WeakPtr<T> &&wp) noexcept : cptr(wp.cptr), object(wp.object) {
    wp.cptr = nullptr;
    wp.object = nullptr;
  }
  template <typename U> WeakPtr(WeakPtr<U> &&wp) {
    if (std::convertible_to<U, T>) {
      cptr = wp.cptr;
      object = wp.object;
      wp.cptr = nullptr;
      wp.object = nullptr;
    }
  }

  WeakPtr(const SharedPtr<T> &sp) noexcept {
    cptr = sp.cptr;
    object = sp.ptr_;
    if (cptr != nullptr)
      ++cptr->weak_count;
  }

  WeakPtr &operator=(const WeakPtr<T> &wp) noexcept {
    if (cptr == wp.cptr)
      return *this;
    if (!cptr) {
    } else
      --cptr->weak_count;
    cptr = wp.cptr;
    object = wp.object;
    if (cptr != nullptr)
      ++cptr->weak_count;
    return *this;
  }
  template <typename U> WeakPtr &operator=(const WeakPtr<T> &wp) {
    if (std::convertible_to<U, T>) {
      if (cptr == wp.cptr)
        return *this;
      if (!cptr) {
      } else
        --cptr->weak_count;
      cptr = wp.cptr;
      object = wp.object;
      if (cptr != nullptr)
        ++cptr->weak_count;
    }
    return *this;
  }

  WeakPtr &operator=(WeakPtr<T> &&wp) noexcept {

    if (cptr == wp.cptr) {
      wp.cptr = nullptr;
      return *this;
    }
    if (!cptr) {
    } else
      --cptr->weak_count;
    cptr = wp.cptr;
    object = wp.object;
    wp.cptr = nullptr;
    wp.object = nullptr;
    return *this;
  }
  template <typename U> WeakPtr &operator=(WeakPtr<T> &&wp) {
    if (std::convertible_to<U, T>) {

      if (cptr == wp.cptr) {
        wp.cptr = nullptr;
        return *this;
      }
      if (!cptr) {
      } else
        --cptr->weak_count;
      cptr = wp.cptr;
      object = wp.object;
      wp.cptr = nullptr;
      wp.object = nullptr;
    }
    return *this;
  }
  void Swap(WeakPtr<T> &wp) {
    std::swap(cptr, wp.cptr);
    std::swap(object, wp.object);
  }
  void Reset() {
    if (!cptr)
      return;
    --cptr->weak_count;
    if (cptr->weak_count == 0)
      if (cptr->shared_count == 0) {
        delete cptr;
        cptr = nullptr;
      }
    cptr = nullptr;
    object = nullptr;
  }
  size_t UseCount() const {
    if (!cptr)
      return 0;
    return cptr->shared_count;
  }
  bool Expired() const {
    if (cptr == nullptr)
      return true;
    return (cptr->shared_count == 0);
  }
  SharedPtr<T> Lock() const {
    if (Expired())
      return SharedPtr<T>();

    return SharedPtr<T>(*this);
  }

private:
  Control_Block<T> *cptr = nullptr;
  T *object = nullptr;
};

int main() {
  auto raw_ptr = new int;
  SharedPtr<int> ptr(raw_ptr);
  const WeakPtr<int> b = ptr;
  auto ptr_tmp = b.Lock();

  ptr.Reset();
  printf("ld\n", ptr_tmp.Get() == raw_ptr);
  printf("%ld\n", ptr_tmp.UseCount());
}