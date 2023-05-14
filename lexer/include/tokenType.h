//
// Created by zzl on 23-5-7.
//

#ifndef CUSTOM_COMPILER_TOKENTYPE_H
#define CUSTOM_COMPILER_TOKENTYPE_H

#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <map>

//#define STR(x) #x
//#define CASE_STR(x) case x : return STR(x); break;

enum class TokenType {
    // keyword
    AUTO = 1,   //auto 1
    BREAK,  //break 2
    CASE,   //case 3
    CHAR,   //char 4
    CONST,  //const 5
    CONTINUE,   //continue 6
    DEFAULT,    //default 7
    DO,     //do 8
    DOUBLE, //double 9
    ELSE,   //else 10
    ENUM,   //enum 11
    EXTERN, //extern 12
    FLOAT,  //float 13
    FOR,    //for 14
    GOTO,   //goto 15
    IF,     //if 16
    INT,    //int 17
    LONG,   //long 18
    REGISTER,   //register 19
    RETURN,     //return 20
    SHORT,      //short 21
    SIGNED,     //signed 22
    SIZEOF,     //sizeof 23
    STATIC,     //static 24
    STRUCT,     //struct 25
    SWITCH,     //switch 26
    TYPEDEF,    //typedef 27
    UNION,      //union 28
    UNSIGNED,   //unsigned 29
    VOID,       //void 30
    VOLATILE,   //volatile 31
    WHILE,      //while 32
    MINUS,                  // - 33
    DOUBLE_MINUS,            // -- 34
    MINUS_EQUAL,            // -= 35
    MINUS_GREATER,          // -> 36
    EXCLAMATION_POINT,       // ! 37
    EXCLAMATION_POINT_EQUAL, // != 38
    PERCENT,                // % 39
    PERCENT_EQUAL,          // %= 40
    AMPERSAND,              // & 41
    DOUBLE_AMPERSAND,       // && 42
    AMPERSAND_EQUAL,        // &= 43
    OPEN_PAREN,             // ( 44
    CLOSE_PAREN,            // ) 45
    STAR,                   // * 46
    STAR_EQUAL,             // *= 47
    COMMA,                  // , 48
    POINT,                  // . 49
    SLASH,                  // / 50
    SLASH_EQUAL,            // /= 51
    COLON,                  // : 52
    SEMICOLON,              // ; 53
    QUESTION_MARK,          // ? 54
    OPEN_BRACKET,           // [ 55
    CLOSE_BRACKET,          // ] 56
    CARET,                  // ^ 57
    CARET_EQUAL,            // ^= 58
    OPEN_BRACE,             // { 59
    VERTICAL_BAR,           //  | 60
    DOUBLE_VERTICAL_BAR,    //  || 61
    VERTICAL_BAR_EQUAL,     //  |= 62
    CLOSE_BRACE,            // } 63
    TILDE,                  // ~ 64
    PLUS,                   // + 65
    PLUS_PLUS,              // ++ 66
    PLUS_EQUAL,             // += 67
    LESS_THAN,              // < 68
    LEFT_SHIFT,             // << 69
    LEFT_SHIFT_EQUAL,       // <<= 70
    LESS_EQUAL,             // <= 71
    EQUAL,                  // = 72
    DOUBLE_EQUAL,            //  == 73
    GREATER_THAN,           //     > 74
    GREATER_EQUAL,          //     >= 75
    RIGHT_SHIFT,            //  >> 76
    RIGHT_SHIT_EQUAL,       // >>= 77
    DOUBLE_QUOTATION_MARKS, // " 78
    COMMENT,                // /*注释*/ 79
    CONSTANT,               // 常数 80
    IDENTIFIER             // 标识符 81
};

class TokenTypeInfo {
    std::string tokenStr;
    int typeCode;
public:
    TokenTypeInfo(std::string str, int code) : tokenStr(str), typeCode(code) {}
};

TokenTypeInfo getTokenTypeInfo(TokenType type) {
    std::map<TokenType, TokenTypeInfo> typeInfoMap;
    typeInfoMap.insert(std::make_pair(TokenType::AUTO, TokenTypeInfo("auto", 1)));
    typeInfoMap.insert(std::make_pair(TokenType::BREAK, TokenTypeInfo("break", 2)));
    typeInfoMap.insert(std::make_pair(TokenType::CASE, TokenTypeInfo("case", 3)));
    typeInfoMap.insert(std::make_pair(TokenType::CHAR, TokenTypeInfo("char", 4)));
    typeInfoMap.insert(std::make_pair(TokenType::CONST, TokenTypeInfo("const", 5)));
    typeInfoMap.insert(std::make_pair(TokenType::CONTINUE, TokenTypeInfo("continue", 6)));
    typeInfoMap.insert(std::make_pair(TokenType::DEFAULT, TokenTypeInfo("default", 7)));
    typeInfoMap.insert(std::make_pair(TokenType::DO, TokenTypeInfo("do", 8)));
    typeInfoMap.insert(std::make_pair(TokenType::DOUBLE, TokenTypeInfo("double", 9)));
    typeInfoMap.insert(std::make_pair(TokenType::ELSE, TokenTypeInfo("else", 10)));
    typeInfoMap.insert(std::make_pair(TokenType::ENUM, TokenTypeInfo("enum", 11)));
    typeInfoMap.insert(std::make_pair(TokenType::EXTERN, TokenTypeInfo("extern", 12)));
    typeInfoMap.insert(std::make_pair(TokenType::FLOAT, TokenTypeInfo("float", 13)));
    typeInfoMap.insert(std::make_pair(TokenType::FOR, TokenTypeInfo("for", 14)));
    typeInfoMap.insert(std::make_pair(TokenType::GOTO, TokenTypeInfo("goto", 15)));
    typeInfoMap.insert(std::make_pair(TokenType::IF, TokenTypeInfo("if", 16)));
    typeInfoMap.insert(std::make_pair(TokenType::INT, TokenTypeInfo("int", 17)));
    typeInfoMap.insert(std::make_pair(TokenType::LONG, TokenTypeInfo("long", 18)));
    typeInfoMap.insert(std::make_pair(TokenType::REGISTER, TokenTypeInfo("register", 19)));
    typeInfoMap.insert(std::make_pair(TokenType::RETURN, TokenTypeInfo("return", 20)));
    typeInfoMap.insert(std::make_pair(TokenType::SHORT, TokenTypeInfo("short", 21)));
    typeInfoMap.insert(std::make_pair(TokenType::SIGNED, TokenTypeInfo("signed", 22)));
    typeInfoMap.insert(std::make_pair(TokenType::SIZEOF, TokenTypeInfo("sizeof", 23)));
    typeInfoMap.insert(std::make_pair(TokenType::STATIC, TokenTypeInfo("static", 24)));
    typeInfoMap.insert(std::make_pair(TokenType::STRUCT, TokenTypeInfo("struct", 25)));
    typeInfoMap.insert(std::make_pair(TokenType::SWITCH, TokenTypeInfo("switch", 26)));
    typeInfoMap.insert(std::make_pair(TokenType::TYPEDEF, TokenTypeInfo("typedef", 27)));
    typeInfoMap.insert(std::make_pair(TokenType::UNION, TokenTypeInfo("union", 28)));
    typeInfoMap.insert(std::make_pair(TokenType::UNSIGNED, TokenTypeInfo("unsigned", 29)));
    typeInfoMap.insert(std::make_pair(TokenType::VOID, TokenTypeInfo("void", 30)));
    typeInfoMap.insert(std::make_pair(TokenType::VOLATILE, TokenTypeInfo("volatile", 31)));
    typeInfoMap.insert(std::make_pair(TokenType::WHILE, TokenTypeInfo("while", 32)));
    typeInfoMap.insert(std::make_pair(TokenType::MINUS, TokenTypeInfo("-", 33)));
    typeInfoMap.insert(std::make_pair(TokenType::DOUBLE_MINUS, TokenTypeInfo("--", 34)));
    typeInfoMap.insert(std::make_pair(TokenType::MINUS_EQUAL, TokenTypeInfo("-=", 35)));
    typeInfoMap.insert(std::make_pair(TokenType::MINUS_GREATER, TokenTypeInfo("->", 36)));
    typeInfoMap.insert(std::make_pair(TokenType::EXCLAMATION_POINT, TokenTypeInfo("!", 37)));
    typeInfoMap.insert(std::make_pair(TokenType::EXCLAMATION_POINT_EQUAL, TokenTypeInfo("!=", 38)));
    typeInfoMap.insert(std::make_pair(TokenType::PERCENT, TokenTypeInfo("%", 39)));
    typeInfoMap.insert(std::make_pair(TokenType::PERCENT_EQUAL, TokenTypeInfo("%=", 40)));
    typeInfoMap.insert(std::make_pair(TokenType::AMPERSAND, TokenTypeInfo("&", 41)));
    typeInfoMap.insert(std::make_pair(TokenType::DOUBLE_AMPERSAND, TokenTypeInfo("&&", 42)));
    typeInfoMap.insert(std::make_pair(TokenType::AMPERSAND_EQUAL, TokenTypeInfo("&=", 43)));
    typeInfoMap.insert(std::make_pair(TokenType::OPEN_PAREN, TokenTypeInfo("(", 44)));
    typeInfoMap.insert(std::make_pair(TokenType::CLOSE_PAREN, TokenTypeInfo(")", 45)));
    typeInfoMap.insert(std::make_pair(TokenType::STAR, TokenTypeInfo("*", 46)));
    typeInfoMap.insert(std::make_pair(TokenType::STAR_EQUAL, TokenTypeInfo("*=", 47)));
    typeInfoMap.insert(std::make_pair(TokenType::COMMA, TokenTypeInfo(",", 48)));
    typeInfoMap.insert(std::make_pair(TokenType::POINT, TokenTypeInfo(".", 49)));
    typeInfoMap.insert(std::make_pair(TokenType::SLASH, TokenTypeInfo("/", 50)));
    typeInfoMap.insert(std::make_pair(TokenType::SLASH_EQUAL, TokenTypeInfo("/=", 51)));
    typeInfoMap.insert(std::make_pair(TokenType::COLON, TokenTypeInfo(":", 52)));
    typeInfoMap.insert(std::make_pair(TokenType::SEMICOLON, TokenTypeInfo(";", 53)));
    typeInfoMap.insert(std::make_pair(TokenType::QUESTION_MARK, TokenTypeInfo("?", 54)));
    typeInfoMap.insert(std::make_pair(TokenType::OPEN_BRACKET, TokenTypeInfo("[", 55)));
    typeInfoMap.insert(std::make_pair(TokenType::CLOSE_BRACKET, TokenTypeInfo("]", 56)));
    typeInfoMap.insert(std::make_pair(TokenType::CARET, TokenTypeInfo("^", 57)));
    typeInfoMap.insert(std::make_pair(TokenType::CARET_EQUAL, TokenTypeInfo("^=", 58)));
    typeInfoMap.insert(std::make_pair(TokenType::OPEN_BRACE, TokenTypeInfo("{", 59)));
    typeInfoMap.insert(std::make_pair(TokenType::VERTICAL_BAR, TokenTypeInfo("|", 60)));
    typeInfoMap.insert(std::make_pair(TokenType::DOUBLE_VERTICAL_BAR, TokenTypeInfo("||", 61)));
    typeInfoMap.insert(std::make_pair(TokenType::VERTICAL_BAR_EQUAL, TokenTypeInfo("|=", 62)));
    typeInfoMap.insert(std::make_pair(TokenType::CLOSE_BRACE, TokenTypeInfo("}", 63)));
    typeInfoMap.insert(std::make_pair(TokenType::TILDE, TokenTypeInfo("~", 64)));
    typeInfoMap.insert(std::make_pair(TokenType::PLUS, TokenTypeInfo("+", 65)));
    typeInfoMap.insert(std::make_pair(TokenType::PLUS_PLUS, TokenTypeInfo("++", 66)));
    typeInfoMap.insert(std::make_pair(TokenType::PLUS_EQUAL, TokenTypeInfo("+=", 67)));
    typeInfoMap.insert(std::make_pair(TokenType::LESS_THAN, TokenTypeInfo("<", 68)));
    typeInfoMap.insert(std::make_pair(TokenType::LEFT_SHIFT, TokenTypeInfo("<<", 69)));
    typeInfoMap.insert(std::make_pair(TokenType::LEFT_SHIFT_EQUAL, TokenTypeInfo("<<=", 70)));
    typeInfoMap.insert(std::make_pair(TokenType::LESS_EQUAL, TokenTypeInfo("<=", 71)));
    typeInfoMap.insert(std::make_pair(TokenType::EQUAL, TokenTypeInfo("=", 72)));
    typeInfoMap.insert(std::make_pair(TokenType::DOUBLE_EQUAL, TokenTypeInfo("==", 73)));
    typeInfoMap.insert(std::make_pair(TokenType::GREATER_THAN, TokenTypeInfo(">", 74)));
    typeInfoMap.insert(std::make_pair(TokenType::GREATER_EQUAL, TokenTypeInfo(">=", 75)));
    typeInfoMap.insert(std::make_pair(TokenType::RIGHT_SHIFT, TokenTypeInfo(">>", 76)));
    typeInfoMap.insert(std::make_pair(TokenType::RIGHT_SHIT_EQUAL, TokenTypeInfo(">>=", 77)));
    typeInfoMap.insert(std::make_pair(TokenType::DOUBLE_QUOTATION_MARKS, TokenTypeInfo("\"", 78)));
    typeInfoMap.insert(std::make_pair(TokenType::COMMENT, TokenTypeInfo("/*注释*/", 79)));
    typeInfoMap.insert(std::make_pair(TokenType::CONSTANT, TokenTypeInfo("常数", 80)));
}

#endif //CUSTOM_COMPILER_TOKENTYPE_H
