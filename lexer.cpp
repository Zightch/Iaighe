#include "define.h"
//词法分析
std::vector<Token> lexer(const std::string& sourceCode) {
    enum NumberType {//数字状态类型
        n_noe,//空
        n_numberHeadZero,//数字打头是0
        n_BInteger,//2进制整数
        n_OInteger,//8进制整数
        n_DInteger,//10进制整数
        n_HInteger,//16进制整数
        n_floatStart,//浮点数开始
        n_float,//浮点数完成
        n_scienceNumber,//科学计数法
        n_scienceNumberE,//科学计数法E标志
        n_scienceNumberSymbol,//科学计数法符号标志
    };//数字状态类型
    enum StringType {//字符串状态类型
        str_noe,//空
        str_stringStart,//字符串开始
        str_stringSuspend,//字符串中断
    };//字符串状态类型
    enum NoteType {//注释状态类型
        note_noe,//空
        note_ready,//注释准备开始
        note_start,//单行注释中
        note_starts,//多行注释中
        note_readyEnd,//注释准备结束
    };//注释状态类型
    enum StateType {//状态类型
        s_stringError = -2,//字符串错误
        s_numberError,//数字错误
        s_noe = 0,//空
        s_identifier,//标识符
        s_symbol,//符号
        s_string,//字符串
        s_number,//数字
    };//状态类型
    enum LineType {//行状态类型
        l_noe,//空
        l_CR,//CR换行符
        l_LF,//LF换行符
    };//行状态类型
    std::vector<Token> tokenStream;//目标tokenStream
    UInt64 line = 1;//行号
    UInt64 lineOffset = 0;//行偏移
    StateType state = s_noe;//状态
    NumberType numState = n_noe;//数字状态
    StringType strState = str_noe;//字符串状态
    LineType lineType = l_noe;//行状态
    NoteType noteType = note_noe;//注释状态
    std::vector<Token> tokenSource;//临时分类结果
    bool rollback = false;//注释回退
    auto update = [&](Int8 i8) {//更新状态
        StateType stateLast = state;
        lineType = l_noe;

        //nse(numberStateExclusion)排除数字状态
        auto nse = [&](NumberType source, NumberType exclusion) -> bool {
            bool tmp = false;
            switch (exclusion) {
                case n_BInteger:
                    tmp |= ((source & n_OInteger) == n_OInteger);
                    tmp |= ((source & n_DInteger) == n_DInteger);
                    tmp |= ((source & n_HInteger) == n_HInteger);
                    tmp |= ((source & n_float) == n_float);
                    tmp |= ((source & n_scienceNumber) == n_scienceNumber);
                    break;
                case n_OInteger:
                    tmp |= ((source & n_BInteger) == n_BInteger);
                    tmp |= ((source & n_DInteger) == n_DInteger);
                    tmp |= ((source & n_HInteger) == n_HInteger);
                    tmp |= ((source & n_float) == n_float);
                    tmp |= ((source & n_scienceNumber) == n_scienceNumber);
                    break;
                case n_DInteger:
                    tmp |= ((source & n_BInteger) == n_BInteger);
                    tmp |= ((source & n_OInteger) == n_OInteger);
                    tmp |= ((source & n_HInteger) == n_HInteger);
                    tmp |= ((source & n_float) == n_float);
                    tmp |= ((source & n_scienceNumber) == n_scienceNumber);
                    break;
                case n_HInteger:
                    tmp |= ((source & n_BInteger) == n_BInteger);
                    tmp |= ((source & n_OInteger) == n_OInteger);
                    tmp |= ((source & n_DInteger) == n_DInteger);
                    tmp |= ((source & n_float) == n_float);
                    tmp |= ((source & n_scienceNumber) == n_scienceNumber);
                    break;
                case n_float:
                    tmp |= ((source & n_BInteger) == n_BInteger);
                    tmp |= ((source & n_OInteger) == n_OInteger);
                    tmp |= ((source & n_DInteger) == n_DInteger);
                    tmp |= ((source & n_HInteger) == n_HInteger);
                    tmp |= ((source & n_scienceNumber) == n_scienceNumber);
                    break;
                case n_scienceNumber:
                    tmp |= ((source & n_BInteger) == n_BInteger);
                    tmp |= ((source & n_OInteger) == n_OInteger);
                    tmp |= ((source & n_DInteger) == n_DInteger);
                    tmp |= ((source & n_HInteger) == n_HInteger);
                    tmp |= ((source & n_float) == n_float);
                    break;
                default:
                    break;
            }
            return tmp;
        };

        //空状态
        if (i8 == 0) {
            state = s_noe;//空(空格或者0字符)
            if (strState == str_stringStart || strState == str_stringSuspend) {
                state = s_stringError;
                strState = str_noe;
            }
            if (numState != n_noe) {
                numState = n_noe;
            }
        }

            //字符串状态更新
        else if (strState == str_stringStart) {
            if (i8 == '\r') {
                state = s_stringError;
                lineType = l_CR;
                strState = str_noe;
            } else if (i8 == '\n') {
                state = s_stringError;
                lineType = l_LF;
                strState = str_noe;
            } else if (i8 == '\\')strState = str_stringSuspend;
            else if (i8 == '"')strState = str_noe;
        } else if (strState == str_stringSuspend) {
            if (i8 == '\r') {
                state = s_stringError;
                lineType = l_CR;
                strState = str_noe;
            } else if (i8 == '\n') {
                state = s_stringError;
                lineType = l_LF;
                strState = str_noe;
            } else strState = str_stringStart;
        } else if (i8 == '"') {
            state = s_string;
            strState = str_stringStart;
        }

            //注释状态判定
        else if ((noteType == note_ready) && ((i8 == '/') || (i8 == '*'))) {
            state = s_noe;
            if (i8 == '/')
                noteType = note_start;
            else if (i8 == '*')
                noteType = note_starts;
        } else if (noteType == note_start) {
            if (i8 == '\r') {
                lineType = l_CR;//CR换行符
                state = s_noe;
                noteType = note_noe;
            } else if (i8 == '\n') {
                lineType = l_LF;//LF换行符
                state = s_noe;
                noteType = note_noe;
            }
        } else if ((noteType == note_starts) && (i8 == '*')) {
            noteType = note_readyEnd;
        } else if (noteType == note_readyEnd) {
            if (i8 == '/')
                noteType = note_noe;
            else
                noteType = note_starts;

            if (i8 == '\r') {
                lineType = l_CR;//CR换行符
                state = s_noe;
            } else if (i8 == '\n') {
                lineType = l_LF;//LF换行符
                state = s_noe;
            }
        } else if (i8 == '/') {
            state = s_symbol;
            noteType = note_ready;
        }

            //数字状态更新
        else if (stateLast == s_number) {
            if (i8 == '\r') {
                lineType = l_CR;//CR换行符
                state = s_noe;
            } else if (i8 == '\n') {
                lineType = l_LF;//LF换行符
                state = s_noe;
            } else if (numState == n_numberHeadZero) {//数字头为0
                if (i8 == 'b')//2进制判断
                    numState = n_BInteger;
                else if (('0' <= i8) && (i8 <= '7'))//8进制判断
                    numState = n_OInteger;
                else if (i8 == 'x')//16进制判断
                    numState = n_HInteger;
                else if (i8 == '.')//浮点数判断
                    numState = n_floatStart;
                else {
                    if (isASymbol(i8)) {
                        state = s_symbol;
                        numState = n_noe;
                    } else {
                        state = s_numberError;
                        numState = n_noe;
                    }
                }
            } else if (i8 == '.') {//浮点数判断
                if (nse(numState, n_DInteger) || ((numState & n_floatStart) == n_floatStart)) {
                    state = s_numberError;
                    numState = n_noe;
                } else numState = n_floatStart;
            } else {
                if (numState == n_BInteger) {//2进制整数合法检查
                    if (isASymbol(i8)) {
                        state = s_symbol;
                        numState = n_noe;
                    } else if ((i8 != '1') && (i8 != '0')) {
                        state = s_numberError;
                        numState = n_noe;
                    }
                } else if (numState == n_OInteger) {//8进制整数合法检查
                    if (isASymbol(i8)) {
                        state = s_symbol;
                        numState = n_noe;
                    } else if (!(('0' <= i8) && (i8 <= '7'))) {
                        state = s_numberError;
                        numState = n_noe;
                    }
                } else if (numState == n_HInteger) {//16进制整数合法检查
                    if (isASymbol(i8)) {
                        state = s_symbol;
                        numState = n_noe;
                    } else if (
                            !(
                                    (('0' <= i8) && (i8 <= '9')) ||
                                    (('a' <= i8) && (i8 <= 'f')) ||
                                    (('A' <= i8) && (i8 <= 'F'))
                            )
                            ) {
                        state = s_numberError;
                        numState = n_noe;
                    }
                } else if (numState == n_floatStart) {//浮点数合法判断
                    if (isASymbol(i8)) {
                        state = s_symbol;
                        numState = n_noe;
                    } else if (('0' <= i8) && (i8 <= '9'))numState = n_float;
                    else {
                        state = s_numberError;
                        numState = n_noe;
                    }
                } else if (i8 == 'e' || i8 == 'E') {
                    if (numState == n_DInteger || numState == n_float)numState = n_scienceNumberE;
                    else {
                        state = s_numberError;
                        numState = n_noe;
                    }
                } else if (numState == n_scienceNumberE) {
                    if (i8 == '+' || i8 == '-')numState = n_scienceNumberSymbol;
                    else {
                        state = s_numberError;
                        numState = n_noe;
                    }
                } else if (numState == n_scienceNumberSymbol) {
                    if (('0' <= i8) && (i8 <= '9'))numState = n_scienceNumber;
                    else {
                        state = s_numberError;
                        numState = n_noe;
                    }
                } else if (isASymbol(i8)) {
                    state = s_symbol;
                    numState = n_noe;
                } else if (
                        (numState != n_DInteger && numState != n_float && numState != n_scienceNumber) ||
                        (i8 < '0' || '9' < i8)
                        ) {
                    state = s_numberError;
                    numState = n_noe;
                }
            }
        } else if ((('0' <= i8) && (i8 <= '9')) && (stateLast != s_identifier) && (stateLast != s_numberError)) {
            if (i8 == '0')numState = n_numberHeadZero;
            else numState = n_DInteger;
            state = s_number;
        }

            //其他状态判定
        else if (i8 == ' ') {
            state = s_noe;
            if (numState != n_noe) {
                numState = n_noe;
            }
        } else if (isASymbol(i8))state = s_symbol;//符号
        else if (i8 == '\r') {
            lineType = l_CR;//CR换行符
            state = s_noe;
        } else if (i8 == '\n') {
            lineType = l_LF;//LF换行符
            state = s_noe;
        } else if (stateLast != s_numberError) {
            if (isACharNum(i8))state = s_identifier;//标识符
        }

        if (state == s_number) {
            if (numState == n_BInteger)tokenSource.back().tokenType = t_BInteger;
            if (numState == n_OInteger)tokenSource.back().tokenType = t_OInteger;
            if (numState == n_HInteger)tokenSource.back().tokenType = t_HInteger;
            if (numState == n_float)tokenSource.back().tokenType = t_float;
            if (numState == n_scienceNumber)tokenSource.back().tokenType = t_scienceNumber;
        }
        //临时分类结果更新
        if (state != stateLast) {
            if (state == s_numberError)tokenSource.back().tokenType = t_errorNumber;
            else if (state == s_stringError)tokenSource.back().tokenType = t_errorString;
            if (tokenSource.empty())tokenSource.emplace_back();
            if (!tokenSource.back().name.empty()) tokenSource.emplace_back();
            if ((s_identifier <= state) && (state <= s_number)) {
                tokenSource.back().tokenType = (TokenType) state;
                tokenSource.back().lineOffset = lineOffset + 1;
                tokenSource.back().line = line;
            }
        }
    };
    update(sourceCode[0]);//初始化状态
    for (UInt64 i = 0; i < sourceCode.size(); i++) {
        lineOffset++;//行偏移+1
        char tmp = sourceCode[i];
        char tmpNext = ((i < sourceCode.size()) ? sourceCode[i + 1] : '\0');
        char tmpNextNext = ((i < sourceCode.size() - 1) ? sourceCode[i + 2] : '\0');
        char tmpNextNextNext = ((i < sourceCode.size() - 2) ? sourceCode[i + 3] : '\0');
        //状态检测与更新
        if (lineType == l_CR) {
            line++;
            lineOffset = 0;
            update(tmpNext);
            if (lineType == l_LF) {
                i++;
                update(tmpNextNext);
            }
            rollback = false;
            continue;
        } else if (lineType == l_LF) {
            line++;
            lineOffset = 0;
            rollback = false;
        } else if (state == s_symbol) {
            tokenSource.back().name += tmp;
            if (tmpNext == '=') {
                if (
                        (tmp == '+') ||
                        (tmp == '-') ||
                        (tmp == '*') ||
                        (tmp == '/') ||
                        (tmp == '|') ||
                        (tmp == '&') ||
                        (tmp == '^') ||
                        (tmp == '%') ||
                        (tmp == '<') ||
                        (tmp == '>') ||
                        (tmp == '=') ||
                        (tmp == '!') ||
                        (tmp == '~')
                        ) {
                    lineOffset++;
                    i++;
                    tokenSource.back().name += tmpNext;
                    update(tmpNextNext);
                    continue;
                }
            } else if (tmp == '-' && tmpNext == '>') {
                lineOffset++;
                i++;
                tokenSource.back().name += tmpNext;
                update(tmpNextNext);
                continue;
            } else if (
                    (tmp == '+') ||
                    (tmp == '-') ||
                    (tmp == ':') ||
                    (tmp == '>') ||
                    (tmp == '<') ||
                    (tmp == '&') ||
                    (tmp == '|')
                    ) {
                if (tmpNext == tmp) {
                    if (tmpNextNext == '=') {
                        if (
                                (tmp == '>') ||
                                (tmp == '<') ||
                                (tmp == '&') ||
                                (tmp == '|')
                                ) {
                            lineOffset += 2;
                            i += 2;
                            tokenSource.back().name += tmpNext;
                            tokenSource.back().name += tmpNextNext;
                            update(tmpNextNextNext);
                            continue;
                        } else {
                            lineOffset++;
                            i++;
                            tokenSource.back().name += tmpNext;
                            update(tmpNextNext);
                            continue;
                        }
                    } else {
                        lineOffset++;
                        i++;
                        tokenSource.back().name += tmpNext;
                        update(tmpNextNext);
                        continue;
                    }
                }
            } else if (tmp == '.') {
                if (tmpNext == '.' && tmpNextNext == '.') {
                    lineOffset += 2;
                    i += 2;
                    tokenSource.back().name.append(2, '.');
                    update(tmpNextNextNext);
                    continue;
                }
            }
            tokenSource.emplace_back();
            if ((s_identifier <= state) && (state <= s_number)) {
                tokenSource.back().tokenType = (TokenType) state;
                tokenSource.back().lineOffset = lineOffset + 1;
                tokenSource.back().line = line;
            }
            rollback = false;
        } else if (noteType > note_ready) {
            if (!rollback) {
                tokenSource.pop_back();
                tokenSource.back().tokenType = t_noe;
                tokenSource.back().name.clear();
                tokenSource.back().line = 0;
                tokenSource.back().lineOffset = 0;
                rollback = true;
            }
        } else if (state == s_identifier || state == s_number || state == s_string) {
            tokenSource.back().name += tmp;
            rollback = false;
        }

        //根据下一个字符更新状态
        update(tmpNext);
    }
    if (state == s_stringError)
        tokenSource.back().tokenType = t_errorString;
    for (const auto &i: tokenSource)
        if ((i.tokenType != t_noe) && (!i.name.empty()))
            tokenStream.push_back(i);
    return tokenStream;
}
