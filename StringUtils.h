#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <regex>

class StringUtils
{
public:
    /**
     * @brief 修剪字符串首尾的空白字符（包括换行符、制表符等）
     * @param str 待修剪的字符串
     * @return 修剪后的字符串
     */
    static std::string trim(const std::string &str);

    /**
     * @brief 验证用户ID合法性（仅数字）
     * @param id 用户ID
     * @return true-合法，false-非法
     */
    static bool isValidUserID(const std::string &id);

    /**
     * @brief SQL字符串转义，防止SQL注入和语法错误
     * @param str 原始字符串
     * @return 转义后的字符串
     */
    static std::string escapeSQL(const std::string &str);
};

#endif // STRINGUTILS_H