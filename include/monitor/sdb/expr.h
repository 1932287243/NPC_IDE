#ifndef __EXPR_H__
#define __EXPR_H__

#include <regex>
#include <vector>
#include <string>

// #include "common.h"
#include <iostream>

#define __FOR_TEST__

class EXPR
{
public:
    enum TokenType {
        TK_NOTYPE = 256, TK_ADD, TK_SUB, TK_MULT, TK_DIV, TK_EQ, TK_NEQ, TK_AND, TK_OR,
        TK_DEREF, TK_INTEGER, TK_HEX, TK_REG, TK_L_BRACKET, TK_R_BRACKET
    };

    struct Token {
        int type;
        std::string str;
    };

    EXPR();
    ~EXPR();
    void initExpr();
    uint32_t evaluate(const std::string &expression, bool &success);

private:
    struct Rule {
        std::string regex;
        int tokenType;
    };

    std::vector<Rule> rules;    // 
    std::vector<std::regex> regexPatterns;   // 正则表达式对象
    std::vector<Token> tokens;
    int parenthesesNum = 0;
    bool sub_flag = false;

    void initRules();
    // 初始化正则表达式， 用于匹配输入表达式中的不同类型的符号和数值。
    void initRegex();
    // 词法分析：匹配表达式中的数字和符号
    bool tokenize(const std::string &expression);
    // 处理得到token
    bool handleToken(Token &token);
    // 递归寻找表达式里面的括号
    void recursionCheck(int p, int q);
    // 检查在tokens中p和q索引处的左右括号是否一对
    bool checkParentheses(int p, int q);
    // 在寻找主操作符号的时候找到优先级最低的操作符号
    int getPrecedence(int type);
    // 找到主操作符号
    int findMainOperator(int p, int q);
    uint32_t eval(int p, int q);
};
#endif