#include "define.h"
#include<iostream>
void error(
        const std::string& fileName,//文件名字
        const Token& token,//Token
        const std::string& text,//报错内容文本
        const std::string& line,//Token所在行内容
        UInt64 signStart,//标记开始
        UInt64 signSize,//标记长度
        UInt64 lineOffset//索引位置
) {
    std::cout << std::endl;
    std::cout << fileName << ":" << token.line << ":" << ((lineOffset == 0) ? token.lineOffset : lineOffset) << ": ";
    std::cout << "error: " << token.name << " " << text << std::endl;
    std::cout << "    " << token.line << " | ";
    std::cout << line << std::endl;
    for (UInt64 j = 1; j <= 4 + UInt64ToString(token.line).size(); j++)std::cout << " ";
    std::cout << " | ";
    if (signStart != 0) {
        for (UInt64 j = 1; j < signStart; j++) std::cout << " ";
        std::cout << "^";
    }
    if (signSize != 0) {
        for (UInt64 j = 1; j < signSize; j++) std::cout << "~";
        std::cout << std::endl << std::endl;
    }
}
