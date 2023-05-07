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

class SimpleLexer {
private:
    std::string srcFile;
    /**
     * 有限状态机的各种状态。
     */
    enum class DfaState {
        Initial,

        If, Id_if1, Id_if2, Else, Id_else1, Id_else2, Id_else3, Id_else4, Int, Id_int1, Id_int2, Id_int3, Id, GT, GE,

        Assignment,

        Plus, Minus, Star, Slash,

        SemiColon,
        LeftParen,
        RightParen,

        IntLiteral
    };

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
            if (ch == 'i') {
                newState = DfaState::Id_int1;
            } else {
                newState = DfaState::Id; //进入Id状态
            }
            token->setTokenType(Identifier);
            tokenText.append(ch);
        } else if (isDigit(ch)) {       //第一个字符是数字
            newState = DfaState::IntLiteral;
            token->setTokenType(IntLiteral);
            tokenText.append(ch);
        } else if (ch == '>') {         //第一个字符是>
            newState = DfaState::GT;
            token->setTokenType(GT);
            tokenText.append(ch);
        } else if (ch == '+') {
            newState = DfaState::Plus;
            token->setTokenType(Plus);
            tokenText.append(ch);
        } else if (ch == '-') {
            newState = DfaState::Minus;
            token->setTokenType(Minus);
            tokenText.append(ch);
        } else if (ch == '*') {
            newState = DfaState::Star;
            token->setTokenType(Star);
            tokenText.append(ch);
        } else if (ch == '/') {
            newState = DfaState::Slash;
            token->setTokenType(Slash);
            tokenText.append(ch);
        } else if (ch == ';') {
            newState = DfaState::SemiColon;
            token->setTokenType(SemiColon);
            tokenText.append(ch);
        } else if (ch == '(') {
            newState = DfaState::LeftParen;
            token->setTokenType(LeftParen);
            tokenText.append(ch);
        } else if (ch == ')') {
            newState = DfaState::RightParen;
            token->setTokenType(RightParen);
            tokenText.append(ch);
        } else if (ch == '=') {
            newState = DfaState::Assignment;
            token->setTokenType(Assignment);
            tokenText.append(ch);
        } else {
            newState = DfaState::Initial; // skip all unknown patterns
        }
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
                case DfaState::Id:
                    if (isAlpha(ch) || isDigit(ch)) {
                        tokenText.append(ch);       //保持标识符状态
                    } else {
                        state = initToken(ch);      //退出标识符状态，并保存Token
                    }
                    break;
                case DfaState::GT:
                    if (ch == '=') {
                        token->setTokenType(GE);  //转换成GE
                        state = DfaState::GE;
                        tokenText.append(ch);
                    } else {
                        state = initToken(ch);      //退出GT状态，并保存Token
                    }
                    break;
                case DfaState::GE:
                case DfaState::Assignment:
                case DfaState::Plus:
                case DfaState::Minus:
                case DfaState::Star:
                case DfaState::Slash:
                case DfaState::SemiColon:
                case DfaState::LeftParen:
                case DfaState::RightParen:
                    state = initToken(ch);          //退出当前状态，并保存Token
                    break;
                case DfaState::IntLiteral:
                    if (isDigit(ch)) {
                        tokenText.append(ch);       //继续保持在数字字面量状态
                    } else {
                        state = initToken(ch);      //退出当前状态，并保存Token
                    }
                    break;
                case DfaState::Id_int1:
                    if (ch == 'n') {
                        state = DfaState::Id_int2;
                        tokenText.append(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::Id;    //切换回Id状态
                        tokenText.append(ch);
                    } else {
                        state = initToken(ch);
                    }
                    break;
                case DfaState::Id_int2:
                    if (ch == 't') {
                        state = DfaState::Id_int3;
                        tokenText.append(ch);
                    } else if (isDigit(ch) || isAlpha(ch)) {
                        state = DfaState::Id;    //切换回id状态
                        tokenText.append(ch);
                    } else {
                        state = initToken(ch);
                    }
                    break;
                case DfaState::Id_int3:
                    if (isBlank(ch)) {
                        token->setTokenType(Int);
                        state = initToken(ch);
                    } else {
                        state = DfaState::Id;    //切换回Id状态
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
