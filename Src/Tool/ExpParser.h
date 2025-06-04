#ifndef __DATAENGINE_SRC_TOOL_EXPPARSER_H__
#define __DATAENGINE_SRC_TOOL_EXPPARSER_H__
#include <string>
#include <stdexcept>

#include "Function/Function.h"

namespace engine {

class CExpParser {
public:
    CExpParser() = delete;
    CExpParser(const std::string &exp);
    ~CExpParser();

public:
    double calculate();

private:
    void nextToken();
    double parseConditional();
    double parseLogicalOr();
    double parseLogicalAnd();
    double parseBitOr();
    double parseBitAnd();
    double parseComparison();
    double parseAddSub();
    double parseTerm();
    double parseFactor();

private:
    std::string m_expr;
    size_t m_pos;
    char m_currentToken;
    double m_value;
};

using exprFunc = TFunction<std::string, const std::string &>;
void replaceRangeStr(const std::string &first, const std::string &end, const exprFunc &func, std::string &str);
}

#endif /* __DATAENGINE_SRC_TOOL_EXPPARSER_H__ */
