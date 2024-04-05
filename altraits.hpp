#pragma once

#include <concepts>

template <typename T>
concept TAllocator = requires (T alloc) {
    {alloc.allocate(std::declval<typename T::SizeType>())} -> std::same_as<typename T::Pointer>;
    alloc.deallocate(std::declval<typename T::Pointer>(), std::declval<typename T::SizeType>());
};

template <typename ValueType, TAllocator Alloc>
class AllocatorTraits {
public:

    using Pointer       = typename Alloc::Pointer;
    using SizeType      = typename Alloc::SizeType;

    template <typename... Args>
    static void construct(const Alloc& alloc, Pointer ptr, Args&&... args) {
        *ptr = std::move(ValueType(std::forward<Args>(args)...));
    }
    static void destroy(const Alloc& alloc, ValueType* ptr) {
        ptr->~ValueType();
    }
    static typename Alloc::Pointer allocate(Alloc& alloc, typename Alloc::SizeType n) {
        return alloc.allocate(n);
    }
    static void deallocate(Alloc& alloc, Pointer ptr, typename Alloc::SizeType n) {
        alloc.deallocate(ptr, n);
    }
};
