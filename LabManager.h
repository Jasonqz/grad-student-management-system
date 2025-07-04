#ifndef LABMANAGER_H
#define LABMANAGER_H

#include <string>
#include <fstream>
#include <vector>
#include "User.h"

using namespace std;

// 链表节点定义
struct Node {
    User data;
    Node* next;
    Node(User user) : data(user), next(nullptr) {}
};

class LabManager {
private:
    Node* head; // 链表头指针
    User* currentUser; // 当前登录用户
    
    // 辅助函数：查找节点
    Node* findNode(string id) const;
    
public:
    // 构造函数和析构函数
    LabManager();
    ~LabManager();
    
    // 用户管理
    bool login(string username, string password);
    User* getCurrentUser() const;
    User* queryUser(string id) const;
    
    // 人员信息管理
    bool addUser(User user);
    bool deleteUser(string id);
    bool updateUser(string id, User newInfo);
    void sortByID();
    
    // 数据持久化
    void saveToFile(string filename) const;
    void loadFromFile(string filename);
    
    // 其他功能
    void displayAllUsers() const;
};

#endif // LABMANAGER_H    