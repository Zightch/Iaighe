#include<iostream>
#include"define.h"
#include"readFile.h"
int main(int argc, char *argv[]) {
    if (argc >= 2) {
        UInt64 fileSize = 0;
        char *fileData = nullptr;
        readFile(argv[1], fileSize, fileData);//读取文件
        std::string sourceCode = fileData == nullptr ? "" : fileData;//设置源码
        free(fileData);//释放空间
        std::vector<std::string> el = branchBreaker(sourceCode);//分行
        std::vector<Token> tokenStream = lexer(sourceCode);//词法分析
        UInt64 lineLast = 0;
        if(!tokenStream.empty())
            lineLast = tokenStream[0].line;
        for (auto &i: tokenStream) {//枚举Token
            if (!i.tokenError.empty()) {
                for(auto &j : i.tokenError) {
                    if (j.tokenErrorType == te_errorString) {
                        std::string tmp = (el[i.line - 1] + "_");
                        error(argv[1], i, "缺少'\"'", tmp, j.lineOffset, j.len, j.lineOffset);
                    }

                    if (j.tokenErrorType == te_errorChar) {
                        std::string tmp = (el[i.line - 1] + "_");
                        error(argv[1], i, "缺少'\''", tmp, j.lineOffset, j.len, j.lineOffset);
                    }
                    if (j.tokenErrorType == te_errorNumber) {
                        std::string tmp = (el[i.line - 1]);
                        error(argv[1], i, "数字错误", tmp, j.lineOffset, j.len, j.lineOffset);
                    }
                }
            } else {
                if (argc == 3) {
                    if (std::string(argv[2]) == "token") {
                        if (lineLast != i.line) {
                            std::cout << std::endl;
                            lineLast = i.line;
                        }
                        std::cout << i.name << " ";
                    }
                    if (std::string(argv[2]) == "detail") {
                        std::cout << i.name << " ";
                        std::cout << "类型:" << toString(i.tokenType) << " ";
                        std::cout << "行号:" << i.line << " ";
                        std::cout << "行偏移:" << i.lineOffset << std::endl;
                    }
                }
            }
        }
        std::cout << std::endl << "分析完成" << std::endl;
    }
}
