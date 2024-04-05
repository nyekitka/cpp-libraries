#include "iterators.hpp"
#include "exceptions.hpp"
#include "allocator.hpp"
#include "altraits.hpp"
#include <concepts>
#include <initializer_list>

template <std::default_initializable VType, TAllocator AlType>
class List;

template <typename VType>
struct listNode {
    listNode* prev;
    listNode* next;
    VType val;
    template <std::constructible_from<VType> ...Args>
    listNode(listNode* prev, listNode* next, Args&&... args): prev(prev), next(next), val(std::forward<Args>(args)...) {}
};


template <typename VType>
class listIterator : public BidirectionalIterator<VType>{
public:

    using Base              = BidirectionalIterator<VType>;
    using ValueType         = typename Base::ValueType;
    using Pointer           = typename Base::Pointer;
    using ConstPointer      = typename Base::ConstPointer;
    using Reference         = typename Base::Reference;
    using ConstReference    = typename Base::ConstReference;
    using SizeType          = typename Base::SizeType;

    template <std::default_initializable VT, TAllocator AlType>
    friend class List;

    listIterator(const Base& it) : listIterator(static_cast<const listIterator&>(it)) {}
    listIterator(const ForwardIterator<ValueType>& it) : listIterator(static_cast<const listIterator&>(it)){}

    Reference operator*() const override {
        if (node->next != nullptr) return node->val;
        else throw UndereferencableIterator();
    }
    Pointer operator->() const override {
        if (node->next != nullptr) return &(node->val);
        else throw UndereferencableIterator();
    }

    bool operator== (const ForwardIterator<ValueType>& other) const noexcept override {
        auto it = static_cast<const listIterator&>(other);
        return node == it.node;
    }
    bool operator!= (const ForwardIterator<ValueType>& other) const noexcept override {
        auto it = static_cast<const listIterator&>(other);
        return node != it.node;
    }

    ForwardIterator<ValueType>& operator++() override {
        if (node->next != nullptr) {
            node = node->next;
            return *this;
        } else throw IteratorOutOfBounds();
    }
    listIterator operator++(int) {
        if (node->next != nullptr) {
            node = node->next;
            return listIterator(node->prev);
        } else throw IteratorOutOfBounds();
    }
    Base& operator--() override {
        if (node->prev != nullptr) {
            node = node->prev;
            return *this;
        } else throw IteratorOutOfBounds();
    }
    listIterator operator--(int) {
        if (node->prev != nullptr) {
            node = node->prev;
            return listIterator(node->next);
        } else throw IteratorOutOfBounds();
    }
private:
    listIterator(listNode<ValueType>* node) : node(node) {}

    listNode<ValueType>* node;
};

template <typename VType>
class constListIterator : public ConstBidirectionalIterator<VType>{
public:

    using Base              = ConstBidirectionalIterator<VType>;
    using ValueType         = typename Base::ValueType;
    using Pointer           = typename Base::Pointer;
    using ConstPointer      = typename Base::ConstPointer;
    using Reference         = typename Base::Reference;
    using ConstReference    = typename Base::ConstReference;
    using SizeType          = typename Base::SizeType;

    template <std::default_initializable VT, TAllocator AlType>
    friend class List;

    ConstReference operator*() const override {
        if (node->next != nullptr) return node->val;
        else throw UndereferencableIterator();
    }
    ConstPointer operator->() const override {
        if (node->next != nullptr) return &(node->val);
        else throw UndereferencableIterator();
    }

    bool operator== (const ConstForwardIterator<ValueType>& other) const noexcept override {
        auto it = static_cast<const constListIterator&>(other);
        return node == it.node;
    }
    bool operator!= (const ConstForwardIterator<ValueType>& other) const noexcept override {
        auto it = static_cast<const constListIterator&>(other);
        return node != it.node;
    }
    ConstForwardIterator<ValueType>& operator++() override {
        if (node->next != nullptr) {
            node = node->next;
            return *this;
        } else throw IteratorOutOfBounds();
    }
    constListIterator operator++(int) {
        if (node->next != nullptr) {
            node = node->next;
            return listIterator(node->prev);
        } else throw IteratorOutOfBounds();
    }
    Base& operator--() override {
        if (node->prev != nullptr) {
            node = node->prev;
            return *this;
        } else throw IteratorOutOfBounds();
    }
    constListIterator operator--(int) {
        if (node->prev != nullptr) {
            node = node->prev;
            return listIterator(node->next);
        } else throw IteratorOutOfBounds();
    }
private:
    constListIterator(listNode<ValueType>* node) : node(node) {}

    listNode<ValueType>* node;
};

template <std::default_initializable VType, TAllocator AlType = Allocator<VType>>
class List {
public:
    using ValueType             = VType;
    using Pointer               = ValueType*;
    using Reference             = ValueType&;
    using ConstPointer          = const ValueType*;
    using ConstReference        = const ValueType&;
    using AllocatorType         = AlType;
    using Iterator              = listIterator<ValueType>;
    using ConstIterator         = constListIterator<ValueType>;
    using SizeType              = typename Iterator::SizeType;

private:
    using Node                  = listNode<ValueType>;
    using NodeAlloc             = typename AllocatorType::RebindAlloc<Node>;
    using NodeAllocTraits       = AllocatorTraits<Node, NodeAlloc>;
    using AllocTraits           = AllocatorTraits<ValueType, AllocatorType>;

public:
    List() : _size(), alloc(), nalloc() {
        head = tail = NodeAllocTraits::allocate(nalloc, 1);
        head->prev = head->next = nullptr;
    }
    List(const std::initializer_list<ValueType>& list) : alloc(), nalloc() {
        Node* ptr = NodeAllocTraits::allocate(nalloc, 1);
        head = ptr;
        _size = list.size();
        if (_size == 0) {
            tail = ptr;
            ptr->prev = ptr->next = nullptr;
        } else {
            ptr->prev = nullptr;
            auto it = list.begin();
            AllocTraits::construct(alloc, &(ptr->val), *it);
            ++it;
            for (;it != list.end(); ++it) {
                ptr->next = NodeAllocTraits::allocate(nalloc, 1);
                NodeAllocTraits::construct(nalloc, ptr->next, ptr, nullptr, *it);
                ptr = ptr->next;
            }
            ptr->next = NodeAllocTraits::allocate(nalloc, 1);
            ptr->next->prev = ptr;
            ptr->next->next = nullptr;
            tail = ptr->next;
        }
    }
    template <IsForwardIterator<ValueType> Iter>
    List(const Iter& begin, const Iter& end) : List() {
        if (begin != end) {
            AllocTraits::construct(alloc, &(head->val), *begin);
            ++_size;
            auto it = begin;
            Node* ptr = head;
            for (++it; it != end; ++it, ++_size, ptr = ptr->next) {
                ptr->next = NodeAllocTraits::allocate(nalloc, 1);
                NodeAllocTraits::construct(nalloc, ptr->next, ptr, nullptr, *it);
            }
            ptr->next = NodeAllocTraits::allocate(nalloc, 1);
            ptr->next->prev = ptr;
            ptr->next->next = nullptr;
            tail = ptr->next;
        }
    }
    template <SizeType len>
    List(const ValueType (&arr)[len]) : List() {
        _size = len;
        if (_size != 0) {
            AllocTraits::construct(alloc, &(head->val), arr[0]);
            Node* ptr = head;
            for (SizeType i = 1; i < _size; ++i, ptr = ptr->next) {
                ptr->next = NodeAllocTraits::allocate(nalloc, 1);
                NodeAllocTraits::construct(nalloc, ptr->next, ptr, nullptr, arr[i]);
            }
            ptr->next = NodeAllocTraits::alocate(nalloc, 1);
            ptr->next->prev = ptr;
            ptr->next->next = nullptr;
            tail = ptr->next;
        }
    }
    List(const List& other) : List(other.cbegin(), other.cend()) {}
    List(List&& other) : List() {
        head = other.head;
        tail = other.tail;
        _size = other._size;
    }

    ~List() {
        while (tail != head) {
            tail = tail->prev;
            NodeAllocTraits::deallocate(nalloc, tail->next, 1);
            AllocTraits::destroy(alloc, &(tail->val));
        }
        NodeAllocTraits::deallocate(nalloc, head, 1);
        _size = 0;
    }

    Iterator begin() noexcept {
        return Iterator(head);
    }
    Iterator end() noexcept {
        return Iterator(tail);
    }
    ConstIterator cbegin() const noexcept {
        return ConstIterator(head);
    }
    ConstIterator cend() const noexcept {
        return ConstIterator(tail);
    }
    SizeType size() const noexcept {
        return _size;
    }

    std::enable_if_t<std::equality_comparable<ValueType>, bool> operator== (const List& other) const noexcept {
        if (_size != other._size) return false;
        else {
            for (Node* ptr1 = head, *ptr2 = other.head; ptr1 != tail && ptr2 != other.tail; ptr1 = ptr1->next, ptr2 = ptr2->next) {
                if (ptr1->val != ptr2->val) return false;
            }
            return true;
        }
    }
    std::enable_if_t<std::equality_comparable<ValueType>, bool> operator!= (const List& other) const noexcept {
        if (_size != other._size) return true;
        else {
            for (Node* ptr1 = head, *ptr2 = other.head; ptr1 != tail && ptr2 != other.tail; ptr1 = ptr1->next, ptr2 = ptr2->next) {
                if (ptr1->val != ptr2->val) return true;
            }
            return false;
        }
    }

    void operator= (const List& other) {
        Node *ptr1 = head, *ptr2 = other.head;
        for (;ptr1->next != nullptr && ptr2->next != nullptr; ptr1 = ptr1->next, ptr2 = ptr2->next) {
            ptr1->val = ptr2->val;
        }
        if (ptr1->next == nullptr && ptr2->next != nullptr) {
            insert(Iterator(ptr1), Iterator(ptr2), Iterator(other.tail));
        } else if (ptr1->next != nullptr && ptr2->next == nullptr) {
            erase(Iterator(ptr1), Iterator(tail));
        }
    }
    void operator= (List&& other) {
        this->~List();
        alloc = other.alloc;
        nalloc = other.nalloc;
        head = other.head;
        tail = other.tail;
        _size = other._size;
    }
    void operator= (const std::initializer_list<ValueType>& list) {
        auto it = list.begin();
        Node* ptr = head;
        for (; it != list.end() && ptr->next != nullptr; ++it, ptr = ptr->next) {
            ptr->val = *it;
        }
        if (ptr->next == nullptr && it != list.end()) {
            insert(Iterator(ptr), it, list.end());
        } else if (ptr->next != nullptr && it == list.end()) {
            erase(Iterator(ptr), Iterator(tail));
        }
    }
    template <SizeType len>
    void operator= (const ValueType (&array)[len]) {
        SizeType i = 0;
        Node* ptr = head;
        for (; i < len && ptr->next != nullptr; ++i, ptr = ptr->next) {
            ptr->val = array[i];
        }
        if (ptr->next == nullptr && i < len) {
            insert(Iterator(ptr), array[i], end());
        } else if (ptr->next != nullptr && i == len) {
            erase(Iterator(ptr), Iterator(tail));
        }
    }

    template <IsForwardIterator<ValueType> Iter>
    void insert(Iterator where, Iter begin, Iter end) {
        if (begin != end) {
            Node* first = NodeAllocTraits::allocate(nalloc, 1);
            NodeAllocTraits::construct(nalloc, first, where.node->prev, nullptr, *begin);
            ++_size;
            Node* last = first;
            for (++begin; begin != end; ++begin, last = last->next, ++_size) {
                last->next = NodeAllocTraits::allocate(nalloc, 1);
                NodeAllocTraits::construct(nalloc, last->next, last, nullptr, *begin);
            }
            last->next = where.node;
            if (where.node->prev == nullptr) head = first;
            where.node->prev = last;
        }
    }
    void insert(Iterator where, ConstReference val) {
        emplace(where, val);
    }
    void insert(Iterator where, ValueType&& val) {
        emplace(where, std::move(val));
    }

    void erase(Iterator begin, Iterator end) {
        Node* ptr = begin.node, *prev = begin.node->prev;
        if (prev) prev->next = end.node;
        else head = end.node;
        while (ptr != end.node) {
            AllocTraits::destroy(alloc, &(ptr->val));
            ptr = ptr->next;
            NodeAllocTraits::deallocate(nalloc, ptr->prev, 1);
            --_size;
        }
        end.node->prev = prev;
    }
    void erase(Iterator where) {
        Iterator end = where;
        erase(where, ++end);
    }

    template <typename... Args>
    void emplace(Iterator where, Args&&... args) {
        Node* node = NodeAllocTraits::allocate(nalloc, 1);
        NodeAllocTraits::construct(nalloc, node, where.node->prev, where.node, std::forward<Args>(args)...);
        if (where.node->prev == nullptr) head = node;
        else where.node->prev->next = node;
        where.node->prev = node;
        ++_size;
        where.node = node;
    }
    void append(ConstReference val) {
        insert(Iterator(tail), val);
    }
    void append(ValueType&& val) {
        insert(Iterator(tail), std::move(val));
    }
    void pop_back() {
        if (_size == 0) throw NothingToErase();
        else erase(Iterator(tail->prev));
    }
    void pop_front() {
        if (_size == 0) throw NothingToErase();
        else erase(Iterator(head));
    }
    Reference front() {
        if (_size) {
            return head->val;
        } else throw UndereferencableIterator();
    }
    Reference back() {
        if (_size) {
            return tail->prev->val;
        } else throw UndereferencableIterator();
    }
private:
    Node *head, *tail;
    SizeType _size;
    AllocatorType alloc;
    NodeAlloc nalloc;
};
