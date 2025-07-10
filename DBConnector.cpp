#include "DBConnector.h"
#include <iostream>

DBConnector::DBConnector() : _state(false), _res(nullptr), _column(nullptr)
{
    _conn = new MYSQL;
}

bool DBConnector::connect(std::string const &ip, std::string const &name, std::string const &pass, std::string const &dataBaseName, int const port)
{
    if (_state)
        return false;

    // 初始化数据库
    if (!mysql_init(_conn))
    {
        std::cerr << "MySQL初始化失败: " << mysql_error(_conn) << std::endl;
        return false;
    }
    // 添加此行禁用SSL连接
    int verify = 0;

    // 连接数据库
    if (mysql_real_connect(_conn, ip.c_str(), name.c_str(), pass.c_str(), dataBaseName.c_str(), port, NULL, 0))
    {
        // 设置数据库连接字符集为utf8mb4
        if (mysql_set_character_set(_conn, "utf8mb4") != 0)
        {
            std::cerr << "设置字符集失败: " << mysql_error(_conn) << std::endl;
            mysql_close(_conn);
            return false;
        }
        _state = true;
        return true;
    }
    else
    {
        // 输出错误信息以便调试
        std::cerr << "MySQL连接错误: " << mysql_error(_conn) << std::endl;
        return false;
    }
}

int DBConnector::getTableField(std::string const &tableName)
{
    if (!_state)
        return -1;

    // 将字符串格式化为查询数组
    char query[150];
    sprintf_s(query, "show columns from %s", tableName.c_str());

    // 执行查询
    if (mysql_query(_conn, query))
        return -1;

    // 获取查询结果
    _res = mysql_store_result(_conn);
    if (_res == nullptr)
        return -1;

    return mysql_affected_rows(_conn);
}

std::string DBConnector::query(std::string const &sql)
{
    if (!_state)
    {
        std::cerr << "[错误] 数据库未连接，无法执行查询" << std::endl;
        return "";
    }

    // 执行查询
    if (mysql_query(_conn, sql.c_str()))
    {
        std::cerr << "SQL查询失败: " << mysql_error(_conn) << std::endl;
        std::cerr << "SQL语句: " << sql << std::endl;
        return "";
    }

    // 获取查询结果
    _res = mysql_store_result(_conn);
    if (_res == nullptr)
    {
        // 区分"无结果"和"真错误"
        if (mysql_field_count(_conn) == 0)
        {
            // 无结果集（非SELECT查询）
            std::cerr << "[调试] 查询成功但无结果集返回" << std::endl;
            return "";
        }
        else
        {
            // 真错误
            std::cerr << "[严重错误] 获取结果集失败: " << mysql_error(_conn) << std::endl;
            return "";
        }
    }

    // 获取字段信息
    int fieldCount = mysql_num_fields(_res);
    MYSQL_FIELD *fields = mysql_fetch_fields(_res);

    std::string res = "";
    while (_column = mysql_fetch_row(_res))
    {
        for (int i = 0; i < fieldCount; i++)
        {
            res += (_column[i] ? _column[i] : "NULL");
            res += "\t";
        }
        res += "\n";
    }

    mysql_free_result(_res); // 释放结果集
    return res;
}

bool DBConnector::implement(std::string const &sentence)
{
    if (!_state)
    {
        std::cerr << "数据库未连接，无法执行SQL命令" << std::endl;
        return false;
    }

    // 执行命令
    if (mysql_query(_conn, sentence.c_str()))
    {
        std::cerr << "SQL执行失败: " << mysql_error(_conn) << std::endl;
        std::cerr << "SQL语句: " << sentence << std::endl;
        return false;
    }

    return true;
}