#ifndef BTREE_H
#define BTREE_H

#include <memory>

template<typename T>
class TreeNode {
    std::shared_ptr<TreeNode> m_left;
    std::shared_ptr<TreeNode> m_right;
  public:
    T value;
    explicit TreeNode(T val = T()) : value(val){}
    TreeNode(T val, const TreeNode& left, const TreeNode& right) : value(val),
                                                                   m_left(std::make_shared<TreeNode>(left)),
                                                                   m_right(std::make_shared<TreeNode>(right)) {}

    TreeNode* right(){ return m_right.get(); }
    TreeNode* left(){ return m_left.get(); }

    TreeNode* right(int i){
        TreeNode* node = this;
        for(int j = 0; j != i; j++){
            if(!node->m_right) throw std::out_of_range("TreeNode::right(i) out of range");
            node = node->right();
        }
        return node;
    }

    TreeNode* left(int i){
        TreeNode* node = this;
        for(int j = 0; j != i; j++){
            if(!node->m_left) throw std::out_of_range("TreeNode::right(i) out of range");
            node = node->m_left.get();
        }
        return node;
    }
};

template<typename T>
class BTree : public TreeNode<T>{
  public:
    using Node = TreeNode<T>;
    explicit BTree(const Node& node): Node(node){}

    /**
     * BTree Iterator
     */
    class iterator {
        Node* pointer;
        std::shared_ptr<iterator> parent;
      public:
        explicit iterator(Node* ptr): pointer(ptr){}
        iterator(Node* ptr, iterator parent): pointer(ptr), parent(std::make_shared<iterator>(parent)){}
        bool operator!=(const iterator& rhs) const {
            return pointer != rhs.pointer && parent != rhs.parent;
        }
        iterator& operator++() {
            if(pointer->left()) {
                parent = std::make_shared<iterator>(*this);
                pointer = pointer->left();
            }
            else if(pointer->right()) {
                parent = std::make_shared<iterator>(*this);
                pointer = pointer->right();
            } else {
                // Go to right node of parent
                if (parent->pointer->left() == pointer && parent->pointer->right()) {
                    pointer = parent->pointer->right();
                    parent = parent->parent;
                } else {
                    while(parent->parent){
                        parent = parent->parent;
                        if(parent->pointer->right() && parent->pointer->right() != pointer) {
                            pointer = parent->pointer->right();
                            break;
                        }
                    }
                    pointer = parent->pointer->right();
                }
            }
            return *this;
        }
        T& operator*() {
            return pointer->value;
        }
        T& operator->() {
            return pointer->value;
        }
    };

    iterator begin(){ return iterator(this); }
    iterator end(){ return iterator(this->right()->right(), iterator(this)); }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::queue<T>& queue){
    std::queue<T> que = queue;
    while(!que.empty()) {
        os << que.front() << " ";
        que.pop();
    }

    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, BTree<T>& tree){
    printBT(os, &tree);
    return os;
}

#endif // BTREE_H