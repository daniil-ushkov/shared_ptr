#pragma once

#include "control_block.h"
#include "weak_ptr.h"

template<typename T>
struct shared_ptr {
    // constructors
    shared_ptr<T>();

    shared_ptr<T>(nullptr_t);

    shared_ptr<T>(T *val);

    shared_ptr<T>(const shared_ptr<T> &other);

    shared_ptr<T>(const weak_ptr<T> &other);

    template<typename U, std::enable_if_t<std::is_base_of<T, U>::value, int> = 0>
    shared_ptr(const shared_ptr<U> &other);

    shared_ptr(shared_ptr<T> &&other);

    template<typename U>
    shared_ptr(const shared_ptr<U> other, T *member);

    template<typename D>
    shared_ptr(T *val, D deleter);


    // assignment operator
    shared_ptr<T> &operator=(const shared_ptr<T> &other);

    template<typename U, std::enable_if_t<std::is_base_of<T, U>::value, int> = 0>
    shared_ptr<T> &operator=(const shared_ptr<U> &other);

    shared_ptr<T> &operator=(shared_ptr<T> &&other) noexcept;


    // destructor
    ~shared_ptr();


    // modifiers
    void reset();

    void reset(T *val);

    template<typename D>
    void reset(T *val, D deleter);

    void swap(shared_ptr<T> &other) noexcept;


    // observers
    T *get() const noexcept;

    T &operator*() const noexcept;

    T *operator->() const noexcept;

    long use_count() const noexcept;

    explicit operator bool() const noexcept;


private:
    shared_ptr(T *ptr, base_control_block_ *control_block);


    // members
    T *ptr;

    base_control_block_ *ptr_manager;


    // friends
    template<typename U, typename ...Args>
    friend shared_ptr<U> make_shared(Args &&... args);

    friend class weak_ptr<T>;

    template<typename U>
    friend
    class shared_ptr;

    template<typename U>
    friend bool operator==(const shared_ptr<U> &, const shared_ptr<U> &);

    template<typename U>
    friend bool operator==(const shared_ptr<U> &, std::nullptr_t);

    template<typename U>
    friend bool operator==(nullptr_t, const shared_ptr<U> &);

    template<typename U>
    friend bool operator!=(const shared_ptr<U> &, const shared_ptr<U> &);

    template<typename U>
    friend bool operator!=(const shared_ptr<U> &, std::nullptr_t);

    template<typename U>
    friend bool operator!=(nullptr_t, const shared_ptr<U> &);
};


// constructors
template<typename T>
shared_ptr<T>::shared_ptr() : ptr(nullptr), ptr_manager(nullptr) {}

template<typename T>
shared_ptr<T>::shared_ptr(nullptr_t) : ptr(nullptr), ptr_manager(nullptr) {}

template<typename T>
shared_ptr<T>::shared_ptr(T *val)
        : ptr(val),
          ptr_manager(new control_block<T>(val)) {};

template<typename T>
shared_ptr<T>::shared_ptr(const shared_ptr<T> &other)
        : ptr(other.ptr),
          ptr_manager(other.ptr_manager) {
    if (ptr_manager != nullptr) {
        ptr_manager->shared_ptr_counter++;
    }
}

template<typename T>
shared_ptr<T>::shared_ptr(const weak_ptr<T> &other)
        : ptr(other.ptr),
          ptr_manager(other.ptr_manager) {
    if (ptr_manager != nullptr) {
        ptr_manager->shared_ptr_counter++;
    }
}

template<typename T>
template<typename U, std::enable_if_t<std::is_base_of<T, U>::value, int>>
shared_ptr<T>::shared_ptr(const shared_ptr<U> &other)
        : ptr(other.ptr),
          ptr_manager(other.ptr_manager) {
    if (ptr_manager != nullptr) {
        ptr_manager->shared_ptr_counter++;
    }
}

template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr<T> &&other)
        : shared_ptr() {
    swap(other);
}

template<typename T>
template<typename U>
shared_ptr<T>::shared_ptr(const shared_ptr<U> other, T *member)
        : ptr(member),
          ptr_manager(other.ptr_manager) {
    ptr_manager->shared_ptr_counter++;
}

template<typename T>
template<typename D>
shared_ptr<T>::shared_ptr(T *val, D deleter)
        : ptr(val),
          ptr_manager(new control_block<T, D>(val, deleter)) {}

template<typename T>
shared_ptr<T> &shared_ptr<T>::operator=(const shared_ptr<T> &other) {
    if (this == &other) {
        return *this;
    }
    auto sh_ptr = other;
    swap(sh_ptr);
    return *this;
}

template<typename T>
template<typename U, std::enable_if_t<std::is_base_of<T, U>::value, int>>
shared_ptr<T> &shared_ptr<T>::operator=(const shared_ptr<U> &other) {
    if (this == &other) {
        return *this;
    }
    auto sh_ptr = other;
    swap(sh_ptr);
    return *this;
}

// assignment operator
template<typename T>
shared_ptr<T> &shared_ptr<T>::operator=(shared_ptr<T> &&other) noexcept {
    if (this == &other) {
        return *this;
    }
    swap(other);
    shared_ptr<T>().swap(other);
    return *this;
}

//destructor
template<typename T>
shared_ptr<T>::~shared_ptr() {
    if (ptr_manager == nullptr) {
        return;
    }
    ptr_manager->shared_ptr_counter--;
    if (ptr_manager->shared_ptr_counter == 0) {
        ptr = nullptr;
        ptr_manager->destroy();
        if (ptr_manager->weak_ptr_counter == 0) {
            delete ptr_manager;
        }
    }
}

template<typename T>
void shared_ptr<T>::reset() {
    shared_ptr<T>().swap(*this);
}

template<typename T>
void shared_ptr<T>::reset(T *val) {
    shared_ptr<T>(val).swap(*this);
}

template<typename T>
template<typename D>
void shared_ptr<T>::reset(T *val, D deleter) {
    shared_ptr<T>(val, deleter).swap(*this);
}

template<typename T>
void shared_ptr<T>::swap(shared_ptr<T> &other) noexcept {
    std::swap(ptr, other.ptr);
    std::swap(ptr_manager, other.ptr_manager);
}

// observers
template<typename T>
T *shared_ptr<T>::get() const noexcept {
    return ptr;
};

template<typename T>
T &shared_ptr<T>::operator*() const noexcept {
    return *ptr;
};

template<typename T>
T *shared_ptr<T>::operator->() const noexcept {
    return ptr;
};

template<typename T>
long shared_ptr<T>::use_count() const noexcept {
    if (ptr_manager == nullptr) {
        return 0;
    } else {
        return ptr_manager->shared_ptr_counter;
    }
};

template<typename T>
shared_ptr<T>::operator bool() const noexcept {
    return ptr != nullptr;
}

template<typename T, typename ...Args>
inline shared_ptr<T> make_shared(Args &&... args) {
    auto *ptr_manager = new inplace_control_block<T>(std::forward<Args>(args)...);
    auto sh_ptr = shared_ptr(reinterpret_cast<T *>(&ptr_manager->object), static_cast<base_control_block_ *>(ptr_manager));
    return sh_ptr;
}

template<typename T>
shared_ptr<T>::shared_ptr(T *ptr, base_control_block_ *control_block)
        : ptr(ptr),
          ptr_manager(control_block) {}

template<typename T>
bool operator==(const shared_ptr<T> &lhs, const shared_ptr<T> &rhs) {
    return lhs.ptr == rhs.ptr;
}

template<typename T>
bool operator==(const shared_ptr<T> &sh_ptr, std::nullptr_t) {
    return sh_ptr.ptr == nullptr;
}

template<typename T>
bool operator==(std::nullptr_t, const shared_ptr<T> &sh_ptr) {
    return sh_ptr.ptr == nullptr;
}

template<typename T>
bool operator!=(const shared_ptr<T> &sh_ptr, std::nullptr_t) {
    return sh_ptr.ptr != nullptr;
}

template<typename T>
bool operator!=(const shared_ptr<T> &lhs, const shared_ptr<T> &rhs) {
    return lhs.ptr != rhs.ptr;
}

template<typename T>
bool operator!=(std::nullptr_t, const shared_ptr<T> &sh_ptr) {
    return sh_ptr.ptr != nullptr;
}

