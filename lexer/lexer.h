//
// Created by zzl on 23-5-7.
//

#ifndef CUSTOM_COMPILER_READ_SRC_FILE_H
#define CUSTOM_COMPILER_READ_SRC_FILE_H

#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>

#define STR(x) #x
#define CASE_STR(x) case x : return STR(x); break;

enum TokenType {
    Plus,   // +
    Minus,  // -
    Star,   // *
    Slash,  // /

    GE,     // >=
    GT,     // >
    EQ,     // ==
    LE,     // <=
    LT,     // <

    SemiColon, // ;
    LeftParen, // (
    RightParen,// )

    Assignment,// =

    If,
    Else,

    Int,

    Identifier,     //标识符

    IntLiteral,     //整型字面量
    StringLiteral   //字符串字面量
};

static std::string getTokenType(TokenType type) {
    switch (type) {
        CASE_STR(Plus);
        CASE_STR(Minus);
        CASE_STR(Star);
        CASE_STR(Slash);
        CASE_STR(GE);
        CASE_STR(GT);
        CASE_STR(EQ);
        CASE_STR(LE);
        CASE_STR(LT);
        CASE_STR(SemiColon);
        CASE_STR(LeftParen);
        CASE_STR(RightParen);
        CASE_STR(Assignment);
        CASE_STR(If);
        CASE_STR(Else);
        CASE_STR(Int);
        CASE_STR(Identifier);
        CASE_STR(IntLiteral);
        CASE_STR(StringLiteral);
    }
}

class Token {
private:
    unsigned int tokenValue;
    TokenType type;
    std::string text;
public:
    Token(TokenType _type, std::string _text) : type(_type), text(_text) {}

    TokenType getTokenType() {
        return type;
    }

    std::string getText() {
        return text;
    }

    void dump() {
        std::string str = std::to_string(tokenValue) + ": " + "<" + text + ", " + ::getTokenType(type) + ">";
    }
};

#endif //CUSTOM_COMPILER_READ_SRC_FILE_H
