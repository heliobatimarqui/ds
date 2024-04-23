#include <iostream>
#include <chrono>
#include <map>

using namespace std;

#define SET_USING(t, newname)                               \
using newname = t;                                          \
using newname##_const = const newname;                      \
using newname##_ptr = newname *;                            \
using newname##_const_ptr = const newname *;                \
using newname##_reference = newname &;                      \
using newname##_const_reference = const newname &;          \
using newname##_rvalue_reference = newname &&;

#define SET_USING_CLASS(t, newname)                         \
public:                                                     \
SET_USING(t, newname)                                       \
private:

#define EXTRACT_SUB_USING_T(t, u, v)                    \
using v = t::u;                                         \
using v##_const = t:: u##_const;                        \
using v##_ptr = t:: u##_ptr;                            \
using v##_const_ptr = t:: u##_const_ptr;                \
using v##_reference = t:: u##_reference;                \
using v##_const_reference = t:: u##_const_reference;    \
using v##_rvalue_reference = t:: u##_rvalue_reference;

#define EXTRACT_SUB_USING_T_CLASS(t, u, v)         \
public:                                            \
EXTRACT_SUB_USING_T(t, u, v)                       \
private:

enum Color {
    RED,
    BLACK
};

template<typename T>
class Node {
    SET_USING_CLASS(T, type);
    SET_USING_CLASS(Node, node);

    alignas(type) char m_data[sizeof(type)];
    node_ptr m_parent = nullptr;
    node_ptr m_left = nullptr;
    node_ptr m_right = nullptr;
    node_ptr m_t_null = nullptr;
    Color m_color;

    public:

    Node(type data, Color color, node_ptr t_null) : m_color(color), m_t_null(t_null), m_left(t_null), m_right(t_null), m_parent(t_null) {
        type_ptr p = reinterpret_cast<type_ptr>(m_data);
        new(p)type(std::move(data));
    };

    Node() {
        m_t_null = this;
        m_color = Color::BLACK;
    }

    Node(const Node&) = delete;
    Node(Node&&) = delete;

    ~Node() {
        if(m_t_null != this) {
            type_ptr p = reinterpret_cast<type_ptr>(m_data);
            p->~type();
        }
    }

    void set_left(node_ptr n) {
        m_left = n;
    }

    void set_right(node_ptr n) {
        m_right = n;
    }

    void set_parent(node_ptr n) {
        m_parent = n;
    }

    void set_color(Color n) {
        m_color = n;
    }

    node_ptr get_left() {
        return m_left;
    }

    node_ptr get_right() {
        return m_right;
    }

    node_ptr get_parent() {
        return m_parent;
    }

    type_reference get_data() {
        const auto& t = *this;
        return const_cast<type_reference>(t.get_data());
    }

    node_const_ptr get_left() const {
        return m_left;
    }

    node_const_ptr get_right() const {
        return m_right;
    }

    const node_ptr get_parent() const {
        return m_parent;
    }

    type_const_reference get_data() const {
        return *reinterpret_cast<type_const_ptr>(m_data);
    }

    Color get_color() const {
        return m_color;
    }

    node_const_ptr t_null() const {
        return m_t_null;
    }
};

template<typename T>
class LessComparator {
    public:
    using type = T;
    bool operator()(const T& a, const T& b) const {
        return a < b;
    }
};

template<typename T>
class Hash {
    SET_USING_CLASS(size_t, r_type);
    SET_USING_CLASS(T, type);
    public:
    
    r_type operator()(const T& v) const {
        return v;
    }
};

template<typename T, typename U>
class Hash<std::pair<T,U>> {
    using pair = std::pair<T,U>;
    SET_USING_CLASS(size_t, r_type);
    SET_USING_CLASS(pair, type);
    public:

    r_type operator()(type_const_reference v) const {
        return v.first;
    }
};

template<typename T>
class Allocator {
    SET_USING_CLASS(T, type);
    public:

    Allocator() {
        m_i = 0;
    }

    size_t m_i;

    template<typename ...Args>
    type_ptr create(Args... args) {
        type_ptr v = allocate();
        if(v != nullptr) {
            new(v)type(std::forward<Args>(args)...);
        }

        return v;
    }

    void destroy(type_const_ptr p) {
        if(p == nullptr)
            return;

        type_ptr p_nc = const_cast<type_ptr>(p);
        (*p_nc).~type();
        deallocate(p_nc);
    }

    type_ptr allocate() {
        return reinterpret_cast<type_ptr>(malloc(sizeof(type)));
    }
    
    void deallocate(type_const_ptr p) {
        if(p == nullptr)
            return;
        type_ptr p_nc = const_cast<type_ptr>(p);
        free(p_nc);
    }
};

template<typename T, template<typename> class Hash, template <typename> class Cmp, template<typename> class Alloc> 
class RedBlackTree {
    EXTRACT_SUB_USING_T_CLASS(Node<T>, node, node);
    EXTRACT_SUB_USING_T_CLASS(node, type, type);
    using _hs = Hash<type>;
    SET_USING_CLASS(_hs, hsh);
    using _alloc = Alloc<node>;
    SET_USING_CLASS(_alloc, allocator);
    using _cmp = Cmp<typename hsh::r_type>;
    SET_USING_CLASS(_cmp, comparator);
    
    template<bool reverse, bool cnst = false>
    class MapIterator {
        const RedBlackTree* m_tree;
        node_const_ptr m_n;
        
        MapIterator(const RedBlackTree* tree, node_const_ptr n) : m_tree(tree), m_n(n) {};

        template<bool c>
        struct wrapper {
            node_const_ptr n;
            wrapper(node_const_ptr node) : n(node) {};
            node_const_ptr get_n() { return n; }
        };

        template<>
        struct wrapper<false> {
            node_const_ptr n;
            wrapper(node_const_ptr node) : n(node) {};
            node_ptr get_n() { return const_cast<node_ptr>(n); }
        };
        public:

        MapIterator(const MapIterator& other) : m_tree(other.m_tree), m_n(other.m_n) {};

        auto& operator*() const {
            auto n = wrapper<cnst>(m_n).get_n();
            return n->get_data();
        }

        auto operator->() const {
            auto n = wrapper<cnst>(m_n).get_n();
            return &(n->get_data());
        }

        MapIterator& operator++() {
            if constexpr(reverse == false) {
                m_n = m_tree->get_in_order_successor(m_n);
            }
            if constexpr (reverse == true) {
                m_n = m_tree->get_in_order_predecessor(m_n);
            }
            return *this;
        }

        MapIterator operator++(int) {
            auto other = *this;
            ++(*this);
            return other;
        }

        MapIterator& operator--() {
             if constexpr(reverse == false) {
                m_n = m_tree->get_in_order_predecessor(m_n);
            }
            if constexpr (reverse == true) {
                m_n = m_tree->get_in_order_successor(m_n);
            }
            return *this;
        }

        MapIterator operator--(int) {
            auto other = *this;
            --(*this);
            return other;
        }

        MapIterator& operator=(const MapIterator& other) {
            m_tree = other.m_tree;
            m_n = other.m_n;

            return *this;
        }

        bool operator==(const MapIterator& other) const {
            return m_n == other.m_n && m_tree == other.m_tree;
        }

        bool operator!=(const MapIterator& other) const {
            return !(*(this) == other);
        }

        operator MapIterator<reverse, true>() const {
            return MapIterator<reverse, true>(m_tree, m_n);
        }

        friend class RedBlackTree;
    };

    using it = MapIterator<false>;
    using rit = MapIterator<true>;
    using cit = MapIterator<false, true>;
    using crit = MapIterator<true,true>;

    SET_USING_CLASS(it, iterator);
    SET_USING_CLASS(rit, reverse_iterator);
    SET_USING_CLASS(cit, const_iterator);
    SET_USING_CLASS(crit, const_reverse_iterator);

    allocator m_allocator;
    node m_t_null;
    node_ptr m_root;

    node_const_ptr find_helper(type_const_reference key, node_const_ptr* parent_save) const {
        comparator_const c;
        hsh_const h;
        node_ptr current = m_root;
        *parent_save = t_null();
        while(current != t_null()) {
            if(c(h(key), h(current->get_data()))) {
                *parent_save = current;
                current = current->get_left();
                continue;
            }
            if(c(h(current->get_data()), h(key))) {
                *parent_save = current;
                current = current->get_right();
                continue;
            }

            break;
        }
        return current;
    }

    node_ptr find_helper(type_const_reference key, node_ptr* p) {
        const auto& c = *this;
        node_const_ptr r = nullptr;
        node_const_ptr p2 = nullptr;

        r = c.find_helper(key, &p2);
        *p = const_cast<node_ptr>(p2);
        return const_cast<node_ptr>(r);
    }

    void insert_fix(node_ptr n) {
        node_ptr p = nullptr;
        node_ptr u = nullptr;
        node_ptr gp = nullptr;

        if(n == m_root) {
            n->set_color(Color::BLACK);
            return;
        }

        p = n->get_parent();

        if(!is_red(n) || !is_red(p))
            return;

        if(p)
            gp = p->get_parent();
        if(gp)
            u = gp->get_left() == p ? gp->get_right() : gp->get_left();

        if(is_red(u)) {
            gp->set_color(Color::RED);
            u->set_color(Color::BLACK);
            p->set_color(Color::BLACK);
            insert_fix(gp);  
        } else if(is_black(u)) {
            if(is_right_child(p)) {
                if(is_right_child(n)) {
                    rotate_left(gp);
                    p->set_color(Color::BLACK);
                    gp->set_color(Color::RED);
                }
                else if(is_left_child(n)) {
                    n->set_color(Color::BLACK);
                    p->set_color(Color::RED);
                    gp->set_color(Color::RED);
                    rotate_right(n);
                    rotate_left(p);
                }
            } else if(is_left_child(p)) {
                if(is_left_child(n)){
                    rotate_right(gp);
                    p->set_color(Color::BLACK);
                    gp->set_color(Color::RED);
                }else if(is_right_child(n)) {
                    n->set_color(Color::BLACK);
                    p->set_color(Color::RED);
                    gp->set_color(Color::RED);
                    rotate_left(n);
                    rotate_right(p);
                }
            }
        }        
    }

    void remove_fix(node_ptr n) { 
        while (n != m_root && is_black(n)) {
            node_ptr p = n->get_parent();
            node_ptr s;
            if(is_left_child(n,p)) {
                s = p->get_right();
                if (is_red(s)) {
                    s->set_color(Color::BLACK);
                    p->set_color(Color::RED);
                    rotate_left(p);
                    s = p->get_right();
                }
                if (is_black(s->get_left()) && is_black(s->get_right())) {
                    s->set_color(Color::RED);
                    n = p;
                    p = p->get_parent();
                } else {
                    if (is_black(s->get_right())) {
                        s->get_left()->set_color(Color::BLACK);
                        s->set_color(Color::RED);
                        rotate_right(s);
                        s = p->get_right();
                    }
                    s->set_color(p->get_color());
                    p->set_color(Color::BLACK);
                    s->get_right()->set_color(Color::BLACK);
                    rotate_left(p);
                    n = m_root;
            }
            } else {
                s = p->get_left();
                if (is_red(s)) {
                    s->set_color(Color::BLACK);
                    p->set_color(Color::RED);
                    rotate_right(p);
                    s = p->get_left();
                }
                if(is_black(s->get_right()) && is_black(s->get_left())) {
                    s->set_color(Color::RED);
                    n = p;
                    p = p->get_parent();
                } else {
                    if(is_black(s->get_left())) {
                        s->get_right()->set_color(Color::BLACK);
                        s->set_color(Color::RED);
                        rotate_left(s);
                        s = p->get_left();
                    }
                    s->set_color(p->get_color());
                    p->set_color(Color::BLACK);
                    s->get_left()->set_color(Color::BLACK);
                    rotate_right(p);
                    n = m_root;
                }
            }
        }
        if(n)
            n->set_color(Color::BLACK);
    }

    bool is_left_child(node_const_ptr n, node_const_ptr p = nullptr) const {
        if(n != nullptr) {
            if(n->get_parent() != nullptr) {
                return n->get_parent()->get_left() == n;
            }
        }
        if(p != nullptr) {
            if(p->get_left() == n)
                return true;
        }
        return false;
    }

    bool is_right_child(node_const_ptr n, node_const_ptr p = nullptr) const {
        if(n){ 
            if(n->get_parent()) {
                return n->get_parent()->get_right() == n;
            }
        }
        if(p) {
            if(n == p->get_right())
                return true;
        }
        return false;
    }

    void rotate_left(node_ptr n) {
        if(n == nullptr || n == t_null())
            return;

        node_ptr nr = n->get_right();

        if(nr) {
            n->set_right(nr->get_left());

            if(n->get_right())
                n->get_right()->set_parent(n);

            nr->set_left(n);
            nr->set_parent(n->get_parent());
            if(is_left_child(n))
                n->get_parent()->set_left(nr);
            else if(is_right_child(n))
                n->get_parent()->set_right(nr);
            else 
                m_root = nr;

            n->set_parent(nr);
        }
    }

    void rotate_right(node_ptr n) {
        if(n == nullptr || n == t_null())
            return;

        node_ptr nl = n->get_left();

        if(nl) {
            n->set_left(nl->get_right());
            if(n->get_left())
                n->get_left()->set_parent(n);
            
            nl->set_right(n);
            nl->set_parent(n->get_parent());

            if(is_left_child(n))
                n->get_parent()->set_left(nl);
            else if(is_right_child(n))
                n->get_parent()->set_right(nl);
            else 
                m_root = nl;

            n->set_parent(nl);
        }
    }

    inline bool is_red(node_ptr p) {
        if(p)
            return p->get_color() == Color::RED;
        return false;
    }

    inline bool is_black(node_ptr p) {
        return !is_red(p);
    }

    node_ptr find_minimum(node_ptr n) {
        const auto& c = *this;
        return const_cast<node_ptr>(c.find_minimum(n));
    }

    node_const_ptr find_minimum(node_const_ptr n) const {
        node_const_ptr ret_val = n;
        while(n != t_null()) {
            ret_val = n;
            n = n->get_left();
        }
        return ret_val;
    }

    node_ptr find_maximum(node_ptr n) {
        const auto& c = *this;
        return const_cast<node_ptr>(c.find_maximum(n));
    }

    node_const_ptr find_maximum(node_const_ptr n) const {
        node_const_ptr ret_val = n;
        while(n != t_null()) {
            ret_val = n;
            n = n->get_right();
        }
        return ret_val;
    }

    void transplant(node_ptr n, node_ptr sn) {
        if(n == t_null())
            return;

        node_ptr p = n->get_parent();
        if(p != t_null()) {
            if(p->get_left() == n)
                p->set_left(sn);
            if(p->get_right() == n)
                p->set_right(sn);
        }
        else {
            m_root = sn;
        }

        if(sn) {
            if(sn != n->get_left())
                sn->set_left(n->get_left());
            if(sn != n->get_right())
                sn->set_right(n->get_right());
            if(sn->get_left())
                sn->get_left()->set_parent(sn);
            if(sn->get_right())
                sn->get_right()->set_parent(sn);
            sn->set_parent(n->get_parent());
        }
    }

    void destructor_helper(node_ptr n) {
        if(n->get_left() != t_null()) {
            destructor_helper(n->get_left());
            m_allocator.destroy(n->get_left());
        }
        if(n->get_right() != t_null()) {
            destructor_helper(n->get_right());
            m_allocator.destroy(n->get_right());
        }

        if(n == m_root)
            m_allocator.destroy(n);
    }

    public:

    RedBlackTree() {
        m_root = t_null();
    }

    ~RedBlackTree() {
        if(m_root == t_null())
            return;

        destructor_helper(m_root);
    }

    void remove(const T& key) {
        node_ptr p = nullptr;
        node_ptr n = find_helper(key, &p);
        node_ptr x = t_null();
        if(n == t_null())
            return;

        p = n->get_parent();
        Color original_color = n->get_color();

        if(n->get_left() == t_null()) {
            x = n->get_right();
            transplant(n,x);
        }
        else if(n->get_right() == t_null()) {
            x = n->get_left();
            transplant(n,x);
        }
        else {
            node_ptr y = find_minimum(n->get_right());
            original_color = y->get_color();
            x = y->get_right();
            if(y == n->get_right()) {
                x->set_parent(y);
            }
            else {
                transplant(y, x);
                x->set_parent(y->get_parent());
            }
            transplant(n, y);
            y->set_color(original_color);
        }

        if(original_color == Color::BLACK)
            remove_fix(x);

        m_allocator.destroy(n);
    }

    void insert(const T& key) {
        node_ptr n = m_allocator.create(key, Color::RED, t_null());

        // Handle edge case when we don't have a root node
        if(m_root == t_null()) {
            m_root = n;
            n->set_color(Color::BLACK);
            return;
        }

        // Find to which node we are going to insert node n
        node_ptr p;
        find_helper(n->get_data(), &p);        

        comparator_const c;
        hsh_const h;
        if(c(h(key), h(p->get_data()))) {
            p->set_left(n);
        }
        else {
            p->set_right(n);
        }

        n->set_parent(p);
        insert_fix(n);
    }

    bool contains(const T& key) const {
        node_const_ptr p;
        return find_helper(key, &p) != t_null();
    }

    node_ptr get_node(const T& key) {
        const auto& c = *this;
        return const_cast<node_ptr>(c.get_node(key));
    }

    node_const_ptr get_node(const T& key) const {
        node_const_ptr p = nullptr;
        return find_helper(key, &p);
    }
    
    node_ptr get_in_order_successor(node_ptr n) {
        const auto& c = *this;
        return const_cast<node_ptr>(c.get_in_order_successor(n));
    }

    node_const_ptr get_in_order_successor(node_const_ptr n) const {
        if(m_root == t_null())
            return t_null();
        
        if(n == nullptr) {
            return find_minimum(m_root);
        }

        if(n != t_null()){
            if(n->get_right() != t_null()) {
                return find_minimum(n->get_right());
            }
            else {
                while(n != t_null() && !is_left_child(n)) {
                    n = n->get_parent();
                }
                if(n != t_null())
                    return n->get_parent();
            }
        }

        return t_null();
    }

    node_ptr get_in_order_predecessor(node_ptr n) {
        const auto& c = *this;
        return const_cast<node_ptr>(c.get_in_order_predecessor(n));
    }

    node_const_ptr get_in_order_predecessor(node_const_ptr n) const {
        if(m_root == t_null())
            return t_null();

        if(n != nullptr) {    
            if(n == t_null())
                return find_maximum(m_root);

            if(n->get_left() != t_null()) 
                return find_maximum(n->get_left());

            if(is_right_child(n))
                return n->get_parent();

            while(n != t_null() && !is_right_child(n)) {
                n = n->get_parent();
            }

            if(n != t_null())
                return n->get_parent();
        }

        return nullptr;
    }
    
    node_const_ptr get_root() const {
        return m_root;
    }

    node_ptr t_null() {
        const auto& c = *this;
        return const_cast<node_ptr>(c.t_null());
    }

    node_const_ptr t_null() const {
        return &m_t_null;
    }

    node_const_ptr equal_or_greater(type_const_reference k) const {
        node_const_ptr p = t_null();
        node_const_ptr n = find_helper(k, &p);
        return n ? n : p;
    }

    iterator begin() {
        return iterator(this, get_in_order_successor(nullptr));
    }

    iterator end() {
        return iterator(this, t_null());
    }

    reverse_iterator rbegin() {
        return reverse_iterator(this, get_in_order_predecessor(t_null()));
    }

    reverse_iterator rend() {
        return reverse_iterator(this, nullptr);
    }

    const_iterator begin() const {
        return const_iterator(this, get_in_order_successor(nullptr));
    }

    const_iterator end() const {
        return const_iterator(this, t_null());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(this, get_in_order_predecessor(t_null()));
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(this, nullptr);
    }

    bool empty() const {
        return get_root() != t_null();
    }

    size_t size() const {
        if(empty())
            return 0;

        size_t c = 0;
        for(auto i = begin(); i != end(); ++i) {
            ++c;
        }
        return c;
    }   

    size_t max_size() const  {
        return UINT64_MAX;
    }
    
    template<bool a, bool b>
    bool is_tree_iterator(const MapIterator<a,b>& it) {
        return it->m_tree == this;
    }

    template<typename IteratorType>
    IteratorType build_iterator(node_const_ptr n) const {
        if(n) {
            if(n->t_null() == t_null())
                return IteratorType(this, n);
        }

        return end();
    }

};

template<typename Key, typename Value, template <typename> class Allocator>
class Map : RedBlackTree<std::pair<Key,Value>, Hash, LessComparator, Allocator> {
    SET_USING_CLASS(Key, key);
    SET_USING_CLASS(Value, value);
    using PR = std::pair<key, value>;
    SET_USING_CLASS(PR, pair);
    using rb_tree = RedBlackTree<std::pair<Key,Value>, Hash, LessComparator, Allocator>;
    EXTRACT_SUB_USING_T_CLASS(rb_tree, node, node);
    EXTRACT_SUB_USING_T_CLASS(rb_tree, const_iterator, iterator);
    EXTRACT_SUB_USING_T_CLASS(rb_tree, const_reverse_iterator, reverse_iterator);

public:

    Map() = default;
    ~Map() = default;
    
    using rb_tree::empty;
    using rb_tree::size;
    using rb_tree::max_size;

    bool contains(key_const_reference k) const {
        pair p = {k, {}};
        return rb_tree::contains(p);
    }

    pair_reference insert(key_const_reference k, value_const_reference v) {
        pair p = {k, v};
        if(!rb_tree::contains(p)) {
            rb_tree::insert(p);
        }

        node_ptr n = rb_tree::get_node(p);
        return n->get_data();
    }

    void remove(key_const_reference k) {
        pair p = {k,{}};
        if(rb_tree::contains(p)) {
            node_ptr n = rb_tree::get_node(p);
            rb_tree::remove(n);
        }
    }

    void erase(iterator_const_reference it) {
        if(!is_tree_iterator(it)) 
            return;
        if(it == end())
            return;
        
        node_ptr n = rb_tree::get_node(*it);
        rb_tree::remove(n);
    }

    value_reference operator[](key_const_reference k) {
        pair p = {k, {}};
        if (!rb_tree::contains(p)) {
            rb_tree::insert(p);   
        }
        node_ptr n = rb_tree::get_node(p);
        return n->get_data().second;
    }

    iterator lower_bound(key_const_reference k) const {
        pair p {k, {}};
        return rb_tree:: template build_iterator<iterator>(rb_tree::equal_or_greater(p));
    }

    iterator upper_bound(key_const_reference k) const {
        typename rb_tree::comparator_const c;
        typename rb_tree::hsh_const h;

        pair p {k, {}};
        node_const_ptr n = rb_tree::equal_or_greater(p);
        
        if(n == rb_tree::t_null() || c(h(n->get_data()), h(p)))
            return rb_tree:: template build_iterator<iterator>(n);

        return rb_tree:: template build_iterator<iterator>(rb_tree::get_in_order_successor(n));
    }

    value_const_reference at(key_const_reference k) const {
        if(contains(k)) {
            pair p {k, {}};
            node_const_ptr n = rb_tree::get_node(p);
        }
    }

    iterator begin() const {
        return rb_tree::begin();
    }

    iterator end() const {
        return rb_tree::end();
    }

    reverse_iterator rbegin() const {
        return rb_tree::rbegin();
    }

    reverse_iterator rend() const {
        return rb_tree::rend();
    }
};

int main() {
    Map<int, int, Allocator> m;

    for(size_t i = 0; i < 100; ++i) {
        m.insert(i,i);
    }


    auto it = m.lower_bound(20);
    auto it2 = m.upper_bound(20);

    std::cout << it->first << " " << it->second << "\r\n";
    std::cout << it2->first << " " << it2->second << "\r\n";





  //for(auto n = V.get_in_order_successor(nullptr); n != nullptr; n = V.get_in_order_successor(n)) {
  //  std::cout << n->get_data() << "\r\n";
  //}

};