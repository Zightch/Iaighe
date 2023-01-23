#pragma once
#include<string>
#include<vector>
typedef unsigned long long UInt64;
typedef unsigned int UInt32;
typedef unsigned short UInt16;
typedef unsigned char UInt8;
typedef long long Int64;
typedef int Int32;
typedef short Int16;
typedef char Int8;
auto Class_Function(const std::string &) -> std::string;
#define CLASS_FUNCTION Class_Function(__PRETTY_FUNCTION__)

bool isInteger(const std::string &);

bool isFloating(const std::string &);

bool isNumber(const std::string &);

bool suffix(const std::string &, const std::string &);

bool prefix(const std::string &, const std::string &);

bool isASymbol(Int8);

bool isACharNum(Int8);

std::string UInt64ToString(UInt64);

//词类型
enum TokenType {
    t_errorNumber = -2,//数字错误(存在非法数字)
    t_errorString,//字符串错误(有头无尾)
    t_noe = 0,//空
    t_identifier,//标识符
    t_symbol,//符号
    t_string,//字符串
    t_DInteger,//10进制整数
    t_BInteger,//2进制整数
    t_OInteger,//8进制整数
    t_HInteger,//16进制整数
    t_float,//浮点数
    t_scienceNumber,//科学计数法
};

std::string toString(TokenType);

//词
struct Token {
    TokenType tokenType = t_noe;//类型
    std::string name;//名称
    UInt64 line = 0;//行号
    UInt64 lineOffset = 0;//行偏移
};

//词法分析
std::vector<Token> lexer(const std::string &);

//语法分析
std::vector<std::string> parser(const std::vector<Token> &, const std::vector<std::string> &);

//分行器
std::vector<std::string> branchBreaker(const std::string &);

//报错
void error(
        const std::string &,//文件名字
        const Token &,//Token
        const std::string &,//报错内容文本
        const std::string &,//Token所在行内容
        UInt64,//标记开始
        UInt64,//标记长度
        UInt64 = 0//索引位置
);
