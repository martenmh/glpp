#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <queue>
#include <string>
#include <vector>

//#include "syntaxtree.h"

bool isarithmetic(char c){
    return  c == '+' || c == '-' || c == '*' ||
            c == '/' || c == '^';
}

enum TokenType {
    TOKEN_PLUS, TOKEN_MIN,
	TOKEN_MUL, TOKEN_MODULO,
    TOKEN_SLASH, TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN, TOKEN_POW , TOKEN_PIPE, TOKEN_AMPERSAND,
    TOKEN_ASSIGN, TOKEN_REASSIGN,   // =, :=
	TOKEN_EQUAL, TOKEN_UNEQUAL,     // ==, !=
	TOKEN_SMALLER, TOKEN_GREATER,   // <, >
	TOKEN_SMALLER_EQUAL, TOKEN_GREATER_EQUAL,   // <=, >=
	TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_IDENTIFIER,
    TOKEN_EOF, TOKEN_ERROR, TOKEN_NONE, TOKEN_NEWLINE, TOKEN_DOT,
    TOKEN_AMBIGUOUS_IDENTIFIER  // either an implicit multiplication or a function call
};

std::string tokenName(TokenType t){
    switch(t){
    case TOKEN_PLUS: return "PLUS"; break;
    case TOKEN_MIN: return "MIN"; break;
    case TOKEN_MUL: return "MUL"; break;
    case TOKEN_SLASH: return "SLASH"; break;
    case TOKEN_LEFT_PAREN: return "LEFT_PAREN"; break;
    case TOKEN_RIGHT_PAREN: return "RIGHT_PAREN"; break;
    case TOKEN_POW: return "POW"; break;
    case TOKEN_PIPE: return "PIPE"; break;
    case TOKEN_AMPERSAND: return "AMPERSAND"; break;
    case TOKEN_INTEGER: return "INTEGER"; break;
    case TOKEN_FLOAT: return "FLOAT"; break;
    case TOKEN_IDENTIFIER: return "IDENTIFIER"; break;
    case TOKEN_EOF: return "EOF"; break;
    case TOKEN_ERROR: return "ERROR"; break;
    case TOKEN_NONE: return "NONE"; break;
    case TOKEN_NEWLINE: return "NEWLINE"; break;
    case TOKEN_DOT: return "DOT"; break;
    case TOKEN_MODULO: return "MODULO"; break;
    }
}

//template<enum TokenType>
//class Tokenizer {
//
//    std::vector<bool(*)(char)> singleCharRules;
//    std::vector<bool(*)(char*)> multiCharRules;
//
//    template<typename Pred>
//    void addSingleCharRule(Pred p, TokenType type){
//        singleCharRules.push_back(p);
//    }
//    template<typename Pred>
//    void addMultiCharRule(Pred p, TokenType type){
//        multiCharRules.push_back(p);
//    }
//    std::vector<Token> operator()(std::string& input){
//        std::vector<Token> tokens;
//        Token token;
//        for(auto &c : input){
//            token.col++;
//            if(c == '\n') token.line++;
//            for(auto rule : singleCharRules){
//                if(rule(c)) tokens.emplace_back(Token(rule.type, &c));
//            }
//            for(auto rule : singleCharRules){
//                if(rule(c)) tokens.emplace_back(Token(rule.type, &c));
//            }
//        }
//
//    }
//};

struct Token {
    Token(char *string, TokenType type) : start(string), type(type) {}
    Token(char *string, TokenType type, int col, int line, int length = 0)
        : start(string), type(type), length(length), col(col), line(line) {}
    Token()=default;
	explicit Token(TokenType type): type(type){}
    TokenType type = TOKEN_NONE;
    char* start = nullptr;
    int length = 0;
    int col = 0, line = 0;
};

std::ostream& operator<<(std::ostream& os, const Token& token){
    os << "(" << tokenName(token.type) << ": " << std::string(token.start,token.length) << ")";
    return os;
}

/*
 * Tokenizer<TokenType> tokenizer;
 * tokenizer.addRule(isdigit, TOKEN_INTEGER);
 *
 * tokenizer.addRule([](char c){c == 'a';}, TOKEN_A);
 * tokenizer.addRule(isAlpha, TOKEN_IDENTIFIER);
 * tokenizer.addRule([](char){return true;}, TOKEN_UNKNOWN); // is the same as:
 * tokenizer.addDefault(TOKEN_UNKNOWN);
 * return tokenizer(string);
 */

std::queue<Token> tokenize(std::string& input){
    std::queue<Token> tokens;
    Token token = Token(&input[0], TOKEN_NONE);
    // Store the old token and initialize a new one
    auto newToken = [&tokens](Token& oldToken, char* start = nullptr, TokenType type = TOKEN_NONE){
      if(oldToken.type != TOKEN_NONE) tokens.push(oldToken);
      if(start && type != TOKEN_NONE) {
          oldToken = Token(start, type, oldToken.col, oldToken.line);
          oldToken.length++;
      }
    };

    auto extendToken = [](Token& token, TokenType type){
      token.length++;
      if(token.type != TOKEN_NONE) token.type = type;
    };

    for(auto& c : input){
        token.col++;
        if(isdigit(c)){
            if(token.type != TOKEN_INTEGER && token.type != TOKEN_FLOAT)
                newToken(token, &c, TOKEN_INTEGER);
            else extendToken(token, token.type);
        } else if(c == '.') {
            if(token.type != TOKEN_FLOAT && token.type != TOKEN_INTEGER)
                newToken(token, &c, TOKEN_FLOAT);
            else extendToken(token, TOKEN_FLOAT);
        } else if(isalpha(c)){
            if(token.type != TokenType::TOKEN_IDENTIFIER) newToken(token, &c, TOKEN_IDENTIFIER);
            else extendToken(token, TOKEN_IDENTIFIER);
        } else if(isspace(c)){
//            newToken(token);
        } else {
            switch(c){
            case '\n': token.line++; continue;
            case '+':  newToken(token, &c, TOKEN_PLUS);  continue;
            case '%':  newToken(token, &c, TOKEN_MODULO);  continue;
            case '-':  newToken(token, &c, TOKEN_MIN);   continue;
            case '/':  newToken(token, &c, TOKEN_SLASH); continue;
            case '^':  newToken(token, &c, TOKEN_POW); continue;
            case '|':  newToken(token, &c, TOKEN_PIPE); continue;
            case '&':  newToken(token, &c, TOKEN_AMPERSAND); continue;
            case '*':  newToken(token, &c, TOKEN_MUL);   continue;
            case '(':  newToken(token, &c, TOKEN_LEFT_PAREN);  continue;
            case ')':  newToken(token, &c, TOKEN_RIGHT_PAREN); continue;
            default: break;
            }
        }
    }
    // Add remaining token
    newToken(token);
    tokens.push(Token(TOKEN_EOF));
    return tokens;
}

#endif //TOKENIZER_H
