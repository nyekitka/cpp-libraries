#pragma once

#include "allocator.hpp"
#include "altraits.hpp"
#include "iterators.hpp"
#include "exceptions.hpp"
#include <initializer_list>

template <typename Array, typename ValueType>
concept IsArrayLike = requires(Array v) {
    requires std::same_as<ValueType, typename Array::ValueType>;
    {v.begin()} -> IsForwardIterator<ValueType>;
    {v.cbegin()} -> IsForwardIterator<ValueType>;
    {v.end()} -> IsForwardIterator<ValueType>;
    {v.cend()} -> IsForwardIterator<ValueType>;
    {v.size()} -> std::same_as<typename Array::SizeType>;
};

template <std::default_initializable ValueType,
            TAllocator AllocatorType>
class Array;

template <std::default_initializable VType>
class Slice;


template <typename ValueType>
class arrayIterator : public RandomAccessIterator<ValueType> {
public:
    using Base              = RandomAccessIterator<ValueType>;
    using Pointer           = typename Base::Pointer;
    using ConstPointer      = typename Base::ConstPointer;
    using Reference         = typename Base::Reference;
    using ConstReference    = typename Base::ConstReference;
    using ItDiff            = typename Base::ItDiff;
    using SizeType          = typename Base::SizeType;

    template <std::default_initializable VT, TAllocator AllocatorType>
    friend class Array;

    template <std::default_initializable VT>
    friend class Slice;

private:
    arrayIterator(const Pointer& begin, const SizeType& size, SizeType pos) : begin(begin), size(size), pos(pos) {}

public:

    arrayIterator(const Base& it) : arrayIterator(static_cast<const arrayIterator&>(it)) {}
    arrayIterator(const BidirectionalIterator<ValueType>& it) : arrayIterator(static_cast<const arrayIterator&>(it)) {}
    arrayIterator(const ForwardIterator<ValueType>& it) : arrayIterator(static_cast<const arrayIterator&>(it)) {}

    bool operator==(const ForwardIterator<ValueType>& it) const noexcept override {
        arrayIterator _it = static_cast<const arrayIterator<ValueType>&>(it);
        return begin == _it.begin && size == _it.size && pos == _it.pos;
    }

    bool operator!=(const ForwardIterator<ValueType>& it) const noexcept override {
        arrayIterator _it = static_cast<const arrayIterator<ValueType>&>(it);
        return begin != _it.begin || size != _it.size || pos != _it.pos;
    }

    bool operator>(const Base& it) const override {
        arrayIterator _it = static_cast<const arrayIterator<ValueType>&>(it);
        if (begin != _it.begin || size != _it.size) {
                throw NotComparableIterators();
        } else {
            return pos > _it.pos;
        }
    }

    bool operator<(const Base& it) const override {
        arrayIterator _it = static_cast<const arrayIterator<ValueType>&>(it);
        if (begin != _it.begin || size != _it.size) {
                throw NotComparableIterators();
        } else {
            return pos < _it.pos;
        }
    }

    bool operator>=(const Base& it) const override {
        arrayIterator _it = static_cast<const arrayIterator<ValueType>&>(it);
        if (begin != _it.begin || size != _it.size) {
                throw NotComparableIterators();
        } else {
            return pos >= _it.pos;
        }
    }

    bool operator<=(const Base& it) const override {
        arrayIterator _it = static_cast<const arrayIterator<ValueType>&>(it);
        if (begin != _it.begin || size != _it.size) {
                throw NotComparableIterators();
        } else {
            return pos <= _it.pos;
        }
    }

    Reference operator*() const override {
        if (pos >= size) {
            throw UndereferencableIterator();
        } else {
            return *(begin + pos);
        }
    }

    Pointer operator->() const override {
        if (pos >= size) {
            throw UndereferencableIterator();
        } else {
            return begin + pos;
        }
    }

    ForwardIterator<ValueType>& operator++() override {
        if (pos >= size) {
            throw IteratorOutOfBounds();
        } else {
            ++pos;
            return static_cast<ForwardIterator<ValueType>&>(*this);
        }
    }

    arrayIterator operator++(int) {
        if (pos >= size) {
            throw IteratorOutOfBounds();
        } else {
            return arrayIterator(begin, size, pos++);
        }
    }

    BidirectionalIterator<ValueType>& operator--() override {
        if (pos == 0) {
            throw IteratorOutOfBounds();
        } else {
            --pos;
            return static_cast<BidirectionalIterator<ValueType>&>(*this);
        }
    }

    arrayIterator operator--(int) {
        if (pos == 0) {
            throw IteratorOutOfBounds();
        } else {
            return arrayIterator(begin, size, pos--);
        }
    }

    Base& operator+=(ItDiff offset) override {
        if (pos + offset >= 0 && pos + offset <= size) {
            pos += offset;
            return static_cast<Base&>(*this);
        } else {
            throw IteratorOutOfBounds();
        }
    }

    Base& operator-=(ItDiff offset) override {
        if (pos - offset >= 0 && pos - offset <= size) {
            pos -= offset;
            return static_cast<Base&>(*this);
        } else {
            throw IteratorOutOfBounds();
        }
    }

    arrayIterator operator+ (ItDiff offset) const {
        if (pos + offset >= 0 && pos + offset <= size) {
            return arrayIterator(begin, size, pos + offset);
        } else {
            throw IteratorOutOfBounds();
        }
    }

    arrayIterator operator- (ItDiff offset) const {
        if (pos - offset >= 0 && pos - offset <= size) {
            return arrayIterator(begin, size, pos - offset);
        } else {
            throw IteratorOutOfBounds();
        }
    }

    ItDiff operator- (const Base& it) const override {
        arrayIterator _it = static_cast<const arrayIterator&>(it);
        if (_it.begin == begin && _it.size == size) {
            return ItDiff(pos) - _it.pos;
        } else {
            throw NotComparableIterators();
        }
    }

private:
    const Pointer& begin;
    const SizeType& size;
    SizeType pos;
};

template <typename ValueType>
class constArrayIterator : public ConstRandomAccessIterator<ValueType> {
public:
    using Base              = ConstRandomAccessIterator<ValueType>;
    using Pointer           = typename Base::Pointer;
    using ConstPointer      = typename Base::ConstPointer;
    using Reference         = typename Base::Reference;
    using ConstReference    = typename Base::ConstReference;
    using ItDiff            = typename Base::ItDiff;
    using SizeType          = typename Base::SizeType;

    template <std::default_initializable VT, TAllocator AllocatorType>
    friend class Array;

    template <std::default_initializable VT>
    friend class Slice;

private:
    constArrayIterator(const Pointer& begin, const SizeType& size, SizeType pos) : begin(begin), size(size), pos(pos) {}

public:

    bool operator==(const ConstForwardIterator<ValueType>& it) const noexcept override {
        constArrayIterator _it = static_cast<const constArrayIterator<ValueType>&>(it);
        return begin == _it.begin && size == _it.size && pos == _it.pos;
    }

    bool operator!=(const ConstForwardIterator<ValueType>& it) const noexcept override {
        constArrayIterator _it = static_cast<const constArrayIterator<ValueType>&>(it);
        return begin != _it.begin || size != _it.size || pos != _it.pos;
    }

    bool operator>(const Base& it) const override {
        constArrayIterator _it = static_cast<const constArrayIterator<ValueType>&>(it);
        if (begin != _it.begin || size != _it.size) {
                throw NotComparableIterators();
        } else {
            return pos > _it.pos;
        }
    }

    bool operator<(const Base& it) const override {
        constArrayIterator _it = static_cast<const constArrayIterator<ValueType>&>(it);
        if (begin != _it.begin || size != _it.size) {
                throw NotComparableIterators();
        } else {
            return pos < _it.pos;
        }
    }

    bool operator>=(const Base& it) const override {
        constArrayIterator _it = static_cast<const constArrayIterator<ValueType>&>(it);
        if (begin != _it.begin || size != _it.size) {
                throw NotComparableIterators();
        } else {
            return pos >= _it.pos;
        }
    }

    bool operator<=(const Base& it) const override {
        constArrayIterator _it = static_cast<const constArrayIterator<ValueType>&>(it);
        if (begin != _it.begin || size != _it.size) {
                throw NotComparableIterators();
        } else {
            return pos <= _it.pos;
        }
    }

    ConstReference operator*() const override {
        if (pos >= size) {
            throw UndereferencableIterator();
        } else {
            return *(begin + pos);
        }
    }

    ConstPointer operator->() const override {
        if (pos >= size) {
            throw UndereferencableIterator();
        } else {
            return begin + pos;
        }
    }

    ConstForwardIterator<ValueType>& operator++() override {
        if (pos >= size) {
            throw IteratorOutOfBounds();
        } else {
            ++pos;
            return static_cast<ConstForwardIterator<ValueType>&>(*this);
        }
    }

    constArrayIterator operator++(int) {
        if (pos >= size) {
            throw IteratorOutOfBounds();
        } else {
            return constArrayIterator(begin, size, pos++);
        }
    }

    ConstBidirectionalIterator<ValueType>& operator--() override {
        if (pos == 0) {
            throw IteratorOutOfBounds();
        } else {
            --pos;
            return static_cast<ConstBidirectionalIterator<ValueType>&>(*this);
        }
    }

    constArrayIterator operator--(int) {
        if (pos == 0) {
            throw IteratorOutOfBounds();
        } else {
            return constArrayIterator(begin, size, pos--);
        }
    }

    Base& operator+=(ItDiff offset) override {
        if (pos + offset >= 0 && pos + offset <= size) {
            pos += offset;
            return static_cast<Base&>(*this);
        } else {
            throw IteratorOutOfBounds();
        }
    }

    Base& operator-=(ItDiff offset) override {
        if (pos - offset >= 0 && pos - offset <= size) {
            pos -= offset;
            return static_cast<Base&>(*this);
        } else {
            throw IteratorOutOfBounds();
        }
    }

    constArrayIterator operator+ (ItDiff offset) const {
        if (pos + offset >= 0 && pos + offset <= size) {
            return constArrayIterator(begin, size, pos + offset);
        } else {
            throw IteratorOutOfBounds();
        }
    }

    constArrayIterator operator- (ItDiff offset) const {
        if (pos - offset >= 0 && pos - offset <= size) {
            return constArrayIterator(begin, size, pos - offset);
        } else {
            throw IteratorOutOfBounds();
        }
    }

    ItDiff operator- (const Base& it) const override {
        constArrayIterator _it = static_cast<const constArrayIterator&>(it);
        if (_it.begin == begin && _it.size == size) {
            return ItDiff(pos) - _it.pos;
        } else {
            throw NotComparableIterators();
        }
    }

private:
    const Pointer& begin;
    const SizeType& size;
    SizeType pos;
};

template <std::default_initializable VType>
class Slice {

public:
    using ValueType             = VType;
    using Pointer               = ValueType*;
    using Reference             = ValueType&;
    using ConstPointer          = const ValueType*;
    using ConstReference        = const ValueType&;
    using Iterator              = arrayIterator<ValueType>;
    using ConstIterator         = constArrayIterator<ValueType>;
    using SizeType              = typename Iterator::SizeType;
    using ItDiff                = typename Iterator::ItDiff;

    template<std::default_initializable ValueType, TAllocator AlType>
    friend class Array;

    Iterator begin() noexcept {
        return Iterator(arr, _size, 0);
    }

    ConstIterator cbegin() const noexcept {
        return ConstIterator(arr, _size, 0);
    }

    Iterator end() noexcept {
        return Iterator(arr, _size, _size);
    }

    ConstIterator cend() const noexcept {
        return ConstIterator(arr, _size, _size);
    }

    SizeType size() const noexcept {
        return _size;
    }

    ValueType& operator[](ItDiff ind) noexcept {
        if (ind >= 0) {
            return *(arr + (ind % _size));
        } else {
            return *(arr + (_size + ind % _size) % _size);
        }
    }

    void operator=(const std::initializer_list<ValueType>& list) noexcept {
        SizeType minsz = (list.size() < _size ? list.size() : _size);
        auto it = list.begin();
        for (SizeType i = 0; i < minsz; ++i, ++it) {
            *(arr + i) = *it;
        }
    }
    template <TAllocator AlType>
    void operator=(const Array<ValueType, AlType>& array) {
        SizeType minsz = (array.size() < _size ? array.size() : _size);
        for (SizeType i = 0; i < minsz; ++i) {
            *(arr + i) = array[i];
        }
    }
private:
    Pointer arr;
    SizeType _size;

    Slice(Pointer begin, SizeType size) : arr(begin), _size(size) {}
};

template <std::default_initializable VType,
            TAllocator AlType = Allocator<VType>>
class Array {

public:
    using ValueType             = VType;
    using Pointer               = ValueType*;
    using Reference             = ValueType&;
    using ConstPointer          = const ValueType*;
    using ConstReference        = const ValueType&;
    using AllocatorType         = AlType;
    using Iterator              = arrayIterator<ValueType>;
    using ConstIterator         = constArrayIterator<ValueType>;
    using SizeType              = typename Iterator::SizeType;
    using ItDiff                = typename Iterator::ItDiff;
    using SliceType             = Slice<ValueType>;

    static const SizeType default_capacity = 8;

private:
    using AllocTraits       = AllocatorTraits<ValueType, AllocatorType>;

public:

    Array() : _size(), alloc(), cap(default_capacity) {
        arr = AllocTraits::allocate(alloc, cap);
    }

    Array(SizeType n, const AllocatorType& alloc = AllocatorType()) : _size(n), cap(2*n), alloc(alloc) {
        arr = AllocTraits::allocate(this->alloc, 2*n);
        for (SizeType i = 0; i < n; i++) {
            AllocTraits::construct(this->alloc, arr + i);
        }
    }

    template <IsArrayLike<ValueType> Vector>
    Array(const Vector& v, const AllocatorType& alloc = AllocatorType()) : alloc(alloc){
        _size = v.size();
        cap = 2*_size;
        arr = AllocTraits::allocate(this->alloc, cap);
        SizeType i = 0;
        for (auto it = v.cbegin(); it != v.cend(); ++it) {
            AllocTraits::construct(this->alloc, arr + i, *it);
            ++i;
        }
    }

    template <SizeType len>
    Array(const ValueType (&array)[len], const AllocatorType& alloc = AllocatorType()) : _size(len), cap(2*len), alloc(alloc){
        arr = AllocTraits::allocate(this->alloc, cap);
        for (SizeType i = 0; i < _size; ++i) {
            AllocTraits::construct(this->alloc, arr + i, array[i]);
        }
    }

    Array(const std::initializer_list<ValueType>& list, const AllocatorType& alloc = AllocatorType()) : alloc(alloc) {
        _size = list.size();
        cap = 2*_size;
        arr = AllocTraits::allocate(this->alloc, cap);
        SizeType i = 0;
        for (auto it = list.begin(); it != list.end(); ++it) {
            AllocTraits::construct(this->alloc, arr + i, *it);
            ++i;
        }
    }

    Array(const Array& other) : alloc(alloc) {
        _size = other._size;
        cap = other.cap;
        alloc = other.alloc;
        arr = AllocTraits::allocate(alloc, cap);
        for (SizeType i = 0; i < _size; i++) {
            AllocTraits::construct(alloc, arr + i, other.arr + i);
        }
    }

    Array (Array&& other) : _size(other._size), alloc(other.alloc), cap(other.cap), arr(other.arr) {
        other.arr = AllocTraits::allocate(alloc, default_capacity);
        other._size = 0;
        other.cap = default_capacity;
    }

    ~Array() {
        for (SizeType i = 0; i < _size; ++i) {
            AllocTraits::destroy(alloc, arr + i);
        }
        AllocTraits::deallocate(alloc, arr, cap);
    }

    void operator=(const Array& other) {
        if (cap < other._size) {
            for (SizeType i = 0; i < _size; ++i) {
                AllocTraits::destroy(alloc, arr + i);
            }
            AllocTraits::deallocate(alloc, arr, cap);
            arr = AllocTraits::allocate(alloc, other.cap);
        }
        for (SizeType i = 0; i < other._size; ++i) {
            if (cap < other._size || i >= _size) {
                AllocTraits::construct(alloc, arr + i, other.arr + i);
            } else {
                *(arr + i) = *(other.arr + i);
            }
        }
        if (cap < other._size) {
            cap = other.cap;
        }
        _size = other._size;
    }
    void operator= (Array&& other) {
        for (SizeType i = 0; i < _size; i++) {
            AllocTraits::destroy(alloc, arr + i);
        }
        AllocTraits::deallocate(alloc, arr, cap);
        arr = other.arr;
        _size = other._size;
        cap = other.cap;
    }
    void operator= (const std::initializer_list<ValueType>& list) {
        if (cap < list.size()) {
            for (SizeType i = 0; i < _size; ++i) {
                AllocTraits::destroy(alloc, arr + i);
            }
            AllocTraits::deallocate(alloc, arr, cap);
            arr = AllocTraits::allocate(alloc, 2*list.size());
        }
        auto it = list.begin();
        for (SizeType i = 0; i < list.size(); ++i, ++it) {
            if (cap < list.size() || i >= _size) {
                AllocTraits::construct(alloc, arr + i, *it);
            } else {
                *(arr + i) = *it;
            }
        }
        if (cap < list.size()) {
            cap = 2*list.size();
        }
        _size = list.size();
    }

    template <IsArrayLike<ValueType> Vector>
    void operator= (const Vector& vec) {
        if (cap < vec.size()) {
            for (SizeType i = 0; i < _size; ++i) {
                AllocTraits::destroy(alloc, arr + i);
            }
            AllocTraits::deallocate(alloc, arr, cap);
            arr = AllocTraits::allocate(alloc, 2*vec.size());
        }
        auto it = vec.cbegin();
        for (SizeType i = 0; i < vec.size(); ++i, ++it) {
            if (cap < vec.size() || i >= _size) {
                AllocTraits::construct(alloc, arr + i, *it);
            } else {
                *(arr + i) = *it;
            }
        }
        if (cap < vec.size()) {
            cap = 2*vec.size();
        }
        _size = vec.size();
    }

    template <SizeType len>
    void operator= (const ValueType (&array)[len]) {
        if (cap < len) {
            for (SizeType i = 0; i < _size; ++i) {
                AllocTraits::destroy(alloc, arr + i);
            }
            AllocTraits::deallocate(alloc, arr, cap);
            arr = AllocTraits::allocate(alloc, 2*len);
        }
        for (SizeType i = 0; i < len; ++i) {
            if (cap < len || i >= _size) {
                AllocTraits::construct(alloc, arr + i, array[i]);
            } else {
                *(arr + i) = array[i];
            }
        }
        if (cap < len) {
            cap = 2*len;
        }
        _size = len;
    }

    Iterator begin() noexcept {
        return Iterator(arr, _size, 0);
    }

    ConstIterator cbegin() const noexcept {
        return ConstIterator(arr, _size, 0);
    }

    Iterator end() noexcept {
        return Iterator(arr, _size, _size);
    }

    ConstIterator cend() const noexcept {
        return ConstIterator(begin, _size, _size);
    }

    SizeType size() const noexcept {
        return _size;
    }

    SizeType capacity() const noexcept {
        return cap;
    }

    std::enable_if_t<std::equality_comparable<ValueType>, bool> operator== (const Array& other) const noexcept {
        if (_size != other._size) return false;
        else {
            for (SizeType i = 0; i < _size; ++i) {
                if (*(arr + i) != *(other._arr + i)) {
                    return false;
                }
            }
            return true;
        }
    }

    std::enable_if_t<std::equality_comparable<ValueType>, bool> operator!= (const Array& other) const noexcept {
        if (_size != other._size) return true;
        else {
            for (SizeType i = 0; i < _size; ++i) {
                if (*(arr + i) != *(other._arr + i)) {
                    return true;
                }
            }
            return false;
        }
    }

    template <std::constructible_from<ValueType> ...Args>
    void emplace(const Iterator& where, Args&& ...args) {
        if (where.pos > _size) throw IteratorOutOfBounds();
        if (_size == cap) {
            Pointer newarr = AllocTraits::allocate(alloc, cap*2);
            for (SizeType i = 0; i < where.pos; ++i) {
                AllocTraits::construct(alloc, newarr + i, std::move(*(arr + i)));
            }
            AllocTraits::construct(alloc, newarr + where.pos, std::forward<Args>(args)...);
            for (SizeType i = where.pos + 1; i < _size + 1; ++i) {
                AllocTraits::construct(alloc, newarr + i, std::move(*(arr + i - 1)));
            }
            AllocTraits::deallocate(alloc, arr, cap);
            cap *= 2;
            ++_size;
            arr = newarr;
        } else {
            for (SizeType i = _size; i > where.pos; --i) {
                AllocTraits::construct(alloc, arr + i, std::move(*(arr + i - 1)));
            }
            AllocTraits::construct(alloc, arr + where.pos, std::forward<Args>(args)...);
            ++_size;
        }
    }

    template <IsForwardIterator<ValueType> FIt>
    void insert(const Iterator& where, const FIt& begin, const FIt& end) {
        if (where.begin != arr || where.size != _size) throw AnotherIterator();
        SizeType addsize = 0;
        for (auto it = begin; it != end; ++it) {++addsize;}
        if (_size + addsize > cap) {
            Pointer newarr = AllocTraits::allocate(alloc, cap + addsize);
            SizeType i = 0;
            for (; i < where.pos; ++i) {
                AllocTraits::construct(alloc, newarr + i, std::move(*(arr + i)));
            }
            for (auto it = begin; it != end; ++it, ++i) {
                AllocTraits::construct(alloc, newarr + i, *it);
            }
            for (; i < _size + addsize; ++i) {
                AllocTraits::construct(alloc, newarr + i, std::move(*(arr + i - addsize)));
            }
            AllocTraits::deallocate(alloc, arr, cap);
            arr = newarr;
            cap += addsize;
            _size += addsize;
        } else {
            for (SizeType i = addsize + _size - 1; i >= where.pos + addsize; --i) {
                AllocTraits::construct(alloc, arr + i, std::move(*(arr + i - addsize)));
            }
            SizeType i = where.pos;
            for (auto it = begin; it != end; ++it, ++i) {
                AllocTraits::construct(alloc, arr + i, *it);
            }
            _size += addsize;
        }
    }

    void append(const ValueType& val) {
        emplace(end(), val);
    }

    void append(ValueType&& val) {
        emplace(end(), std::move(val));
    }

    void pop() {
        erase(end()-1,end(),1);
    }

    void erase(const Iterator& begin, const Iterator& end, SizeType step=1) {
        if (begin.pos >= _size || end.pos >= _size) throw IteratorOutOfBounds();
        else if (begin > end) return;
        else {
            SizeType deleted = 0;
            for (SizeType i = begin.pos; i < _size; ++i) {
                if (i % step == begin.pos % step && i < end.pos) {
                    ++deleted;
                    AllocTraits::destroy(alloc, arr + i);
                } else {
                    AllocTraits::construct(alloc, arr + i - deleted, std::move(*(arr + i)));
                }
            }
            _size -= deleted;
        }
    }

    void reserve(SizeType n) {
        if (n <= cap) {
            return;
        } else {
            Pointer newarr = AllocTraits::allocate(alloc, n);
            for (SizeType i = 0; i < _size; ++i) {
                AllocTraits::construct(alloc, newarr + i, std::move(*(arr + i)));
            }
            AllocTraits::deallocate(alloc, arr, cap);
            arr = newarr;
            cap = n;
        }
    }

    void shrink_to_fit() {
        if (cap == _size) return;
        Pointer newarr = AllocTraits::allocate(alloc, _size);
        for (SizeType i = 0; i < _size; ++i) {
            AllocTraits::construct(alloc, newarr + i, std::move(*(arr + i)));
        }
        AllocTraits::deallocate(alloc, arr, cap);
        arr = newarr;
        cap = _size;
    }

    ValueType& operator[](ItDiff ind) noexcept {
        if (ind >= 0) {
            return *(arr + (ind % _size));
        } else {
            return *(arr + (_size + ind % _size) % _size);
        }
    }

    SliceType slice(SizeType from, SizeType to)  {
        if (from > to) return Slice(arr + _size, 0);
        else return Slice(arr + from, to - from);
    }
private:
    Pointer arr;
    SizeType _size, cap;
    AllocatorType alloc;

};
