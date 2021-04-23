#pragma once

template<typename T>
struct shared_ptr;

template<typename T>
struct weak_ptr {
    // constructor
    weak_ptr<T>();

    weak_ptr<T>(const weak_ptr<T> &other);

    weak_ptr<T>(weak_ptr<T> &&other);

    weak_ptr<T>(const shared_ptr<T> &other);


    // assignment operator
    weak_ptr<T> &operator=(const weak_ptr<T> &other) noexcept;

    weak_ptr<T> &operator=(weak_ptr<T> &&other) noexcept;

    weak_ptr<T> &operator=(const shared_ptr<T> &other) noexcept;


    // destructor
    ~weak_ptr();


    // modifiers
    void reset();

    void swap(weak_ptr<T> &other);


    // observers
    long use_count() const noexcept;

    bool expired() const noexcept;

    shared_ptr<T> lock() const noexcept;

private:
    T *ptr;

    base_control_block_ *ptr_manager;


    friend class shared_ptr<T>;
};

// constructors
template<typename T>
weak_ptr<T>::weak_ptr()
        : ptr(nullptr),
          ptr_manager(nullptr) {}

template<typename T>
weak_ptr<T>::weak_ptr(const weak_ptr<T> &other)
        : ptr(other.ptr),
          ptr_manager(other.ptr_manager) {
    if (ptr_manager != nullptr) {
        ptr_manager->weak_ptr_counter++;
    }
}

template<typename T>
weak_ptr<T>::weak_ptr(weak_ptr<T> &&other)
        : weak_ptr() {
    swap(other);
}

template<typename T>
weak_ptr<T>::weak_ptr(const shared_ptr<T> &other)
        : ptr(other.ptr),
          ptr_manager(other.ptr_manager) {
    if (ptr_manager != nullptr) {
        ptr_manager->weak_ptr_counter++;
    }
}

// assignment operators
template<typename T>
weak_ptr<T> &weak_ptr<T>::operator=(const weak_ptr<T> &other) noexcept {
    if (this == &other) {
        return *this;
    }
    auto w_ptr = other;
    swap(w_ptr);
    return *this;
}

template<typename T>
weak_ptr<T> &weak_ptr<T>::operator=(weak_ptr<T> &&other) noexcept {
    if (this == &other) {
        return *this;
    }
    swap(other);
    other.reset();
    return *this;
}

template<typename T>
weak_ptr<T> &weak_ptr<T>::operator=(const shared_ptr<T> &other) noexcept {
    weak_ptr<T> w_ptr = other;
    swap(w_ptr);
    return *this;
}

// destructor
template<typename T>
weak_ptr<T>::~weak_ptr() {
    if (ptr_manager == nullptr) {
        return;
    }
    ptr_manager->weak_ptr_counter--;
}

// modifiers
template<typename T>
void weak_ptr<T>::reset() {
    weak_ptr<T>().swap(*this);
}

template<typename T>
void weak_ptr<T>::swap(weak_ptr<T> &other) {
    std::swap(ptr, other.ptr);
    std::swap(ptr_manager, other.ptr_manager);
}

// observers
template<typename T>
long weak_ptr<T>::use_count() const noexcept {
    if (ptr_manager == nullptr) {
        return 0;
    } else {
        return ptr_manager->shared_ptr_counter;
    }
}

template<typename T>
bool weak_ptr<T>::expired() const noexcept {
    return use_count() == 0;
}

template<typename T>
shared_ptr<T> weak_ptr<T>::lock() const noexcept {
    return expired() ? shared_ptr<T>() : shared_ptr<T>(*this);
}
