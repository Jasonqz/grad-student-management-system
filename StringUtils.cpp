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
        if (c == '\'')
            res += "''"; // 单引号转义为两个单引号
        else if (c == '"')
            res += "\""; // 双引号转义
        else if (c == '\\')
            res += "\\\\"; // 反斜杠转义
        else
            res += c;
    }
    return res;
}