#include "define.h"
auto Class_Function(std::string &prettyFun) -> std::string {
    unsigned int i = 0;
    while (prettyFun[i++] != '(');
    i -= 2;
    unsigned int j = i;
    while (j != 0)
        if (prettyFun[j--] == ' ')break;
    j == 0 ?: j += 2;
    std::string tmp;
    for (unsigned int k = j; k <= i; k++) {
        tmp.push_back(prettyFun[k]);
    }
    return tmp;
}

bool suffix(const std::string &src, const std::string &s) {
    bool tmp = false;
    if (src.size() <= s.size())return false;
    for (UInt64 i = src.size() - 1; i >= src.size() - s.size(); i--)
        tmp |= src[i] != s[i - src.size() + s.size()];
    return !tmp;
}
bool prefix(const std::string &src, const std::string &s) {
    bool tmp = false;
    if (src.size() <= s.size())return false;
    for (UInt64 i = 0; i < s.size(); i++)
        tmp |= src[i] != s[i];
    return !tmp;
}

bool isInteger(const std::string &num) {
    UInt64 len = 0;
    while (num[len++]);
    len--;
    if ((len == 1) && (('0' <= num[0]) && (num[0] <= '9')))
        return true;
    if (('1' <= num[0]) && (num[0] <= '9')) {
        UInt8 end = 0;
        if (suffix(num, "ull"))end = 3;
        else if (suffix(num, "ul") || suffix(num, "ll"))end = 2;
        else if (suffix(num, "l") || suffix(num, "u"))end = 1;
        for (UInt64 i = 1; i < len - end; i++) {
            if (!(('0' <= num[i]) && (num[i] <= '9')))return false;
        }
    } else return false;
    return true;
}
bool isFloating(const std::string &num) {
    UInt64 len = 0;
    while (num[len++]);
    len--;
    if ((len == 1) && (('0' <= num[0]) && (num[0] <= '9')))
        return true;
    bool decimalPoint = false;
    UInt64 decimalIndex = 0;
    for (UInt64 i = 1; i < len; i++) {
        if (num[i] == '.') {
            if (!decimalPoint) {
                decimalPoint = true;
                decimalIndex = i;
            } else return false;
        }
    }
    std::string integer;
    integer.append(num, decimalIndex == 0 ? len : decimalIndex);
    if (isInteger(integer)) {
        UInt8 end = 0;
        if (suffix(num, "l") || suffix(num, "f"))end = 1;
        for (UInt64 i = decimalIndex + 1; i < len - end; i++) {
            if (!(('0' <= num[i]) && (num[i] <= '9')))return false;
        }
    } else
        return false;
    return true;
}
bool isNumber(const std::string& num) {
    UInt64 len = 0;
    while (num[len++]);
    len--;
    bool tmp = isInteger(num) || isFloating(num);
    if (!tmp) {
        UInt8 end = 0;
        if (suffix(num, "ull"))end = 3;
        else if (suffix(num, "ul") || suffix(num, "ll"))end = 2;
        else if (suffix(num, "l") || suffix(num, "u"))end = 1;
        if (prefix(num, "0x")) {
            for (UInt64 i = 2; i < len - end; i++) {
                if (
                        (!(('0' <= num[i]) && (num[i] <= '9'))) &&
                        (!(('a' <= num[i]) && (num[i] <= 'f'))) &&
                        (!(('A' <= num[i]) && (num[i] <= 'Z')))
                        )
                    return false;
            }
            return true;
        } else if (prefix(num, "0b")) {
            for (UInt64 i = 2; i < len - end; i++) {
                if ((!((num[i] == '0') || (num[i] == '1'))))
                    return false;
            }
            return true;
        } else if (prefix(num, "0")) {
            for (UInt64 i = 1; i < len - end; i++) {
                if (!(('0' <= num[i]) && (num[i] <= '7')))return false;
            }
            return true;
        }
    } else return true;
    return false;
}
bool isASymbol(char c) {
    return (
            (
                    ((33 <= c) && (c <= 47)) ||
                    ((58 <= c) && (c <= 64)) ||
                    ((91 <= c) && (c <= 96)) ||
                    ((123 <= c) && (c <= 126))
            ) && (c != '_')
    );
}

std::string UInt64ToString(UInt64 num) {
    std::string tmp;
    if (num == 0)tmp = "0";
    while (num != 0) {
        tmp += (char) ((num % 10) + 48);
        num /= 10;
    }
    std::string ans;
    for (UInt64 i = tmp.size() - 1; i > 0; i--)
        ans += tmp[i];
    ans += tmp[0];
    return ans;
}
std::string toString(TokenType tp) {
    if (tp == t_identifier) {
        return "标识符";
    }
    if (tp == t_BInteger) {
        return "2进制整数";
    }
    if (tp == t_OInteger) {
        return "8进制整数";
    }
    if (tp == t_DInteger) {
        return "10进制整数";
    }
    if (tp == t_HInteger) {
        return "16进制整数";
    }
    if (tp == t_float) {
        return "浮点数";
    }
    if (tp == t_scienceNumber) {
        return "科学计数法";
    }
    if (tp == t_symbol) {
        return "符号";
    }
    if (tp == t_string) {
        return "字符串";
    }
    if (tp == t_noe) {
        return "空";
    }
    if (tp == t_errorString) {
        return "错误的字符串";
    }
    if (tp == t_errorNumber) {
        return "错误的数字";
    }
    return "未知类型";
}
bool isACharNum(Int8 c) {
    return (isalnum(c) || (c == '_') || ((-128 <= c) && (c <= -1)));
}
