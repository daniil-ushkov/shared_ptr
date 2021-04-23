#pragma once
#include <cstdint>
#include <type_traits>

template<typename T>
struct default_deleter {
    void operator()(T* object) {
        delete object;
    }
};


struct base_control_block_ {
    virtual void destroy() {};

    virtual ~base_control_block_() = default;


    size_t shared_ptr_counter = 1;

    size_t weak_ptr_counter = 0;
};

template<typename T, typename D = default_deleter<T>>
struct control_block : base_control_block_ {
    control_block(T* ptr)
    : base_control_block_(),
      ptr(ptr),
      deleter(default_deleter<T>())
      {}

    control_block(T* ptr, D deleter)
    : base_control_block_(),
      ptr(ptr),
      deleter(deleter)
      {}

    void destroy() override {
        deleter(ptr);
        ptr = nullptr;
    }

    ~control_block() override {
        if (ptr != nullptr) {
            deleter(ptr);
        }
    }

    T* ptr;
    D deleter;
};

template<typename T>
struct inplace_control_block : base_control_block_ {
    template<typename ...Args>
    explicit inplace_control_block(Args&&... args) {
        new(&object) T(std::forward<Args>(args)...);
    }

    void destroy() override {
        reinterpret_cast<T*>(&object)->~T();
    }

    std::aligned_storage_t<sizeof(T), alignof(T)> object;
};