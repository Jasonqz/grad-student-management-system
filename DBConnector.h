#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <mysql.h>
#include <string>
#include <vector>

class DBConnector
{
private:
    MYSQL *conn;
    static DBConnector *instance;
    DBConnector(const std::string &host, const std::string &user, const std::string &password, const std::string &dbname, unsigned int port);

public:
    ~DBConnector();
    static DBConnector *getInstance(const std::string &host = "localhost", const std::string &user = "root", const std::string &password = "qz284781", const std::string &dbname = "grad_student_management", unsigned int port = 3306);
    std::vector<std::vector<std::string>> query(const std::string &sql);
    bool update(const std::string &sql);
};

#endif // DBCONNECTOR_H