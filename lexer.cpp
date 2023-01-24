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
        str_stringEscapeStart,//字符串转义开始
    };//字符串状态类型
    enum CharType {//字符串状态类型
        c_noe,//空
        c_charStart,//字符开始
        c_charEscapeStart,//字符串转义开始
    };//字符串状态类型
    enum NoteType {//注释状态类型
        note_noe,//空
        note_ready,//注释准备开始
        note_start,//单行注释中
        note_starts,//多行注释中
        note_readyEnd,//注释准备结束
    };//注释状态类型
    enum StateType {//状态类型
        s_noe = 0,//空
        s_identifier,//标识符
        s_symbol,//符号
        s_char,//字符串
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
    CharType charState = c_noe;//字符状态
    LineType lineState = l_noe;//行状态
    NoteType noteState = note_noe;//注释状态
    TokenErrorType tokenErrorType = te_noe;//错误类型
    std::vector<Token> tokenSource;//临时分类结果
    auto update = [&](Int8 i8) {//更新状态
        StateType stateLast = state;
        lineState = l_noe;

        //空状态
        if (i8 == 0) {
            state = s_noe;//空(空格或者0字符)
            if (strState >= str_stringStart) {
                tokenErrorType = te_errorString;
                strState = str_noe;
            }
            if (charState >= c_charStart) {
                tokenErrorType = te_errorChar;
                charState = c_noe;
            }
            if (numState != n_noe) {
                numState = n_noe;
            }
        }

            //字符串状态更新
        else if (strState == str_stringStart) {
            if (i8 == '\r') {
                tokenErrorType = te_errorString;
                lineState = l_CR;
                strState = str_noe;
            } else if (i8 == '\n') {
                tokenErrorType = te_errorString;
                lineState = l_LF;
                strState = str_noe;
            } else if (i8 == '\\')strState = str_stringEscapeStart;
            else if (i8 == '"') {
                state = s_noe;
                strState = str_noe;
            }
        } else if (strState == str_stringEscapeStart) {
            if (i8 == '\r') {
                tokenErrorType = te_errorString;
                lineState = l_CR;
                strState = str_noe;
            } else if (i8 == '\n') {
                tokenErrorType = te_errorString;
                lineState = l_LF;
                strState = str_noe;
            } else {
                strState = str_stringStart;
            }
        } else if (i8 == '"' && noteState == note_noe && charState == c_noe) {
            state = s_string;
            strState = str_stringStart;
        }

            //字符状态更新
        else if (charState == c_charStart) {
            if (i8 == '\r') {
                tokenErrorType = te_errorChar;
                lineState = l_CR;
                charState = c_noe;
            } else if (i8 == '\n') {
                tokenErrorType = te_errorChar;
                lineState = l_LF;
                charState = c_noe;
            } else if (i8 == '\\')charState = c_charEscapeStart;
            else if (i8 == '\'') {
                state = s_noe;
                charState = c_noe;
            }
        } else if (charState == c_charEscapeStart) {
            if (i8 == '\r') {
                tokenErrorType = te_errorString;
                lineState = l_CR;
                charState = c_noe;
            } else if (i8 == '\n') {
                tokenErrorType = te_errorString;
                lineState = l_LF;
                charState = c_noe;
            } else {
                charState = c_charStart;
            }
        } else if (i8 == '\'' && noteState == note_noe && strState == str_noe) {
            state = s_char;
            charState = c_charStart;
        }

            //注释状态判定
        else if ((noteState == note_ready) && ((i8 == '/') || (i8 == '*'))) {
            state = s_noe;
            if (i8 == '/')
                noteState = note_start;
            else
                noteState = note_starts;
        } else if (noteState == note_start) {
            if (i8 == '\r') {
                lineState = l_CR;//CR换行符
                state = s_noe;
                noteState = note_noe;
            } else if (i8 == '\n') {
                lineState = l_LF;//LF换行符
                state = s_noe;
                noteState = note_noe;
            }
        } else if (noteState == note_starts) {
            if (i8 == '*') {
                noteState = note_readyEnd;
            }
        } else if (noteState == note_readyEnd) {
            if (i8 == '/')
                noteState = note_noe;
            else
                noteState = note_starts;

            if (i8 == '\r') {
                lineState = l_CR;//CR换行符
                state = s_noe;
            } else if (i8 == '\n') {
                lineState = l_LF;//LF换行符
                state = s_noe;
            }
        } else if (i8 == '/' && strState == str_noe && charState == c_noe) {
            state = s_symbol;
            noteState = note_ready;
        }

            //数字状态更新
        else if (stateLast == s_number) {
            if (i8 == '\r') {
                lineState = l_CR;//CR换行符
                state = s_noe;
            } else if (i8 == '\n') {
                lineState = l_LF;//LF换行符
                state = s_noe;
            } else if (i8 == ' ' || i8 == '\t') {
                state = s_noe;
                numState = n_noe;
            } else if (numState == n_numberHeadZero) {//数字头为0
                if (i8 == 'b' || i8 == 'B')//2进制判断
                    numState = n_BInteger;
                else if (('0' <= i8) && (i8 <= '7'))//8进制判断
                    numState = n_OInteger;
                else if (i8 == 'x' || i8 == 'X')//16进制判断
                    numState = n_HInteger;
                else if (i8 == '.')//浮点数判断
                    numState = n_floatStart;
                else {
                    if (isASymbol(i8)) {
                        state = s_symbol;
                        numState = n_noe;
                    } else {
                        tokenErrorType = te_errorNumber;
                        numState = n_noe;
                    }
                }
            } else if (i8 == '.') {//浮点数判断
                if (numState == n_DInteger)
                    numState = n_floatStart;
                else {
                    tokenErrorType = te_errorNumber;
                    numState = n_noe;
                }
            } else {
                if (numState == n_BInteger) {//2进制整数合法检查
                    if (isASymbol(i8)) {
                        state = s_symbol;
                        numState = n_noe;
                    } else if ((i8 != '1') && (i8 != '0')) {
                        tokenErrorType = te_errorNumber;
                        numState = n_noe;
                    }
                } else if (numState == n_OInteger) {//8进制整数合法检查
                    if (isASymbol(i8)) {
                        state = s_symbol;
                        numState = n_noe;
                    } else if (!('0' <= i8 && i8 <= '7')) {
                        tokenErrorType = te_errorNumber;
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
                        tokenErrorType = te_errorNumber;
                        numState = n_noe;
                    }
                } else if (numState == n_floatStart) {//浮点数合法判断
                    if (isASymbol(i8)) {
                        state = s_symbol;
                        numState = n_noe;
                    } else if (('0' <= i8) && (i8 <= '9'))numState = n_float;
                    else {
                        tokenErrorType = te_errorNumber;
                        numState = n_noe;
                    }
                } else if (i8 == 'e' || i8 == 'E') {
                    if (numState == n_DInteger || numState == n_float)numState = n_scienceNumberE;
                    else {
                        tokenErrorType = te_errorNumber;
                        numState = n_noe;
                    }
                } else if (numState == n_scienceNumberE) {
                    if (i8 == '+' || i8 == '-')numState = n_scienceNumberSymbol;
                    else if ('0' <= i8 && i8 <= '9')numState = n_scienceNumber;
                    else {
                        tokenErrorType = te_errorNumber;
                        numState = n_noe;
                    }
                } else if (numState == n_scienceNumberSymbol) {
                    if (('0' <= i8) && (i8 <= '9'))numState = n_scienceNumber;
                    else {
                        tokenErrorType = te_errorNumber;
                        numState = n_noe;
                    }
                } else if (isASymbol(i8)) {
                    state = s_symbol;
                    numState = n_noe;
                } else if (
                        (
                                numState != n_DInteger &&
                                numState != n_float &&
                                numState != n_scienceNumber
                        ) ||
                        (i8 < '0' || '9' < i8)
                        ) {
                    tokenErrorType = te_errorNumber;
                    numState = n_noe;
                }
            }
        } else if (
                (('0' <= i8) && (i8 <= '9')) &&
                (stateLast != s_identifier) &&
                (tokenErrorType != te_errorNumber)
                ) {
            if (i8 == '0')numState = n_numberHeadZero;
            else numState = n_DInteger;
            state = s_number;
        }

            //其他状态判定
        else if (i8 == ' ' || i8 == '\t') {
            state = s_noe;
            if (numState != n_noe) {
                numState = n_noe;
            }
        } else if (isASymbol(i8))state = s_symbol;//符号
        else if (i8 == '\r') {
            lineState = l_CR;//CR换行符
            state = s_noe;
        } else if (i8 == '\n') {
            lineState = l_LF;//LF换行符
            state = s_noe;
        } else if (tokenErrorType != te_errorNumber) {
            if (isACharNum(i8))state = s_identifier;//标识符
        }

        if (state == s_number) {
            if (numState == n_BInteger)tokenSource.back().tokenType = tt_BInteger;
            if (numState == n_OInteger)tokenSource.back().tokenType = tt_OInteger;
            if (numState == n_HInteger)tokenSource.back().tokenType = tt_HInteger;
            if (numState == n_float)tokenSource.back().tokenType = tt_float;
            if (numState == n_scienceNumber)tokenSource.back().tokenType = tt_scienceNumber;
        }
        switch (tokenErrorType) {
            case te_errorNumber: {
                Token &token = tokenSource.back();
                UInt64 offset = token.lineOffset;
                UInt64 len = token.name.size() + 1;
                token.tokenError.clear();
                token.tokenError.push_back({te_errorNumber, offset, len});
                tokenErrorType = te_noe;
            }
                break;
            case te_errorString: {
                Token &token = tokenSource.back();
                UInt64 offset = token.lineOffset + token.name.size();
                token.tokenError.push_back({te_errorString, offset, 1});
                tokenErrorType = te_noe;
            }
                break;
            case te_errorChar: {
                Token &token = tokenSource.back();
                UInt64 offset = token.lineOffset + token.name.size();
                token.tokenError.push_back({te_errorChar, offset, 1});
                tokenErrorType = te_noe;
            }
                break;
            default:
                break;
        }
        //临时分类结果更新
        if (state != stateLast) {
            if (stateLast == s_string)tokenSource.back().name += '"';
            if (stateLast == s_char)tokenSource.back().name += '\'';
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
        if (lineState == l_CR) {
            line++;
            lineOffset = 0;
            update(tmpNext);
            if (lineState == l_LF) {
                i++;
                update(tmpNextNext);
            }
            continue;
        } else if (lineState == l_LF) {
            line++;
            lineOffset = 0;
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
                    if (tmp == '<' && tmpNextNext == '>') {
                        lineOffset += 2;
                        i += 2;
                        tokenSource.back().name += tmpNext;
                        tokenSource.back().name += tmpNextNext;
                        state = s_noe;
                        update(tmpNextNextNext);
                        continue;
                    } else {
                        lineOffset++;
                        i++;
                        tokenSource.back().name += tmpNext;
                        state = s_noe;
                        update(tmpNextNext);
                        continue;
                    }
                }
            } else if (tmp == '-' && tmpNext == '>') {
                lineOffset++;
                i++;
                tokenSource.back().name += tmpNext;
                state = s_noe;
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
                            state = s_noe;
                            update(tmpNextNextNext);
                            continue;
                        } else {
                            lineOffset++;
                            i++;
                            tokenSource.back().name += tmpNext;
                            state = s_noe;
                            update(tmpNextNext);
                            continue;
                        }
                    } else {
                        lineOffset++;
                        i++;
                        tokenSource.back().name += tmpNext;
                        state = s_noe;
                        update(tmpNextNext);
                        continue;
                    }
                }
            } else if (tmp == '.') {
                if (tmpNext == '.' && tmpNextNext == '.') {
                    lineOffset += 2;
                    i += 2;
                    tokenSource.back().name.append(2, '.');
                    state = s_noe;
                    update(tmpNextNextNext);
                    continue;
                }
            }
            state = s_noe;
        } else if (noteState > note_ready) {
            if (!tokenSource.empty()) {
                UInt64 size = tokenSource.size();
                tokenSource[size - 1].tokenType = tt_noe;
                tokenSource[size - 1].name.clear();
                tokenSource[size - 1].line = 0;
                tokenSource[size - 1].lineOffset = 0;
            }
        } else if (
                state == s_identifier ||
                state == s_number ||
                state == s_string ||
                state == s_char
                ) {
            tokenSource.back().name += tmp;
        }

        //根据下一个字符更新状态
        update(tmpNext);
    }
    if (tokenErrorType == te_errorString) {
        Token &token = tokenSource[tokenSource.size() - 2];
        UInt64 offset = token.lineOffset + token.name.size();
        token.tokenError.push_back({te_errorString, offset, 1});
    }
    if (tokenErrorType == te_errorChar) {
        Token &token = tokenSource[tokenSource.size() - 2];
        UInt64 offset = token.lineOffset + token.name.size();
        token.tokenError.push_back({te_errorChar, offset, 1});
    }
    for (const auto &i: tokenSource)
        if ((i.tokenType != tt_noe) && (!i.name.empty()))
            tokenStream.push_back(i);
    return tokenStream;
}
