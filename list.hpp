#ifndef _LIST_HPP_
#define _LIST_HPP_

#include "macros.hpp"
#include "node.hpp"

template<typename T>
class ListNode : Node<T, 1> {
    using nd = Node<T, 1>;
    SET_USING_CLASS(nd, b_node);
    SET_USING_CLASS(ListNode, node);
    EXTRACT_SUB_USING_T_CLASS(nd, type, type);

    public:

    using b_node::get_data;
    using b_node::set_data;

    void set_next
    
};


#endif