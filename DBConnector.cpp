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
    // mysql_ssl_set(_conn, "", "", "", "", "");
    // 移除以下不兼容的行
    // int verify = 0;
    // mysql_options(_conn, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &verify);
    // 保留SSL模式禁用配置
    // mysql_options(_conn, MYSQL_OPT_SSL_MODE, "DISABLED");
    // mysql_ssl_set(_conn, "", "", "", "", "");

    // 连接数据库
    if (mysql_real_connect(_conn, ip.c_str(), name.c_str(), pass.c_str(), dataBaseName.c_str(), port, NULL, 0))
    {
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

std::string DBConnector::query(std::string const &tableName)
{
    if (!_state)
        return "";

    // 获取列数
    int field = getTableField(tableName);

    // 将字符串格式化为查询数组
    char query[150];
    sprintf_s(query, "select * from %s", tableName.c_str());

    // 执行查询
    if (mysql_query(_conn, query))
        return "";

    // 获取查询结果
    _res = mysql_store_result(_conn);
    if (_res == nullptr)
        return "";

    // 将查询结果转化为字符串输出
    fd.reserve(field);
    fd.resize(field);
    for (int i = 0; i < field; i++)
        fd[i] = mysql_fetch_field(_res);

    std::string res = "";
    while (_column = mysql_fetch_row(_res))
    {
        for (int i = 0; i < field; i++)
            res += _column[i], res += "\t";
        res += "\n";
    }
    return res;
}

bool DBConnector::implement(std::string const &sentence)
{
    if (!_state)
        return false;

    // 字符串格式化
    char query[150];
    sprintf_s(query, "%s", sentence.c_str());

    // 执行命令
    if (mysql_query(_conn, query))
        return false;

    return true;
}