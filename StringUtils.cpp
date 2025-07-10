#include "StringUtils.h"
#include <algorithm>
#include <cctype>

std::string StringUtils::trim(const std::string &str)
{
    auto start = str.begin();
    while (start != str.end() && std::isspace(static_cast<unsigned char>(*start)))
        start++;

    auto end = str.rbegin();
    while (end != str.rend() && std::isspace(static_cast<unsigned char>(*end)))
        end++;

    return std::string(start, end.base());
}

bool StringUtils::isValidUserID(const std::string &id)
{
    // 仅数字，长度限制1-20字符
    std::regex pattern("^[0-9]{1,20}$");
    return std::regex_match(id, pattern);
}

std::string StringUtils::escapeSQL(const std::string &str)
{
    std::string res;
    for (char c : str)
    {
        switch (c)
        {
        case '\'':
            res += "\\'";
            break; // 单引号转义
        case '"':
            res += "\\\"";
            break; // 双引号转义
        case '\\':
            res += "\\\\";
            break; // 反斜杠转义
        case '\n':
            res += "\\n";
            break; // 换行符转义
        case '\r':
            res += "\\r";
            break; // 回车符转义
        case '\t':
            res += "\\t";
            break; // 制表符转义
        case '\b':
            res += "\\b";
            break; // 退格符转义
        case '\v':
            res += "\\v";
            break; // 垂直制表符转义
        case '\0':
            res += "\\0";
            break; // 空字符转义
        default:
            res += c; // 其他字符不变
        }
    }
    return res;
}