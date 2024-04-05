#include "rbtree.hpp"

template <class T>
struct SetConverter {
    const T& operator() (const T& val) const noexcept {
        return val;
    }
};


template <typename KType,
            Comparator<KType> CType = Less<KType>,
            TAllocator AlType = Allocator<KType>>
class Set : public RBTree<KType, KType, SetConverter<KType>, false, CType, AlType> {
public:
    using Base              = RBTree<KType, KType, SetConverter<KType>, false, CType, AlType>;
    using KeyType           = typename Base::KeyType;
    using Pointer           = typename Base::Pointer;
    using ConstPointer      = typename Base::ConstPointer;
    using Reference         = typename Base::Reference;
    using ConstReference    = typename Base::ConstReference;
    using AllocatorType     = typename Base::AllocatorType;
    using ComparatorType    = typename Base::ComparatorType;
    using SizeType          = typename Base::SizeType;
    using Iterator          = typename Base::Iterator;
    using ConstIterator     = typename Base::ConstIterator;

    Set() : Base() {}
    Set(const std::initializer_list<KeyType>& list) : Base(list) {}
    template <IsForwardIterator<KeyType> Iter>
    Set(Iterator first, Iterator last) : Base(first, last) {}

    SizeType count(const KeyType& key) const noexcept = delete;

    Set& operator+= (const Set& other) {
        Base::insert(other.cbegin(), other.cend());
        return *this;
    }

    Set operator+ (const Set& other) const {
        Set res = *this;
        res += other;
        return res;
    }

};
