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
        return results;

    MYSQL_ROW row;
    unsigned int num_fields = mysql_num_fields(res);
    MYSQL_FIELD *fields = mysql_fetch_fields(res);

    // 添加列名
    std::vector<std::string> column_names;
    for (unsigned int i = 0; i < num_fields; ++i)
    {
        column_names.push_back(fields[i].name);
    }
    results.push_back(column_names);

    // 添加行数据
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

// 创建用户
bool DBConnector::createUser(const std::string &studentID, const std::string &name, const std::string &gender, const std::string &major, int enrollmentYear, const std::string &contactInfo, const std::string &username, const std::string &password, bool isAdmin)
{
    std::string sql = "INSERT INTO users (studentID, name, gender, major, enrollmentYear, contactInfo, username, password, isAdmin) VALUES ('" +
                      studentID + "', '" + name + "', '" + gender + "', '" + major + "', " + std::to_string(enrollmentYear) + ", '" +
                      contactInfo + "', '" + username + "', '" + password + "', " + (isAdmin ? "1" : "0") + ")";
    return update(sql);
}

// 按用户名查询用户
std::vector<std::vector<std::string>> DBConnector::getUserByUsername(const std::string &username)
{
    std::string sql = "SELECT * FROM users WHERE username = '" + username + "'";
    return query(sql);
}

// 查询所有用户
std::vector<std::vector<std::string>> DBConnector::getAllUsers()
{
    return query("SELECT * FROM users");
}

// 更新用户信息（不含密码）
bool DBConnector::updateUser(const std::string &studentID, const std::string &name, const std::string &gender, const std::string &major, int enrollmentYear, const std::string &contactInfo, const std::string &username, bool isAdmin)
{
    std::string sql = "UPDATE users SET name = '" + name + "', gender = '" + gender + "', major = '" + major + "', enrollmentYear = " +
                      std::to_string(enrollmentYear) + ", contactInfo = '" + contactInfo + "', username = '" + username + "', isAdmin = " +
                      (isAdmin ? "1" : "0") + " WHERE studentID = '" + studentID + "'";
    return update(sql);
}

// 更新密码
bool DBConnector::updateUserPassword(const std::string &username, const std::string &newPassword)
{
    std::string sql = "UPDATE users SET password = '" + newPassword + "' WHERE username = '" + username + "'";
    return update(sql);
}

// 删除用户
bool DBConnector::deleteUser(const std::string &studentID)
{
    std::string sql = "DELETE FROM users WHERE studentID = '" + studentID + "'";
    return update(sql);
}