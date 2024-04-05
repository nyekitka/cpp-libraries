#pragma once

template <typename VType>
class ForwardIterator {
public:
    using ValueType         = VType;
    using Pointer           = ValueType*;
    using ConstPointer      = const ValueType*;
    using Reference         = ValueType&;
    using ConstReference    = const Reference&;
    using SizeType          = size_t;
    using RightReference    = ValueType&&;

    // dereferencing

    virtual Reference operator*() const = 0;
    virtual Pointer operator->() const = 0;

    // checking for equality/unequality

    virtual bool operator==(const ForwardIterator&) const noexcept = 0;
    virtual bool operator!=(const ForwardIterator&) const noexcept = 0;

    // increment

    virtual ForwardIterator& operator++() = 0;
};

template <typename VType>
class ConstForwardIterator {
public:
    using ValueType         = VType;
    using Pointer           = ValueType*;
    using ConstPointer      = const ValueType*;
    using Reference         = ValueType&;
    using ConstReference    = const Reference&;
    using SizeType          = size_t;
    using RightReference    = ValueType&&;

    // dereferencing

    virtual ConstReference operator*() const = 0;
    virtual ConstPointer operator->() const = 0;

    // checking for equality/unequality

    virtual bool operator==(const ConstForwardIterator&) const noexcept = 0;
    virtual bool operator!=(const ConstForwardIterator&) const noexcept = 0;

    // increment

    virtual ConstForwardIterator& operator++() = 0;
};

template <typename VType>
class BidirectionalIterator : public ForwardIterator<VType> {
public:
    using ValueType         = VType;
    using Base              = ForwardIterator<ValueType>;
    using Pointer           = typename Base::Pointer;
    using Reference         = typename Base::Reference;
    using ConstPointer      = typename Base::ConstPointer;
    using ConstReference    = typename Base::ConstReference;
    using SizeType          = typename Base::SizeType;
    using RightReference    = typename Base::RightReference;

    virtual BidirectionalIterator& operator--() = 0;
};

template <typename VType>
class ConstBidirectionalIterator : public ConstForwardIterator<VType> {
public:
    using ValueType         = VType;
    using Base              = ConstForwardIterator<ValueType>;
    using Pointer           = typename Base::Pointer;
    using Reference         = typename Base::Reference;
    using ConstPointer      = typename Base::ConstPointer;
    using ConstReference    = typename Base::ConstReference;
    using SizeType          = typename Base::SizeType;
    using RightReference    = typename Base::RightReference;

    virtual ConstBidirectionalIterator& operator--() = 0;
};

template <typename VType>
class RandomAccessIterator : public BidirectionalIterator<VType> {
public:
    using ValueType         = VType;
    using Base              = BidirectionalIterator<ValueType>;
    using Pointer           = typename Base::Pointer;
    using Reference         = typename Base::Reference;
    using ConstPointer      = typename Base::ConstPointer;
    using ConstReference    = typename Base::ConstReference;
    using SizeType          = typename Base::SizeType;
    using ItDiff            = int;
    using RightReference    = typename Base::RightReference;

    virtual RandomAccessIterator& operator+= (ItDiff) = 0;
    virtual RandomAccessIterator& operator-= (ItDiff) = 0;
    virtual ItDiff operator- (const RandomAccessIterator&) const = 0;
    virtual bool operator> (const RandomAccessIterator&) const = 0;
    virtual bool operator< (const RandomAccessIterator&) const = 0;
    virtual bool operator>= (const RandomAccessIterator&) const = 0;
    virtual bool operator<= (const RandomAccessIterator&) const = 0;
};

template <typename VType>
class ConstRandomAccessIterator : public ConstBidirectionalIterator<VType> {
public:
    using ValueType         = VType;
    using Base              = ConstBidirectionalIterator<ValueType>;
    using Pointer           = typename Base::Pointer;
    using Reference         = typename Base::Reference;
    using ConstPointer      = typename Base::ConstPointer;
    using ConstReference    = typename Base::ConstReference;
    using SizeType          = typename Base::SizeType;
    using ItDiff            = int;
    using RightReference    = typename Base::RightReference;

    virtual ConstRandomAccessIterator& operator+= (int) = 0;
    virtual ConstRandomAccessIterator& operator-= (int) = 0;
    virtual ItDiff operator- (const ConstRandomAccessIterator&) const = 0;
    virtual bool operator> (const ConstRandomAccessIterator&) const = 0;
    virtual bool operator< (const ConstRandomAccessIterator&) const = 0;
    virtual bool operator>= (const ConstRandomAccessIterator&) const = 0;
    virtual bool operator<= (const ConstRandomAccessIterator&) const = 0;
};

template <class T, class VT>
concept IsForwardIterator = std::derived_from<T, ForwardIterator<VT>> || std::derived_from<T, ConstForwardIterator<VT>>;

template <class T, class VT>
concept IsBidirectionalIterator = std::derived_from<T, BidirectionalIterator<VT>> || std::derived_from<T, ConstBidirectionalIterator<VT>>;

template <class T, class VT>
concept IsRandomAccessIterator = std::derived_from<T, RandomAccessIterator<VT>> || std::derived_from<T, ConstRandomAccessIterator<VT>>;
