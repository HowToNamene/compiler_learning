//
// Created by zzl on 23-5-7.
//

#ifndef CUSTOM_COMPILER_SIMPLELEXER_H
#define CUSTOM_COMPILER_SIMPLELEXER_H

#include "token.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>

#define ID_MAX_LEN 32
#define PATCH_SIZE 1024*8
/**
* 有限状态机的各种状态。
*/
enum class DfaState {
    Initial,
    AUTO1, AUTO2, AUTO3, AUTO4,
    BREAK1, BREAK2, BREAK3, BREAK4, BREAK5,
    CASE1, CASE2, CASE3, CASE4,
    CHAR1, CHAR2, CHAR3, CHAR4,
    CONST1, CONST2, CONST3, CONST4, CONST5,
    CONTINUE1, CONTINUE2, CONTINUE3, CONTINUE4, CONTINUE5, CONTINUE6, CONTINUE7, CONTINUE8,
    DEFAULT1, DEFAULT2, DEFAULT3, DEFAULT4, DEFAULT5, DEFAULT6, DEFAULT7,
    DO1, DO2,
    DOUBLE1, DOUBLE2, DOUBLE3, DOUBLE4, DOUBLE5, DOUBLE6,
    ELSE1, ELSE2, ELSE3, ELSE4,
    ENUM1, ENUM2, ENUM3, ENUM4,
    EXTERN1, EXTERN2, EXTERN3, EXTERN4, EXTERN5, EXTERN6,
    FLOAT1, FLOAT2, FLOAT3, FLOAT4, FLOAT5,
    FOR1, FOR2, FOR3,
    GOTO1, GOTO2, GOTO3, GOTO4, GOTO5,
    IF1, IF2,
    INT1, INT2, INT3,
    LONG1, LONG2, LONG3, LONG4,
    REGISTER1, REGISTER2, REGISTER3, REGISTER4, REGISTER5, REGISTER6, REGISTER7, REGISTER8,
    RETURN1, RETURN2, RETURN3, RETURN4, RETURN5, RETURN6, RETURN7,
    SHORT1, SHORT2, SHORT3, SHORT4, SHORT5,
    SIGNED1, SIGNED2, SIGNED3, SIGNED4, SIGNED5, SIGNED6,
    SIZEOF1, SIZEOF2, SIZEOF3, SIZEOF4, SIZEOF5, SIZEOF6,
    STATIC1, STATIC2, STATIC3, STATIC4, STATIC5, STATIC6,
    STRUCT1, STRUCT2, STRUCT3, STRUCT4, STRUCT5, STRUCT6,
    SWITCH1, SWITCH2, SWITCH3, SWITCH4, SWITCH5, SWITCH6,
    TYPEDEF1, TYPEDEF2, TYPEDEF3, TYPEDEF4, TYPEDEF5, TYPEDEF6, TYPEDEF7,
    UNION1, UNION2, UNION3, UNION4, UNION5,
    UNSIGNED1, UNSIGNED2, UNSIGNED3, UNSIGNED4, UNSIGNED5, UNSIGNED6, UNSIGNED7, UNSIGNED8,
    VOID1, VOID2, VOID3, VOID4,
    VOLATILE1, VOLATILE2, VOLATILE3, VOLATILE4, VOLATILE5, VOLATILE6, VOLATILE7, VOLATILE8,
    WHILE1, WHILE2, WHILE3, WHILE4, WHILE5,

    MINUS,                  // - 33
    MINUS_MINUS,            // -- 34
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

class SimpleLexer {
private:
    std::string srcFile;


    class TokenText {
    public:
        char text[ID_MAX_LEN];
        int pos = 0;
        int len = 0;

        std::string toString() {
            std::string str;
            for (int i = 0; i < len; ++i) {
                str += text[i];
            }
            return str;
        }

        void append(char c) {
            text[pos] = c;
            pos++;
            len++;
        }

        int length() {
            return len;
        }

        void reset() {
            pos = 0;
            len = 0;
        }
    };

    //下面几个变量是在解析过程中用到的临时变量,如果要优化的话，可以塞到方法里隐藏起来
    TokenText tokenText;   //临时保存token的文本
    std::vector<Token *> tokens;       //保存解析出来的Token
    Token *token;        //当前正在解析的Token

    //是否是字母
    bool isAlpha(int ch) {
        return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z';
    }

    //是否是数字
    bool isDigit(int ch) {
        return ch >= '0' && ch <= '9';
    }

    //是否是空白字符
    bool isBlank(int ch) {
        return ch == ' ' || ch == '\t' || ch == '\n';
    }

    /**
     * 有限状态机进入初始状态。
     * 这个初始状态其实并不做停留，它马上进入其他状态。
     * 开始解析的时候，进入初始状态；某个Token解析完毕，也进入初始状态，在这里把Token记下来，然后建立一个新的Token。
     * @param ch
     * @return
     */
    DfaState initToken(char ch) {
        if (tokenText.length() > 0) {
            token->setTokenText(tokenText.toString());
            tokens.push_back(token);

            tokenText.reset();
            token = new Token();
        }

        DfaState newState = DfaState::Initial;
        if (isAlpha(ch)) {              //第一个字符是字母
            switch (ch) {
                case 'a':
                    newState = DfaState::AUTO1;
                    break;
                case 'b':
                    newState = DfaState::BREAK1;
                    break;
                case 'c':
                    newState = DfaState::CASE1;
                    break;
                case 'd':
                    newState = DfaState::DEFAULT1;
                    break;
                case 'e':
                    newState = DfaState::ELSE1;
                    break;
                case 'f':
                    newState = DfaState::FLOAT1;
                    break;
                case 'g':
                    newState = DfaState::GOTO1;
                    break;
                case 'i':
                    newState = DfaState::IF1;
                    break;
                case 'l':
                    newState = DfaState::LONG1;
                    break;
                case 'r':
                    newState = DfaState::REGISTER1;
                    break;
                case 's':
                    newState = DfaState::SHORT1;
                    break;
                case 't':
                    newState = DfaState::TYPEDEF1;
                    break;
                case 'u':
                    newState = DfaState::UNION1;
                    break;
                case 'v':
                    newState = DfaState::VOID1;
                    break;
                case 'w':
                    newState = DfaState::WHILE1;
                    break;
                default:
                    newState = DfaState::IDENTIFIER;
                    break;
            }
        } else if (ch == '-') {
            newState = DfaState::MINUS;
        } else if (ch == '!') {
            newState = DfaState::EXCLAMATION_POINT;
        } else if (ch == '%') {
            newState = DfaState::PERCENT;
        } else if (ch == '&') {
            newState = DfaState::AMPERSAND;
        } else if (ch == '(') {
            newState = DfaState::OPEN_PAREN;
        } else if (ch == ')') {
            newState = DfaState::CLOSE_PAREN;
        } else if (ch == '*') {
            newState = DfaState::STAR;
        } else if (ch == ',') {
            newState = DfaState::COMMA;
        } else if (ch == '.') {
            newState = DfaState::POINT;
        } else if (ch == '/') {
            newState = DfaState::SLASH;
        } else if (ch == ':') {
            newState = DfaState::COLON;
        } else if (ch == ';') {
            newState = DfaState::SEMICOLON;
        } else if (ch == '?') {
            newState = DfaState::QUESTION_MARK;
        } else if (ch == '[') {
            newState = DfaState::OPEN_BRACKET;
        } else if (ch == ']') {
            newState = DfaState::CLOSE_BRACKET;
        } else if (ch == '^') {
            newState = DfaState::CARET;
        } else if (ch == '{') {
            newState = DfaState::OPEN_BRACE;
        } else if (ch == '|') {
            newState = DfaState::VERTICAL_BAR;
        } else if (ch == '}') {
            newState = DfaState::CLOSE_BRACE;
        } else if (ch == '~') {
            newState = DfaState::TILDE;
        } else if (ch == '+') {
            newState = DfaState::PLUS;
        } else if (ch == '<') {
            newState = DfaState::LESS_THAN;
        } else if (ch == '=') {
            newState = DfaState::EQUAL;
        } else if (ch == '>') {
            newState = DfaState::GREATER_THAN;
        } else if (ch == '"') {
            newState = DfaState::DOUBLE_QUOTATION_MARKS;
        } else if (isDigit(ch)) {       //第一个字符是数字
            newState = DfaState::CONSTANT;
        } else {
            newState = DfaState::Initial; // skip all unknown patterns
        }
//        token->setTokenType(newState);
        if (newState != DfaState::Initial)
            tokenText.append(ch);
        return newState;
    }

    int it = 0;
    FILE *file = NULL;

    int readSrcFile(std::vector<char> &buffer) {
        fseek(file, it * PATCH_SIZE, SEEK_SET);
        size_t size = fread(buffer.data(), sizeof(char), PATCH_SIZE, file);
        if (size > 0) {
            it++;
            return size;
        }
        return -1;
    }

public:

    SimpleLexer(std::string _srcFile) {
        srcFile = _srcFile;
        file = fopen(srcFile.c_str(), "r");
        if (file == nullptr) {
            std::cout << srcFile << " open failed" << std::endl;
            exit(1);
        }
    }

/**
 * 解析字符串，形成Token。
 * 这是一个有限状态自动机，在不同的状态中迁移。
 * @param code
 * @return
 */
    void tokenize() {
        std::vector<char> codeBuffer(PATCH_SIZE);
        int cur_size = readSrcFile(codeBuffer);
        if (cur_size == -1) {
            return;
        }
        int ich = 0;
        char ch = 0;
        DfaState state = DfaState::Initial;
        for (int i = 0; i < cur_size; ++i) {
            ch = codeBuffer[i];
            switch (state) {
                case DfaState::Initial:
                    state = initToken(ch);          //重新确定后续状态
                    break;
                case DfaState::AUTO1:
                    if (ch == 'u') {
                        state = DfaState::AUTO2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::AUTO2:
                    if (ch == 't') {
                        state = DfaState::AUTO3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::AUTO3:
                    if (ch == 'o') {
                        state = DfaState::AUTO4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::AUTO4:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::AUTO);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::BREAK1:
                    if (ch == 'r') {
                        state = DfaState::BREAK2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::BREAK2:
                    if (ch == 'e') {
                        state = DfaState::BREAK3;
                        tokenText.append(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::BREAK3:
                    if (ch == 'a') {
                        state = DfaState::BREAK4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::BREAK4:
                    if (ch == 'k') {
                        state = DfaState::BREAK5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::BREAK5:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::BREAK);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::CASE1:
                    if (ch == 'a') {
                        state = DfaState::CASE2;
                    } else if (ch == 'h') {
                        state = DfaState::CHAR2;
                    } else if (ch == 'o') {
                        state = DfaState::CONST2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CASE2:
                    if (ch == 's') {
                        state = DfaState::CASE3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CASE3:
                    if (ch == 'e') {
                        state = DfaState::CASE4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CASE4:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::CASE);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::CHAR2:
                    if (ch == 'a') {
                        state = DfaState::CHAR3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CHAR3:
                    if (ch == 'r') {
                        state = DfaState::CHAR4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CHAR4:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::CHAR);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::CONST2:
                    if (ch == 'n') {
                        state = DfaState::CONST3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CONST3:
                    if (ch == 's') {
                        state = DfaState::CONST4;
                    } else if (ch == 't') {
                        state = DfaState::CONTINUE4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CONST4:
                    if (ch == 't') {
                        state = DfaState::CONST5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CONST5:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::CONST);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::CONTINUE4:
                    if (ch == 'i') {
                        state = DfaState::CONTINUE5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CONTINUE5:
                    if (ch == 'n') {
                        state = DfaState::CONTINUE6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CONTINUE6:
                    if (ch == 'u') {
                        state = DfaState::CONTINUE7;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CONTINUE7:
                    if (ch == 'e') {
                        state = DfaState::CONTINUE8;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::CONTINUE8:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::CONTINUE);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::DEFAULT1:
                    if (ch == 'e') {
                        state = DfaState::DEFAULT2;
                    } else if (ch == 'o') {
                        state = DfaState::DO2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::DEFAULT2:
                    if (ch == 'f') {
                        state = DfaState::DEFAULT3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::DEFAULT3:
                    if (ch == 'a') {
                        state = DfaState::DEFAULT4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::DEFAULT4:
                    if (ch == 'u') {
                        state = DfaState::DEFAULT5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::DEFAULT5:
                    if (ch == 'l') {
                        state = DfaState::DEFAULT6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::DEFAULT6:
                    if (ch == 't') {
                        state = DfaState::DEFAULT7;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::DEFAULT7:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::DEFAULT);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::DO2:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::DO);
                        state = initToken(ch);
                    } else if (ch == 'u') {
                        state = DfaState::DOUBLE3;
                        tokenText.append(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::DOUBLE3:
                    if (ch == 'b') {
                        state = DfaState::DOUBLE4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::DOUBLE4:
                    if (ch == 'l') {
                        state = DfaState::DOUBLE5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::DOUBLE5:
                    if (ch == 'e') {
                        state = DfaState::DOUBLE6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::DOUBLE6:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::DOUBLE);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::ELSE1:
                    if (ch == 'l') {
                        state = DfaState::ELSE2;
                    }
                    if (ch == 'n') {
                        state = DfaState::ENUM2;
                    }
                    if (ch == 'x') {
                        state = DfaState::EXTERN2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::ELSE2:
                    if (ch == 's') {
                        state = DfaState::ELSE3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::ELSE3:
                    if (ch == 'e') {
                        state = DfaState::ELSE4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::ELSE4:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::ELSE);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::ENUM2:
                    if (ch == 'u') {
                        state = DfaState::ENUM3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::ENUM3:
                    if (ch == 'm') {
                        state = DfaState::ENUM4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::ENUM4:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::ENUM);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::EXTERN2:
                    if (ch == 't') {
                        state = DfaState::EXTERN3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::EXTERN3:
                    if (ch == 'e') {
                        state = DfaState::EXTERN4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::EXTERN4:
                    if (ch == 'r') {
                        state = DfaState::EXTERN5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::EXTERN5:
                    if (ch == 'n') {
                        state = DfaState::EXTERN6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::EXTERN6:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::EXTERN);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::FLOAT1:
                    if (ch == 'l') {
                        state = DfaState::FLOAT2;
                    }
                    if (ch == 'o') {
                        state = DfaState::FOR2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::FLOAT2:
                    if (ch == 'o') {
                        state = DfaState::FLOAT3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::FLOAT3:
                    if (ch == 'a') {
                        state = DfaState::FLOAT4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::FLOAT4:
                    if (ch == 't') {
                        state = DfaState::FLOAT5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::FLOAT5:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::FLOAT);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::FOR2:
                    if (ch == 'r') {
                        state = DfaState::FOR3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::FOR3:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::FOR);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::GOTO1:
                    if (ch == 'o') {
                        state = DfaState::GOTO2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::GOTO2:
                    if (ch == 't') {
                        state = DfaState::GOTO3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::GOTO3:
                    if (ch == 'o') {
                        state = DfaState::GOTO4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::GOTO4:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::GOTO);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::IF1:
                    if (ch == 'f') {
                        state = DfaState::IF2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::IF2:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::IF);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::INT1:
                    if (ch == 'n') {
                        state = DfaState::INT2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::INT2:
                    if (ch == 't') {
                        state = DfaState::INT3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::INT3:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::INT);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::LONG1:
                    if (ch == 'o') {
                        state = DfaState::LONG2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::LONG2:
                    if (ch == 'n') {
                        state = DfaState::LONG3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::LONG3:
                    if (ch == 'g') {
                        state = DfaState::LONG4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::LONG4:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::LONG);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::REGISTER1:
                    if (ch == 'e') {
                        state = DfaState::REGISTER2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::REGISTER2:
                    if (ch == 'g') {
                        state = DfaState::REGISTER3;
                    }
                    if (ch == 't') {
                        state = DfaState::RETURN3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::REGISTER3:
                    if (ch == 'i') {
                        state = DfaState::REGISTER4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::REGISTER4:
                    if (ch == 's') {
                        state = DfaState::REGISTER5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::REGISTER5:
                    if (ch == 't') {
                        state = DfaState::REGISTER6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::REGISTER6:
                    if (ch == 'e') {
                        state = DfaState::REGISTER7;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::REGISTER7:
                    if (ch == 'r') {
                        state = DfaState::REGISTER8;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::REGISTER8:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::REGISTER);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::RETURN3:
                    if (ch == 'u') {
                        state = DfaState::RETURN4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::RETURN4:
                    if (ch == 'r') {
                        state = DfaState::RETURN5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::RETURN5:
                    if (ch == 'n') {
                        state = DfaState::RETURN6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SHORT1:
                    if (ch == 'h') {
                        state = DfaState::SHORT2;
                    }
                    if (ch == 'i') {
                        state = DfaState::SIGNED2;
                    }
                    if (ch == 't') {
                        state = DfaState::STATIC2;
                    }
                    if (ch == 'w') {
                        state = DfaState::SWITCH2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SHORT2:
                    if (ch == 'o') {
                        state = DfaState::SHORT3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SHORT3:
                    if (ch == 'r') {
                        state = DfaState::SHORT4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SHORT4:
                    if (ch == 't') {
                        state = DfaState::SHORT5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SHORT5:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::SHORT);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::SIGNED2:
                    if (ch == 'g') {
                        state = DfaState::SIGNED3;
                    }
                    if (ch == 'z') {
                        state = DfaState::SIZEOF3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SIGNED3:
                    if (ch == 'n') {
                        state = DfaState::SIGNED4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SIGNED4:
                    if (ch == 'e') {
                        state = DfaState::SIGNED5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SIGNED5:
                    if (ch == 'd') {
                        state = DfaState::SIGNED6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SIGNED6:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::SIGNED);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::SIZEOF3:
                    if (ch == 'e') {
                        state = DfaState::SIZEOF4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SIZEOF4:
                    if (ch == 'o') {
                        state = DfaState::SIZEOF5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SIZEOF5:
                    if (ch == 'f') {
                        state = DfaState::SIZEOF6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SIZEOF6:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::SIZEOF);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::STATIC2:
                    if (ch == 'a') {
                        state = DfaState::STATIC3;
                    }
                    if (ch == 'r') {
                        state = DfaState::STRUCT3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::STATIC3:
                    if (ch == 't') {
                        state = DfaState::STATIC4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::STATIC4:
                    if (ch == 'i') {
                        state = DfaState::STATIC5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::STATIC5:
                    if (ch == 'c') {
                        state = DfaState::STATIC6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::STATIC6:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::STATIC);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::STRUCT3:
                    if (ch == 'u') {
                        state = DfaState::STRUCT4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::STRUCT4:
                    if (ch == 'c') {
                        state = DfaState::STRUCT5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::STRUCT5:
                    if (ch == 't') {
                        state = DfaState::STRUCT6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::STRUCT6:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::STRUCT);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SWITCH2:
                    if (ch == 'i') {
                        state = DfaState::SWITCH3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SWITCH3:
                    if (ch == 't') {
                        state = DfaState::SWITCH4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SWITCH4:
                    if (ch == 'c') {
                        state = DfaState::SWITCH5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SWITCH5:
                    if (ch == 'h') {
                        state = DfaState::SWITCH6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::SWITCH6:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::SWITCH);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::TYPEDEF1:
                    if (ch == 'y') {
                        state = DfaState::TYPEDEF2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::TYPEDEF2:
                    if (ch == 'p') {
                        state = DfaState::TYPEDEF3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::TYPEDEF3:
                    if (ch == 'e') {
                        state = DfaState::TYPEDEF4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::TYPEDEF4:
                    if (ch == 'd') {
                        state = DfaState::TYPEDEF5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::TYPEDEF5:
                    if (ch == 'e') {
                        state = DfaState::TYPEDEF6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::TYPEDEF6:
                    if (ch == 'f') {
                        state = DfaState::TYPEDEF7;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::TYPEDEF7:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::TYPEDEF);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::UNION1:
                    if (ch == 'n') {
                        state = DfaState::UNION2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::UNION2:
                    if (ch == 'i') {
                        state = DfaState::UNION3;
                    }
                    if (ch == 's') {
                        state = DfaState::UNSIGNED3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::UNION3:
                    if (ch == 'o') {
                        state = DfaState::UNION4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::UNION4:
                    if (ch == 'n') {
                        state = DfaState::UNION5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::UNION5:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::UNION);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::UNSIGNED3:
                    if (ch == 'i') {
                        state = DfaState::UNSIGNED4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::UNSIGNED4:
                    if (ch == 'g') {
                        state = DfaState::UNSIGNED5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::UNSIGNED5:
                    if (ch == 'n') {
                        state = DfaState::UNSIGNED6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::UNSIGNED6:
                    if (ch == 'e') {
                        state = DfaState::UNSIGNED7;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::UNSIGNED7:
                    if (ch == 'd') {
                        state = DfaState::UNSIGNED8;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::UNSIGNED8:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::UNSIGNED);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::VOID1:
                    if (ch == 'o') {
                        state = DfaState::VOID2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::VOID2:
                    if (ch == 'i') {
                        state = DfaState::VOID3;
                    }
                    if (ch == 'l') {
                        state = DfaState::VOLATILE3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::VOID3:
                    if (ch == 'd') {
                        state = DfaState::VOID4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::VOID4:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::VOID);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::VOLATILE3:
                    if (ch == 'a') {
                        state = DfaState::VOLATILE4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::VOLATILE4:
                    if (ch == 't') {
                        state = DfaState::VOLATILE5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::VOLATILE5:
                    if (ch == 'i') {
                        state = DfaState::VOLATILE6;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::VOLATILE6:
                    if (ch == 'l') {
                        state = DfaState::VOLATILE7;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::VOLATILE7:
                    if (ch == 'e') {
                        state = DfaState::VOLATILE8;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::VOLATILE8:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::VOLATILE);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::WHILE1:
                    if (ch == 'h') {
                        state = DfaState::WHILE2;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::WHILE2:
                    if (ch == 'i') {
                        state = DfaState::WHILE3;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::WHILE3:
                    if (ch == 'l') {
                        state = DfaState::WHILE4;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::WHILE4:
                    if (ch == 'e') {
                        state = DfaState::WHILE5;
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                    }
                    tokenText.append(ch);
                    break;
                case DfaState::WHILE5:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::WHILE);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                case DfaState::MINUS:
                    if (isBlank(ch)) {
                        token->setTokenType(TokenType::MINUS);
                        state = initToken(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::IDENTIFIER;
                        tokenText.append(ch);
                    }
                    break;
                default:

            }

        }

        tokenize();
    }

    void dumpTokens() {
        for (Token *tmp_token: tokens) {
            tmp_token->dump();
        }
    }

};


/**
 * 一个简单的Token流。是把一个Token列表进行了封装。
 */
class SimpleTokenReader {
    std::vector<Token *> tokens;

    SimpleTokenReader() {};
};

#endif //CUSTOM_COMPILER_SIMPLELEXER_H
