#include "define.h"
//分行器
std::vector<std::string> branchBreaker(const std::string &sourceCode) {
    std::vector<std::string> eachLine;//用来存储每一行的内容
    {
        std::string lineSrc;
        for (UInt64 i = 0; i < sourceCode.size(); i++) {
            char tmp = sourceCode[i];
            char tmpNext = ((i < sourceCode.size()) ? sourceCode[i + 1] : '\0');
            if (tmp == '\r') {
                if (tmpNext == '\n')i++;
                eachLine.push_back(lineSrc);
                lineSrc.clear();
            } else if (tmp == '\n') {
                eachLine.push_back(lineSrc);
                lineSrc.clear();
            } else {
                lineSrc += tmp;
            }
        }
        eachLine.push_back(lineSrc);
    }
    return eachLine;
}
