#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>

using namespace std;

// 用户基类
class User
{
protected:
    string studentID;   // 学号/工号
    string name;        // 姓名
    string gender;      // 性别
    string major;       // 专业
    int enrollmentYear; // 入学/入职年份
    string contactInfo; // 联系方式

    string username; // 用户名
    string password; // 密码
    bool isAdmin;    // 是否为管理员

public:
    // 构造函数
    User(string id = "", string name = "", string gender = "",
         string major = "", int year = 0, string contact = "",
         string uname = "", string pwd = "", bool admin = false);
    virtual ~User();

    // Getter方法
    string getID() const;
    string getName() const;
    string getGender() const;
    string getMajor() const;
    int getEnrollmentYear() const;
    string getContactInfo() const;
    string getUsername() const;
    string getPassword() const; // 添加此行

    bool checkPassword(string pwd) const;
    bool isAdminUser() const;

    // Setter方法
    void setID(string id);
    void setName(string name);
    void setGender(string gender);
    void setMajor(string major);
    void setEnrollmentYear(int year);
    void setContactInfo(string contact);
    void setUsername(string uname);
    void setPassword(string pwd);
    void setAdmin(bool admin);

    // 输入/输出方法
    void inputInfo(bool partial = false);
    void displayInfo() const;

    // 权限判断
    bool canModifyAll() const; // 直接使用isAdmin判断
    bool canModifySelf(string id) const;

    // 文件操作重载
    friend ostream &operator<<(ostream &os, const User &user);
    friend istream &operator>>(istream &is, User &user);
};

#endif // USER_H