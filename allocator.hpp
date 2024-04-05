#pragma once

#include <cstdlib>

template <typename ValueType>
class Allocator {
public:
    using Pointer           = ValueType*;
    using Reference         = ValueType&;
    using ConstPointer      = const ValueType*;
    using ConstReference    = const ValueType&;
    using SizeType          = size_t;
    using DiffType          = int;

    template<typename T>
    using RebindAlloc       = Allocator<T>;

    Allocator() = default;

    Pointer allocate(SizeType n) {
        if (n == 0) {
            return nullptr;
        } else {
            return static_cast<Pointer>(malloc(n*sizeof(ValueType)));
        }
    }

    void deallocate(Pointer ptr, SizeType n) {
        delete[] ptr;
    }

};
