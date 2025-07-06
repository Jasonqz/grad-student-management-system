#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <string>
#include <vector>
#include <mysql.h>

class DBConnector
{
private:
    MYSQL *conn;
    static DBConnector *instance;
    DBConnector(const std::string &host, const std::string &user, const std::string &password, const std::string &dbname, unsigned int port);

public:
    ~DBConnector();
    static DBConnector *getInstance(const std::string &host = "localhost", const std::string &user = "root", const std::string &password = "your_password", const std::string &dbname = "grad_student_management", unsigned int port = 3306);

    // Users表CRUD操作
    bool createUser(const std::string &studentID, const std::string &name, const std::string &gender, const std::string &major, int enrollmentYear, const std::string &contactInfo, const std::string &username, const std::string &password, bool isAdmin = false);
    std::vector<std::vector<std::string>> getUserByUsername(const std::string &username);
    std::vector<std::vector<std::string>> getAllUsers();
    bool updateUser(const std::string &studentID, const std::string &name, const std::string &gender, const std::string &major, int enrollmentYear, const std::string &contactInfo, const std::string &username, bool isAdmin);
    bool updateUserPassword(const std::string &username, const std::string &newPassword);
    bool deleteUser(const std::string &studentID);

private:
    std::vector<std::vector<std::string>> query(const std::string &sql);
    bool update(const std::string &sql);
};

#endif // DBCONNECTOR_H