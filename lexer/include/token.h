//
// Created by zzl on 23-5-7.
//

#ifndef CUSTOM_COMPILER_TOKEN_H
#define CUSTOM_COMPILER_TOKEN_H

#include "tokenType.h"

class Token {
private:
    unsigned int tokenValue;
    TokenType type;
    std::string text;
public:
    Token() {}

    Token(TokenType _type, std::string _text) : type(_type), text(_text) {}

    void setTokenText(std::string _text) {
        text = _text;
    }

    void setTokenType(TokenType _type) {
        type = _type;
    }

    TokenType getTokenType() {
        return type;
    }

    std::string getText() {
        return text;
    }

    void dump() {
        std::string str = std::to_string(tokenValue) + ": " + "<" + text + ", " + ::getTokenType(type) + ">";
        std::cout << str << std::endl;
    }
};

#endif //CUSTOM_COMPILER_TOKEN_H
