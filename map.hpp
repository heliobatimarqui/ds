#ifndef _MAP_HPP_
#define _MAP_HPP_

#include "rb_tree.hpp"

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

#endif