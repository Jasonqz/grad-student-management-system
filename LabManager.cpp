#include "LabManager.h"
#include <algorithm>
#include <sstream> // 添加此行以使用istringstream

LabManager::LabManager() : head(nullptr), currentUser(nullptr)
{
    // 从文件加载数据
    loadFromFile("data.dat");
    // 数据库连接参数，请根据实际情况修改
    // _db.connect("localhost", "root", "qz284781", "grad_student_management", 3306);
}

// SQL字符串转义，防止SQL注入和语法错误
string LabManager::escapeSQL(const string &str)
{
    string res;
    for (char c : str)
    {
        if (c == '\'')
            res += "''";
        else
            res += c;
    }
    return res;
}

// 添加用户到数据库
bool LabManager::addUserToDataBase(const User &user)
{
    // 检查数据库连接状态
    if (!_db.isConnected())
    {
        _db.connect("localhost", "root", "qz284781", "grad_student_management", 3306);
        if (!_db.isConnected())
            return false;
    }

    string sql = "INSERT INTO students (student_id, name, gender, major, enrollment_year, contact_info, username, password, is_admin) VALUES ('" + escapeSQL(user.getID()) + "', '" + escapeSQL(user.getName()) + "', '" + escapeSQL(user.getGender()) + "', '" + escapeSQL(user.getMajor()) + "', " + to_string(user.getEnrollmentYear()) + ", '" + escapeSQL(user.getContactInfo()) + "', '" + escapeSQL(user.getUsername()) + "', '" + escapeSQL(user.getPassword()) + "', " + (user.isAdminUser() ? "1" : "0") + ")";

    // 输出SQL语句用于调试
    cout << "执行SQL: " << sql << endl;

    bool result = _db.implement(sql);
    if (!result)
    {
        cerr << "添加用户到数据库失败! SQL: " << sql << endl;
        // 获取MySQL错误信息
        MYSQL *conn = _db.getConnection(); // 需要在DBConnector中添加获取连接的方法
        if (conn)
            cerr << "MySQL错误: " << mysql_error(conn) << endl;
    }
    return result;
}

// 从数据库删除用户
bool LabManager::deleteUserFromDataBase(const string &userID)
{
    // 检查数据库连接状态
    if (!_db.isConnected())
    {
        cerr << "数据库未连接，无法添加用户到数据库" << endl;
        return false;
    }
    // 输出SQL语句用于调试
    string sql = "DELETE FROM students WHERE student_id = '" + escapeSQL(userID) + "'";
    cout << "执行SQL: " << sql << endl;
    return _db.implement(sql);
}

// 更新数据库中的用户信息
bool LabManager::updateUserInDataBase(const string &userID, const User &newInfo)
{
    // 检查数据库连接状态
    if (!_db.isConnected())
    {
        cerr << "数据库未连接，无法添加用户到数据库" << endl;
        return false;
    }
    string sql = "UPDATE students SET name = '" + escapeSQL(newInfo.getName()) + "', gender = '" + escapeSQL(newInfo.getGender()) + "', major = '" + escapeSQL(newInfo.getMajor()) + "', " + "enrollment_year = " + to_string(newInfo.getEnrollmentYear()) + ", contact_info = '" + escapeSQL(newInfo.getContactInfo()) + "', " + "username = '" + escapeSQL(newInfo.getUsername()) + "', password = '" + escapeSQL(newInfo.getPassword()) + "', is_admin = " + (newInfo.isAdminUser() ? "1" : "0") + " " + "WHERE student_id = '" + escapeSQL(userID) + "'";
    // 输出SQL语句用于调试
    cout << "执行SQL: " << sql << endl;

    return _db.implement(sql);
}

// 从数据库查询用户
bool LabManager::queryUserFromDataBase(const string &userID, User &user)
{
    string sql = "SELECT * FROM students WHERE student_id = '" + escapeSQL(userID) + "'";
    string result = _db.query(sql);
    istringstream iss(result);
    string line;

    if (getline(iss, line) && !line.empty())
    {
        istringstream lineStream(line);
        vector<string> fields;
        string field;

        while (getline(lineStream, field, '\t'))
        {
            fields.push_back(field);
        }

        if (fields.size() >= 9)
        {
            user.setID(fields[0]);
            user.setName(fields[1]);
            user.setGender(fields[2]);
            user.setMajor(fields[3]);
            user.setEnrollmentYear(stoi(fields[4]));
            user.setContactInfo(fields[5]);
            user.setUsername(fields[6]);
            user.setPassword(fields[7]);
            user.setAdmin(fields[8] == "1");
            return true;
        }
    }
    return false;
}

LabManager::~LabManager()
{
    // 释放链表内存
    while (head)
    {
        Node *temp = head;
        head = head->next;
        delete temp;
    }
}

Node *LabManager::findNode(string id) const
{
    Node *current = head;
    while (current)
    {
        if (current->data.getID() == id)
        {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

bool LabManager::login(string username, string password)
{
    Node *current = head;
    while (current)
    {
        if (current->data.getUsername() == username &&
            current->data.checkPassword(password))
        {
            currentUser = &(current->data);
            return true;
        }
        current = current->next;
    }
    return false;
}

User *LabManager::getCurrentUser() const
{
    return currentUser;
}

User *LabManager::queryUser(string id) const
{
    Node *node = findNode(id);
    if (node)
    {
        return &(node->data);
    }
    return nullptr;
}

bool LabManager::addUser(User user)
{
    if (!currentUser || !currentUser->canModifyAll())
    {
        return false;
    }

    // 检查学号是否已存在
    if (findNode(user.getID()))
    {
        return false;
    }

    // 添加到链表头部
    Node *newNode = new Node(user);
    newNode->next = head;
    head = newNode;
    // 再添加到数据库
    addUserToDataBase(user);

    return true;
}

bool LabManager::deleteUser(string id)
{
    if (!currentUser || !currentUser->canModifyAll())
    {
        return false;
    }

    Node *current = head;
    Node *previous = nullptr;

    while (current && current->data.getID() != id)
    {
        previous = current;
        current = current->next;
    }

    if (!current)
    {
        return false; // 未找到
    }

    if (!previous)
    {
        head = current->next; // 删除头节点
    }
    else
    {
        previous->next = current->next; // 删除中间或尾节点
    }

    delete current;
    // 再从数据库删除
    deleteUserFromDataBase(id);
    return true;
}

bool LabManager::updateUser(string id, User newInfo)
{
    if (!currentUser)
    {
        return false;
    }

    // 检查权限
    if (!currentUser->canModifyAll() && !currentUser->canModifySelf(id))
    {
        return false;
    }

    Node *node = findNode(id);
    if (!node)
    {
        return false;
    }
    // 保存原始ID（可能被修改）
    string originalID = node->data.getID();

    // 更新信息（保留原信息，只替换新提供的信息）
    if (!newInfo.getID().empty())
        node->data.setID(newInfo.getID());
    if (!newInfo.getName().empty())
        node->data.setName(newInfo.getName());
    if (!newInfo.getGender().empty())
        node->data.setGender(newInfo.getGender());
    if (!newInfo.getMajor().empty())
        node->data.setMajor(newInfo.getMajor());
    if (newInfo.getEnrollmentYear() != 0)
        node->data.setEnrollmentYear(newInfo.getEnrollmentYear());
    if (!newInfo.getContactInfo().empty())
        node->data.setContactInfo(newInfo.getContactInfo());

    // 只有管理员可以修改用户名、密码和角色
    if (currentUser->canModifyAll())
    {
        if (!newInfo.getUsername().empty())
            node->data.setUsername(newInfo.getUsername());
        if (newInfo.isAdminUser() != node->data.isAdminUser())
            node->data.setAdmin(newInfo.isAdminUser());
    }

    // 管理员或本人可以修改密码
    if (currentUser->canModifyAll() || currentUser->canModifySelf(id))
    {
        if (!newInfo.getPassword().empty())
            node->data.setPassword(newInfo.getPassword());
    }
    // 更新数据库
    updateUserInDataBase(originalID, node->data);

    return true;
}

void LabManager::sortByID()
{
    if (!head || !head->next)
        return;

    Node *dummy = new Node(User()); // 创建哑节点
    Node *sorted = dummy;           // 已排序部分的尾指针
    Node *current = head;           // 当前待插入节点

    while (current)
    {
        Node *next = current->next; // 保存下一个节点
        Node *prev = dummy;         // 用于遍历已排序部分

        // 在已排序部分找到插入位置
        while (prev->next && stoi(prev->next->data.getID()) < stoi(current->data.getID()))
        {
            prev = prev->next;
        }

        // 插入当前节点到已排序部分
        current->next = prev->next;
        prev->next = current;
        current = next; // 处理下一个节点
    }

    head = dummy->next;
    delete dummy; // 释放哑节点
}

void LabManager::saveToFile(string filename) const
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cerr << "无法打开文件 " << filename << endl;
        return;
    }

    Node *current = head;
    while (current)
    {
        file << current->data << endl;
        current = current->next;
    }

    file.close();
}

void LabManager::loadFromFile(string filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        // 文件不存在，创建新文件
        ofstream outFile(filename);
        outFile.close();
        return;
    }

    // 先清空现有数据
    while (head)
    {
        Node *temp = head;
        head = head->next;
        delete temp;
    }

    string line;
    while (getline(file, line))
    {
        if (line.empty())
            continue;

        // 解析数据行
        istringstream iss(line);
        User user;
        iss >> user;

        // 添加到链表
        Node *newNode = new Node(user);
        newNode->next = head;
        head = newNode;
    }

    file.close();
}

void LabManager::displayAllUsers() const
{
    Node *current = head;
    if (!current)
    {
        cout << "暂无人员信息！\n";
        return;
    }

    while (current)
    {
        current->data.displayInfo();
        cout << "------------------------\n";
        current = current->next;
    }
}