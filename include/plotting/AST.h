#ifndef AST_H
#define AST_H

#include "BTree.h"
#include "tokenizer.h"

#include <any>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

enum class NodeType{
    Num, Operation, Unknown, EOL
};

enum class Precedence {
    TERM,       // +, -
    FACTOR,     // *, /
    EXPONENT,   // ^
    PAREN       // ()
};
#include <iomanip>

using AST = BTree<Token>;

template<typename T>
void printBT(std::ostream& os, TreeNode<T>* node, const std::string& prefix = "" , bool isLeft = false){
    if(node){
        os << prefix;
        os << (isLeft ? "├──" : "└──" );

        // print the value of the node
        os << node->value << std::endl;
        // enter the next tree level - left and right branch
        printBT(os, node->left(), prefix + (isLeft ? "│   " : "    "), true);
        printBT(os, node->right(), prefix + (isLeft ? "│   " : "    "), false);
    }
}

#endif //AST_H