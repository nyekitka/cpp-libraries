#include <concepts>
#include <initializer_list>
#include "list.hpp"

enum Color {
    Red, Black
};

template <typename T, typename V>
concept Comparator = requires (T a, V v) {
    {a(v, v)} -> std::same_as<bool>;
};

template <typename VtoK, typename K, typename V>
concept Converter = requires (VtoK conv, V val) {
    {conv(val)} -> std::same_as<const K&>;
};

template <typename V>
struct Less {
    bool operator()(const V& v1, const V& v2) const noexcept {
        return v1 < v2;
    }
};

template <typename V>
struct RBNode {
    RBNode *parent, *left, *right;
    V val;
    Color color;
    template <typename... Args>
    RBNode(RBNode* parent, RBNode* left, RBNode* right, Color color, Args&&... args) :
            parent(parent), left(left), right(right), color(color), val(std::forward<Args>(args)...) {}
};



template <typename VType>
class RBtreeIterator : public BidirectionalIterator<VType>{
    using Node              = RBNode<VType>;
public:
    using Base              = BidirectionalIterator<VType>;
    using ValueType         = typename Base::ValueType;
    using Pointer           = typename Base::Pointer;
    using ConstPointer      = typename Base::ConstPointer;
    using Reference         = typename Base::Reference;
    using ConstReference    = typename Base::ConstReference;
    using SizeType          = typename Base::SizeType;

    template <typename KType,
            typename VT,
            Converter<KType, VT> ConType,
            bool IsMulti,
            Comparator<KType> CType,
            TAllocator AlType>
    friend class RBTree;

    RBtreeIterator(const Base& other) : RBtreeIterator(static_cast<const RBtreeIterator&>(other)) {}
    RBtreeIterator(const ForwardIterator<ValueType>& other) : RBtreeIterator(static_cast<const RBtreeIterator&>(other)) {}

    Reference operator*() const override {
        return node->val;
    }
    Pointer operator->() const override {
        return &(node->val);
    }

    ForwardIterator<ValueType>& operator++() override {
        if (node->right != nullptr) {
            node = node->right;
            while (node->left) node = node->left;
            return *this;
        } else {
            while (node->parent) {
                if (node->parent->right == node) {
                    node = node->parent;
                } else {
                    node = node->parent;
                    if (node->right) return *this;
                }
            }
            throw IteratorOutOfBounds();
        }
    }
    RBtreeIterator operator++(int) {
        RBtreeIterator it = *this;
        this->operator++();
        return it;
    }

    Base& operator--() override {
        if (node->left != nullptr) {
            node = node->left;
            while (node->right) node = node->right;
            return *this;
        } else {
            while (node->parent) {
                if (node->parent->left == node) {
                    node = node->parent;
                } else {
                    node = node->parent;
                    if (node->left) return *this;
                }
            }
            throw IteratorOutOfBounds();
        }
    }
    RBtreeIterator operator--(int) {
        RBtreeIterator it = *this;
        this->operator--();
        return it;
    }

    bool operator==(const ForwardIterator<ValueType>& other) const noexcept override {
        return node == static_cast<const RBtreeIterator&>(other).node;
    }
    bool operator!=(const ForwardIterator<ValueType>& other) const noexcept override {
        return node != static_cast<const RBtreeIterator&>(other).node;
    }
private:
    RBtreeIterator(Node* node) : node(node) {}
    Node* node;
};

template <typename VType>
class constRBtreeIterator : public ConstBidirectionalIterator<VType>{
    using Node              = RBNode<VType>;
public:
    using Base              = ConstBidirectionalIterator<VType>;
    using ValueType         = typename Base::ValueType;
    using Pointer           = typename Base::Pointer;
    using ConstPointer      = typename Base::ConstPointer;
    using Reference         = typename Base::Reference;
    using ConstReference    = typename Base::ConstReference;
    using SizeType          = typename Base::SizeType;

    template <typename KType,
        typename VT,
        Converter<KType, VT> ConType,
        bool IsMulti,
        Comparator<KType> CType,
        TAllocator AlType>
    friend class RBTree;

    constRBtreeIterator(const Base& other) : constRBtreeIterator(static_cast<const constRBtreeIterator&>(other)) {}
    constRBtreeIterator(const ConstForwardIterator<ValueType>& other) : constRBtreeIterator(static_cast<const constRBtreeIterator&>(other)) {}

    ConstReference operator*() const override {
        return node->val;
    }
    ConstPointer operator->() const override {
        return &(node->val);
    }

    ConstForwardIterator<ValueType>& operator++() override {
        if (node->right != nullptr) {
            node = node->right;
            while (node->left) node = node->left;
            return *this;
        } else {
            while (node->parent) {
                if (node->parent->right == node) {
                    node = node->parent;
                } else {
                    node = node->parent;
                    if (node->right) return *this;
                }
            }
            throw IteratorOutOfBounds();
        }
    }
    constRBtreeIterator operator++(int) {
        constRBtreeIterator it = *this;
        this->operator++();
        return it;
    }

    Base& operator--() override {
        if (node->left != nullptr) {
            node = node->left;
            while (node->right) node = node->right;
            return *this;
        } else {
            while (node->parent) {
                if (node->parent->left == node) {
                    node = node->parent;
                } else {
                    node = node->parent;
                    if (node->left) return *this;
                }
            }
            throw IteratorOutOfBounds();
        }
    }
    constRBtreeIterator operator--(int) {
        constRBtreeIterator it = *this;
        this->operator--();
        return it;
    }

    bool operator==(const ConstForwardIterator<ValueType>& other) const noexcept override {
        return node == static_cast<const constRBtreeIterator&>(other).node;
    }
    bool operator!=(const ConstForwardIterator<ValueType>& other) const noexcept override {
        return node != static_cast<const constRBtreeIterator&>(other).node;
    }
private:
    constRBtreeIterator(Node* node) : node(node) {}
    Node* node;
};


template <typename KType,
            typename VType,
            Converter<KType, VType> ConType,
            bool IsMulti = false,
            Comparator<KType> CType = Less<KType>,
            TAllocator AlType = Allocator<VType>>
class RBTree {
    using Node              = RBNode<VType>;
public:
    using KeyType           = KType;
    using ValueType         = VType;
    using Pointer           = VType*;
    using ConstPointer      = const VType*;
    using Reference         = VType&;
    using ConstReference    = const VType&;
    using AllocatorType     = AlType;
    using ComparatorType    = CType;
    using SizeType          = size_t;
    using Iterator          = std::conditional_t<std::same_as<KeyType, ValueType>, constRBtreeIterator<ValueType>, RBtreeIterator<ValueType>>;
    using ConstIterator     = constRBtreeIterator<ValueType>;
private:

    using NodeAlloc         = typename AllocatorType::RebindAlloc<Node>;
    using NodeAllocTraits   = AllocatorTraits<Node, NodeAlloc>;
    using AllocTraits       = AllocatorTraits<ValueType, AllocatorType>;

    void LeftRotate(Node* node) noexcept {
        if (node->parent) {
            if (node->parent->left == node) {
                node->parent->left = node->right;
            } else {
                node->parent->right = node->right;
            }
        } else root = node->right;
        node->right->parent = node->parent;
        node->parent = node->right;
        node->right = node->right->left;
        if (node->right) node->right->parent = node;
        node->parent->left = node;
    }
    void RightRotate(Node* node) noexcept {
        if (node->parent) {
            if (node->parent->left == node) {
                node->parent->left = node->left;
            } else {
                node->parent->right = node->left;
            }
        } else root = node->left;
        node->left->parent = node->parent;
        node->parent = node->left;
        node->left = node->left->right;
        if (node->left) node->left->parent = node;
        node->parent->right = node;
    }

    bool IsBlack(Node* node) const noexcept {
        return !node || node->color == Black;
    }

    Node* Find(const KeyType& key) const noexcept {

        // node with the value if value is in the tree
        // and pointer to the node that possibly can be a parent for node with the value if given value is not in the tree
        // returned node may be fictional if given value is greater that every value in tree or tree is empty

        Node* node = root;
        while(1) {
            if (comp(conv(node->val), key)) {
                if (node->right && node->right != fictional) node = node->right;
                else break;
            } else if (comp(key, conv(node->val))) {
                if (node->left) node = node->left;
                else break;
            } else break;
        }
        if (comp(conv(node->val), key) && node->right == fictional) return fictional;
        else return node;
    }

    void FixAfterInsert(Node* node) {
        if (!node->parent) {
            node->color = Black;
        } else if (!IsBlack(node->parent)) {
            Node* parent = node->parent, *uncle = nullptr, *grandpa = node->parent->parent;
            if (grandpa->left == parent) uncle = grandpa->right;
            else uncle = grandpa->left;
            if (IsBlack(uncle)) {
                if ((grandpa->left == parent) != (parent->left == node)) {
                    if (parent->left == node) RightRotate(parent);
                    else LeftRotate(parent);
                    std::swap(parent, node);
                }
                if (grandpa->left == parent) RightRotate(grandpa);
                else LeftRotate(grandpa);
                grandpa->color = Red;
                parent->color = Black;
            } else {
                if (grandpa != root) grandpa->color = Red;
                uncle->color = Black;
                parent->color = Black;
                if (grandpa != root) FixAfterInsert(grandpa);
            }
        }
    }
    void FixAfterErase(Node* parent, bool isleft) {
        Node* brother = nullptr, *k = nullptr;
        if (isleft) {
            brother = parent->right;
            k = parent->left;
        }
        else {
            brother = parent->left;
            k = parent->right;
        }
        if (IsBlack(brother)) {
            Node* grandson = nullptr;
            if (isleft) grandson = brother->left;
            else grandson = brother->right;
            if (IsBlack(grandson)) {
                if (isleft) LeftRotate(parent);
                else RightRotate(parent);
                if (IsBlack(parent)) {
                    parent->color = Red;
                    if (brother->parent) FixAfterErase(brother->parent, brother->parent->left == brother);
                }
            } else {
                if (IsBlack(parent)) {
                    grandson->color = Black;
                    if (isleft) {
                        RightRotate(brother);
                        LeftRotate(parent);
                    } else {
                        LeftRotate(brother);
                        RightRotate(parent);
                    }
                } else {
                    Node* grandson2 = nullptr;
                    if (isleft) grandson2 = brother->right;
                    else grandson2 = brother->left;
                    if (IsBlack(grandson2)) {
                        grandson->color = Black;
                        brother->color = Red;
                        if (isleft) {
                            RightRotate(brother);
                            LeftRotate(parent);
                        } else {
                            LeftRotate(brother);
                            RightRotate(parent);
                        }
                    } else {
                        brother->color = Red;
                        grandson2->color = Black;
                        parent->color = Black;
                        if (isleft) LeftRotate(parent);
                        else RightRotate(parent);
                    }
                }
            }
        } else {
            if (isleft) LeftRotate(parent);
            else RightRotate(parent);
            brother->color = Black;
            parent->color = Red;
            FixAfterErase(parent, isleft);
        }
    }

    Node* LowerBound(const KeyType& key) const noexcept {
        Node* node = Find(key);
        ConstIterator beg = cbegin();
        if (comp(conv(node->val), key)) {
            return node;
        } else if (comp(key, conv(node->val))) {
            ConstIterator it(node);
            if (it == beg) return nullptr;
            else {
                --it;
                return it.node;
            }
        } else {
            ConstIterator it(node);
            while (!comp(conv(it.node->val), key) && it != beg) {
                --it;
            }
            if (it == beg && !comp(conv(it.node->val), key)) return nullptr;
            else return it.node;
        }
    }
    Node* UpperBound(const KeyType& key) const noexcept {
        Node* node = Find(key);
        if (comp(key, conv(node->val))) {
            return node;
        } else if (comp(conv(node->val), key)) {
            Iterator it(node);
            ++it;
            return it.node;
        } else {
            Iterator it(node);
            while (it.node != fictional && !comp(key, conv(it.node->val))) {
                ++it;
            }
            return it.node;
        }
    }
    Node* Insert(ValueType&& val) {
        Node* parent = Find(conv(val));
        if (parent == fictional) { // tree is empty or value is greater than every value in the tree
            parent = NodeAllocTraits::allocate(nalloc, 1);
            NodeAllocTraits::construct(nalloc, parent, fictional->parent, nullptr, fictional, Red, std::move(val));
            if (!fictional->parent) root = parent;
            else fictional->parent->right = parent;
            fictional->parent = parent;
        } else if (comp(conv(val), conv(parent->val))) { // value is less than value in the node
            parent->left = NodeAllocTraits::allocate(nalloc, 1);
            NodeAllocTraits::construct(nalloc, parent->left, parent, nullptr, nullptr, Red, std::move(val));
            parent = parent->left;
        } else if (comp(conv(parent->val), conv(val))) { // value is greater than value in the node
            parent->right = NodeAllocTraits::allocate(nalloc, 1);
            NodeAllocTraits::construct(nalloc, parent->right, parent, nullptr, nullptr, Red, std::move(val));
            parent = parent->right;
        } else {
            if constexpr(IsMulti) {
                parent->left = NodeAllocTraits::allocate(nalloc, 1);
                NodeAllocTraits::construct(nalloc, parent->left, parent, nullptr, nullptr, Red, std::move(val));
                parent = parent->left;
            } else return nullptr;
        }
        ++_size;
        FixAfterInsert(parent);
        return parent;
    }
    Node* Insert(ConstReference val) {
        Node* parent = Find(conv(val));
        if (parent == fictional) { // tree is empty or value is greater than every value in the tree
            parent = NodeAllocTraits::allocate(nalloc, 1);
            NodeAllocTraits::construct(nalloc, parent, fictional->parent, nullptr, fictional, Red, val);
            if (!fictional->parent) root = parent;
            else fictional->parent->right = parent;
            fictional->parent = parent;
        } else if (comp(conv(val), conv(parent->val))) { // value is less than value in the node
            parent->left = NodeAllocTraits::allocate(nalloc, 1);
            NodeAllocTraits::construct(nalloc, parent->left, parent, nullptr, nullptr, Red, val);
            parent = parent->left;
        } else if (comp(conv(parent->val), conv(val))) { // value is greater than value in the node
            parent->right = NodeAllocTraits::allocate(nalloc, 1);
            NodeAllocTraits::construct(nalloc, parent->right, parent, nullptr, nullptr, Red, val);
            parent = parent->right;
        } else {
            if constexpr(IsMulti) {
                if (parent->left) {
                    parent = parent->left;
                    while (parent->right) {
                        parent = parent->right;
                    }
                    parent->right = NodeAllocTraits::allocate(nalloc, 1);
                    NodeAllocTraits::construct(nalloc, parent->right, parent, nullptr, nullptr, Red, val);
                    parent = parent->right;
                } else {
                    parent->left = NodeAllocTraits::allocate(nalloc, 1);
                    NodeAllocTraits::construct(nalloc, parent->left, parent, nullptr, nullptr, Red, val);
                    parent = parent->left;
                }
            } else return nullptr;
        }
        ++_size;
        FixAfterInsert(parent);
        return parent;
    }

    void Erase(Node* node) {
        // node has two childs
        if (node->left && node->right && node->right != fictional) {
            Iterator it(node);
            --it;
            Node* closest = it.node;
            std::swap(closest->val, node->val);
            Erase(closest);
        // node is a leaf
        } else if (!node->left && (!node->right || node->right == fictional)) {
            bool isblack = IsBlack(node), isleft = node->parent && node->parent->left == node;
            Node* parent = node->parent;
            if (node->right == fictional) {
                if (parent) parent->right = fictional;
                else root = fictional;
            } else {
                if (parent->left == node) parent->left = nullptr;
                else parent->right = nullptr;
            }
            NodeAllocTraits::destroy(nalloc, node);
            NodeAllocTraits::deallocate(nalloc, node, 1);
            --_size;
            if (isblack && parent) {
                FixAfterErase(parent, isleft);
            }
        // node has one child
        } else {
            Node* child = nullptr;
            if (node->left) child = node->left;
            else child = node->right;
            std::swap(child->val, node->val);
            Erase(child);
        }
    }

    void Clear (Node* node) {
        if (node->left) Clear(node->left);
        if (node->right) Clear(node->right);
        NodeAllocTraits::destroy(nalloc, node);
        --_size;
        NodeAllocTraits::deallocate(nalloc, node, 1);
    }
public:
    RBTree() : _size(), alloc(), nalloc() {
        root = fictional = NodeAllocTraits::allocate(nalloc, 1);
        root->parent = root->right = root->left = nullptr;
        root->color = Black;
    }
    RBTree(const std::initializer_list<ValueType>& ls) : RBTree() {
        for (const auto& it : ls) {
            Insert(it);
        }
    }
    RBTree(const RBTree& tree) : _size(), alloc(), nalloc() {
        root = NodeAllocTraits::allocate(nalloc, 1);
        root->parent = nullptr;
        // (what_to_place, where_to_place)
        List<std::pair<Node*, Node*>> q;
        q.append(std::make_pair(tree.root, root));
        while (q.size() != 0) {
            std::pair<Node*, Node*> nodes = q.front();
            if (nodes.first != tree.fictional) {
                AllocTraits::construct(alloc, &(nodes.second->val), nodes.first->val);
                ++_size;
            }
            else fictional = nodes.second;
            nodes.second->color = nodes.first->color;
            if (nodes.first->left) {
                nodes.second->left = NodeAllocTraits::allocate(nalloc, 1);
                nodes.second->left->parent = nodes.second;
                q.append(std::make_pair(nodes.first->left, nodes.second->left));
            } else {
                nodes.second->left = nullptr;
            }
            if (nodes.first->right) {
                nodes.second->right = NodeAllocTraits::allocate(nalloc, 1);
                nodes.second->right->parent = nodes.second;
                q.append(std::make_pair(nodes.first->right, nodes.second->right));
            } else {
                nodes.second->right = nullptr;
            }
            q.pop_front();
        }
    }
    RBTree(RBTree&& other) : root(other.root), _size(other._size), fictional(other.fictional), alloc(other.alloc), nalloc(other.nalloc) {}
    template <IsForwardIterator<ValueType> Iter>
    RBTree(Iter first, Iter last) : RBTree() {
        while (first != last) {
            Insert(*first);
        }
    }

    ~RBTree() {
        Clear(root);
    }
    SizeType size() const noexcept {
        return _size;
    }
    Iterator begin() noexcept {
        Node* node = root;
        while (node->left) node = node->left;
        return Iterator(node);
    }
    Iterator end() noexcept {
        return Iterator(fictional);
    }
    ConstIterator cbegin() const noexcept {
        Node* node = root;
        while (node->left) node = node->left;
        return ConstIterator(node);
    }
    ConstIterator cend() const noexcept {
        return ConstIterator(fictional);
    }
    Iterator find(const KeyType& key) noexcept {
        Node* node = Find(key);
        if (!comp(key, conv(node->val)) && !comp(conv(node->val), key)) return Iterator(node);
        else return end();
    }
    ConstIterator find(const KeyType& key) const noexcept {
        Node* node = Find(key);
        if (!comp(key, conv(node->val)) && !comp(conv(node->val), key)) return ConstIterator(node);
        else return cend();
    }
    Iterator lower_bound(const KeyType& key) noexcept {
        Node* node = LowerBound(key);
        if (node) return Iterator(node);
        else return end();
    }
    ConstIterator lower_bound(const KeyType& key) const noexcept {
        Node* node = LowerBound(key);
        if (node) return ConstIterator(node);
        else return cend();
    }
    Iterator upper_bound(const KeyType& key) noexcept {
        Node* node = UpperBound(key);
        return Iterator(node);
    }
    ConstIterator upper_bound(const KeyType& key) const noexcept {
        Node* node = UpperBound(key);
        return ConstIterator(node);
    }
    std::pair<Iterator, Iterator> equal_range(const KeyType& key) noexcept {
        return std::make_pair(lower_bound(key), upper_bound(key));
    }
    std::pair<ConstIterator, ConstIterator> equal_range(const KeyType& key) const noexcept {
        return std::make_pair(lower_bound(key), upper_bound(key));
    }
    SizeType count(const KeyType& key) const noexcept {
        Node* node = Find(key);
        ConstIterator it(node), beg(cbegin());
        SizeType cnt = 0;
        while (it != beg && !comp(conv(*it), key) && !comp(key, conv(*it))) {
            ++cnt;
            --it;
        }
        it.node = node;
        ++it;
        while (it.node != fictional && !comp(conv(*it), key) && !comp(key, conv(*it))) {
            ++cnt;
            ++it;
        }
        return cnt;
    }
    bool contains(const KeyType& key) const noexcept {
        Node* node = Find(key);
        return node && node != fictional && !comp(key, conv(node->val)) && !comp(conv(node->val), key);
    }
    void insert(ConstReference val) {
        Insert(val);
    }
    void insert(ValueType&& val) {
        Insert(std::forward<ValueType>(val));
    }
    template <IsForwardIterator<ValueType> Iter>
    void insert(Iter first, Iter last) {
        while (first != last) {
            Insert(*first);
            ++first;
        }
    }
    void clear() {
        Clear(root);
        root = fictional = NodeAllocTraits::allocate(nalloc, 1);
        root->parent = root->right = root->left = nullptr;
        root->color = Black;
    }
    template <typename... Args>
    void emplace(Args&&... args) {
        ValueType val(std::forward<Args>(args)...);
        Insert(std::move(val));
    }
    void erase(const KeyType& key) {
        Node* node = Find(key);
        while (node && node != fictional && !comp(key, conv(node->val)) && !comp(conv(node->val), key)) {
            Erase(node);
            node = Find(key);
        }
    }
    void erase(Iterator iterator) {
        Node* node = iterator.node;
        Erase(node);
    }

    std::enable_if_t<!std::same_as<KeyType, ValueType>> erase(ConstIterator iterator) {
        Node* node = iterator.node;
        Erase(node);
    }
    void erase(Iterator first, Iterator last) {
        while (first.node != last.node) {
            Node* node = first.node;
            ++first;
            Erase(node);
        }
    }
    std::enable_if_t<!std::same_as<KeyType, ValueType>> erase(ConstIterator first, ConstIterator last) {
        while (first.node != last.node) {
            Node* node = first.node;
            ++first;
            Erase(node);
        }
    }
private:
    Node* root, *fictional;
    SizeType _size;
    AllocatorType alloc;
    NodeAlloc nalloc;
    ComparatorType comp;
    ConType conv;
};
