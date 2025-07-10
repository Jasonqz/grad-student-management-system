#pragma once

#include <mysql.h>
#include <vector>
#include <string>
#include <windows.h>
#include <winsock.h>

class DBConnector
{
private:
    bool _state;                     // 连接状态
    MYSQL *_conn;                    // 数据库连接
    std::vector<MYSQL_FIELD *> fd;   // 字段列数组
    std::vector<std::string> _field; // 字段名
    MYSQL_RES *_res;                 // 返回行的查询结果集
    MYSQL_ROW _column;               // 一个行数据的类型安全的表示
    static DBConnector *instance;    // 单例实例
    /*
     * @brief 构造函数，初始化参数
     */
    DBConnector();
    // 私有析构函数
    ~DBConnector();
    // 禁止拷贝构造和赋值运算符
    DBConnector(const DBConnector &) = delete;
    DBConnector &operator=(const DBConnector &) = delete;

public:
    /*
     * @brief 获取单例实例
     * @return DBConnector* 单例对象指针
     */
    static DBConnector *getInstance();

    /*
     * @brief 连接数据库
     * @param ip IP地址
     * @param name 用户名
     * @param pass 密码
     * @param dataBaseName 数据库名
     * @param port 端口
     */
    bool connect(std::string const &ip, std::string const &name, std::string const &pass, std::string const &dataBaseName, int const port);

    /*
     * @brief 获取数据库连接
     * @return MYSQL* 数据库连接指针
     */
    MYSQL *getConnection() const { return _conn; }

    /*
     * @brief 检查连接状态
     * @return bool 连接状态
     */
    bool isConnected() const { return _state; }

    /*
     * @brief 获取表字段数
     * @param tableName 表名
     */
    int getTableField(std::string const &tableName);

    /*
     * @brief 执行SQL查询
     * @param sql SQL查询语句
     * @return 查询结果
     */
    std::string query(std::string const &sql);

    /*
     * @brief 执行 sql 指令
     * @param sentence sql 命令
     */
    bool implement(std::string const &sentence);
};
