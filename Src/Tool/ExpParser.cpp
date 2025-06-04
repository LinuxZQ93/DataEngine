#include "ExpParser.h"

#include "Print/Print.h"

namespace engine {

CExpParser::CExpParser(const std::string &expr)
    : m_expr(expr)
    , m_pos(0)
    , m_currentToken('\0')
{
    nextToken();
}

CExpParser::~CExpParser() {}

double CExpParser::calculate()
{
    double result = 0;
    try {
        result = parseConditional();
        if (m_currentToken != '\0')
        {
            throw std::runtime_error("unknown character: " + std::string(1, m_currentToken));
        }
        return result;
    }
    catch (const std::exception &e) {
        errorf("%s:%s\n", m_expr.c_str(), e.what());
        result = 0;
    }
    return result;
}

void CExpParser::nextToken()
{
    while (m_pos < m_expr.size() && isspace(m_expr[m_pos]))
    {
        ++m_pos;
    }

    if (m_pos >= m_expr.size())
    {
        m_currentToken = '\0';
        return;
    }

    char c = m_expr[m_pos];
    if (isdigit(c) || c == '.')
    {
        // 处理数字
        size_t start = m_pos;
        while (m_pos < m_expr.size() && (isdigit(m_expr[m_pos]) || m_expr[m_pos] == '.'))
        {
            ++m_pos;
        }
        std::string numStr = m_expr.substr(start, m_pos - start);
        m_value = stod(numStr);
        m_currentToken = 'n'; // 'n'表示数字
    }
    else
    {
        switch (c)
        {
        case '&':
            if (m_pos + 1 < m_expr.size() && m_expr[m_pos + 1] == '&')
            {
                m_currentToken = '&';
                m_pos += 2; // 用&&表示逻辑与
            }
            else
            {
                m_currentToken = 'a';
                m_pos += 1; // 用a表示按位与
            }
            break;
        case '|':
            if (m_pos + 1 < m_expr.size() && m_expr[m_pos + 1] == '|')
            {
                m_currentToken = '|';
                m_pos += 2; // 用||表示逻辑或
            }
            else
            {
                m_currentToken = 'o';
                m_pos += 1; // 用o表示按位或
            }
            break;
        case '<':
            if (m_pos + 1 < m_expr.size() && m_expr[m_pos + 1] == '=')
            {
                m_currentToken = 'l';
                m_pos += 2; // <=
            }
            else
            {
                m_currentToken = '<';
                m_pos += 1;
            }
            break;
        case '>':
            if (m_pos + 1 < m_expr.size() && m_expr[m_pos + 1] == '=')
            {
                m_currentToken = 'g';
                m_pos += 2; // >=
            }
            else
            {
                m_currentToken = '>';
                m_pos += 1;
            }
            break;
        case '!':
            if (m_pos + 1 < m_expr.size() && m_expr[m_pos + 1] == '=')
            {
                m_currentToken = 'n';
                m_pos += 2; // !=
            }
            else
            {
                m_currentToken = '!';
                m_pos += 1;
            }
            break;
        case '=':
            if (m_pos + 1 < m_expr.size() && m_expr[m_pos + 1] == '=')
            {
                m_currentToken = 'e';
                m_pos += 2; // ==
            }
            else
            {
                throw std::runtime_error("Unexpected =");
            }
            break;
        default:
            m_currentToken = m_expr[m_pos++];
        }
    }
}

// 条件运算符 ?:
double CExpParser::parseConditional()
{
    double cond = parseLogicalOr();
    if (m_currentToken == '?')
    {
        nextToken();
        double true_val = parseConditional();
        if (m_currentToken != ':')
        {
            throw std::runtime_error("Missing : in conditional");
        }
        nextToken();
        double false_val = parseConditional();
        return cond != 0.0 ? true_val : false_val;
    }
    return cond;
}

 // 逻辑或 ||
double CExpParser::parseLogicalOr()
{
    double result = parseLogicalAnd();
    while (m_currentToken == '|')
    {
        nextToken();
        double rhs = parseLogicalAnd();
        result = (result != 0.0 || rhs != 0.0) ? 1.0 : 0.0;
    }
    return result;
}

// 逻辑与 &&
double CExpParser::parseLogicalAnd()
{
    double result = parseBitOr();
    while (m_currentToken == '&')
    {
        nextToken();
        double rhs = parseBitOr();
        result = (result != 0.0 && rhs != 0.0) ? 1.0 : 0.0;
    }
    return result;
}

double CExpParser::parseBitOr()
{
    double result = parseBitAnd();
    while (m_currentToken == 'o')
    {
        nextToken();
        result = static_cast<int>(result) | static_cast<int>(parseBitAnd());
    }
    return result;
}

double CExpParser::parseBitAnd()
{
    double result = parseComparison();
    while (m_currentToken == 'a')
    {
        nextToken();
        result = static_cast<int>(result) & static_cast<int>(parseComparison());
    }
    return result;
}

// 比较运算符
double CExpParser::parseComparison()
{
    double result = parseAddSub();
    while (true)
    {
        if (m_currentToken == '<')
        {
            nextToken();
            double rhs = parseAddSub();
            result = (result < rhs) ? 1.0 : 0.0;
        }
        else if (m_currentToken == 'l')
        { // <=
            nextToken();
            double rhs = parseAddSub();
            result = (result <= rhs) ? 1.0 : 0.0;
        }
        else if (m_currentToken == '>')
        {
            nextToken();
            double rhs = parseAddSub();
            result = (result > rhs) ? 1.0 : 0.0;
        }
        else if (m_currentToken == 'g')
        { // >=
            nextToken();
            double rhs = parseAddSub();
            result = (result >= rhs) ? 1.0 : 0.0;
        }
        else if (m_currentToken == 'e')
        { // ==
            nextToken();
            double rhs = parseAddSub();
            result = (result == rhs) ? 1.0 : 0.0;
        }
        else if (m_currentToken == 'n')
        { // !=
            nextToken();
            double rhs = parseAddSub();
            result = (result != rhs) ? 1.0 : 0.0;
        }
        else
        {
            break;
        }
    }
    return result;
}

double CExpParser::parseAddSub()
{
    double result = parseTerm();
    while (m_currentToken == '+' || m_currentToken == '-')
    {
        char op = m_currentToken;
        nextToken();
        double term = parseTerm();
        op == '+' ? result += term : result -= term;
    }
    return result;
}

double CExpParser::parseTerm()
{
    double result = parseFactor();
    while (m_currentToken == '*' || m_currentToken == '/')
    {
        char op = m_currentToken;
        nextToken();
        double factor = parseFactor();
        if (op == '*')
        {
            result *= factor;
        }
        else
        {
            if (factor == 0)
            {
                throw std::runtime_error("Division by zero");
            }
            result /= factor;
        }
    }
    return result;
}

double CExpParser::parseFactor()
{
    if (m_currentToken == 'n')
    {
        double value = m_value;
        nextToken();
        return value;
    }
    else if (m_currentToken == '(')
    {
        nextToken();
        double result = parseConditional();
        if (m_currentToken != ')')
        {
            throw std::runtime_error("Missing ) or unknown character: " + std::string(1, m_currentToken));
        }
        nextToken();
        return result;
    }
    else if (m_currentToken == '-')
    {
        nextToken();
        return -parseFactor();
    }
    else if (m_currentToken == '!')
    {
        nextToken();
        return parseFactor() == 0.0 ? 1.0 : 0.0;
    }
    throw std::runtime_error("Unexpected character: " + std::string(1, m_currentToken));
}

void replaceRangeStr(const std::string &first, const std::string &end, const exprFunc &func, std::string &str)
{
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = 0;
    do
    {
        pos1 = str.find(first, pos2);
        if (pos1 == std::string::npos)
        {
            break;
        }
        pos2 = str.find(end, pos1);
        if (pos2 == std::string::npos)
        {
            break;
        }
        const std::string &src = str.substr(pos1 + first.size(), pos2 - pos1 - first.size());
        std::string dst = func(src);
        str.replace(pos1, pos2 - pos1 + 1, dst);
        pos1 = 0, pos2 = 0;
    } while (true);
}

}
