#include "DBConnector.h"
#include <stdexcept>
#include <iostream>

DBConnector *DBConnector::instance = nullptr;

DBConnector::DBConnector(const std::string &host, const std::string &user, const std::string &password, const std::string &dbname, unsigned int port)
{
    conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0))
    {
        throw std::runtime_error(mysql_error(conn));
    }
}

DBConnector::~DBConnector()
{
    if (conn)
    {
        mysql_close(conn);
    }
}

DBConnector *DBConnector::getInstance(const std::string &host, const std::string &user, const std::string &password, const std::string &dbname, unsigned int port)
{
    if (!instance)
    {
        instance = new DBConnector(host, user, password, dbname, port);
    }
    return instance;
}

std::vector<std::vector<std::string>> DBConnector::query(const std::string &sql)
{
    std::vector<std::vector<std::string>> results;
    if (mysql_query(conn, sql.c_str()) != 0)
    {
        throw std::runtime_error(mysql_error(conn));
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res)
    {
        return results; // 非查询语句
    }

    MYSQL_ROW row;
    unsigned int num_fields = mysql_num_fields(res);

    // 获取列名
    MYSQL_FIELD *fields = mysql_fetch_fields(res);
    std::vector<std::string> column_names;
    for (unsigned int i = 0; i < num_fields; ++i)
    {
        column_names.push_back(fields[i].name);
    }
    results.push_back(column_names);

    // 获取行数据
    while ((row = mysql_fetch_row(res)))
    {
        std::vector<std::string> row_data;
        for (unsigned int i = 0; i < num_fields; ++i)
        {
            row_data.push_back(row[i] ? row[i] : "");
        }
        results.push_back(row_data);
    }

    mysql_free_result(res);
    return results;
}

bool DBConnector::update(const std::string &sql)
{
    return mysql_query(conn, sql.c_str()) == 0;
}