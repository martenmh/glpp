#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "BTree.h"

#include <any>
#include <cmath>
#include <string>
#include <vector>

struct Expression {
    TokenType type;
    std::any value;
};

std::optional<Token> maybe_match(std::queue<Token>& tokens, char c){
    if(tokens.front().start != nullptr && *tokens.front().start == c){
        auto tok = tokens.front();
        tokens.pop();
        return tok;
    }
    else return std::nullopt;
}

template<typename ...Arr>
std::optional<Token> maybe_match(std::queue<Token>& tokens, char c, Arr&& ...args){
    std::optional<Token> tok = maybe_match(tokens, args...);
    if(tok.has_value()) {
        return tok;
    } else if((tok = maybe_match(tokens, c)).has_value()) {
        return tok;
    } else return std::nullopt;
}

AST::Node term(std::queue<Token>& tokens);

AST::Node expression(std::queue<Token>& tokens){
    auto lhs = term(tokens);
    while(true){
        std::optional<Token> op = maybe_match(tokens, '+','-');
        if(!op.has_value())
			break;
        auto rhs = term(tokens);
        lhs = AST::Node(*op, lhs, rhs);
    }
    return lhs;
}

AST::Node factor(std::queue<Token>& tokens){
    auto tok = tokens.front();
    if(tok.type == TOKEN_LEFT_PAREN){
        tokens.pop();
        auto expr = expression(tokens);
		if(tokens.front().type == TOKEN_RIGHT_PAREN){
            tokens.pop();
		}
		return expr;
    }
    else if(tok.type == TOKEN_PIPE){
        tokens.pop();
        auto expr = expression(tokens);
        if(tokens.front().type == TOKEN_RIGHT_PAREN){
            tokens.pop();
        }
        auto absolute = AST::Node(tok, expr, AST::Node());
        return absolute;
    }
	else if(tok.type == TOKEN_IDENTIFIER){
		auto identifier = AST::Node(tok);
		tokens.pop();
        return identifier;
	}
    else if(tok.type == TOKEN_MIN || tok.type == TOKEN_PLUS){
		tokens.pop();
		return AST::Node(tok, factor(tokens), AST::Node());
	}
    tokens.pop();
    return AST::Node(tok);
}

AST::Node power(std::queue<Token>& tokens){
    auto lhs = factor(tokens);
    while(tokens.front().type != TOKEN_EOF){
        std::optional<Token> op = maybe_match(tokens, '^');
        if(!op.has_value()) break;
        auto rhs = factor(tokens);
        lhs = AST::Node(*op, lhs, rhs);
    }
    return lhs;
}

bool isFactor(const Token& token){
	return token.type == TOKEN_LEFT_PAREN ||
	        token.type == TOKEN_PIPE ||
	        token.type == TOKEN_IDENTIFIER ||
	        token.type == TOKEN_INTEGER ||
	        token.type == TOKEN_FLOAT;
}

AST::Node term(std::queue<Token>& tokens){
    auto lhs = power(tokens);
    while(tokens.front().type != TOKEN_EOF){
        std::optional<Token> op = maybe_match(tokens, '*','/');
        if(!op.has_value()){
			// if the next token is not a factor there is no implicit multiplication
			if(!isFactor(tokens.front()))
                break;
            
			
            // make implicit multiplication operator explicit
            op = lhs.value;
			op->type = tokens.front().type == TOKEN_IDENTIFIER ? TOKEN_AMBIGUOUS_IDENTIFIER : TOKEN_MUL;
		}
        auto rhs = power(tokens);
        lhs = AST::Node(*op, lhs, rhs);
    }
    return lhs;
}


AST parse(std::queue<Token>& tokens){
	return AST(expression(tokens));
}

#endif //PARSER_H
