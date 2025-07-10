#include "LabManager.h"
#include <algorithm>
#include <sstream> // 添加此行以使用istringstream
#include "StringUtils.h"
#include <cassert> // 添加 assert 头文件

// 数据库连接配置
const std::string DB_IP = "localhost";
const std::string DB_USER = "root";
const std::string DB_PASS = "qz284781";
const std::string DB_NAME = "grad_student_management";
const int DB_PORT = 3306;

/**
 * @brief 构造函数
 *
 */
LabManager::LabManager() : head(nullptr), currentUser(nullptr)
{
    // 从文件加载数据
    loadFromFile("data.dat");
    // 数据库连接参数
    _db.connect(DB_IP, DB_USER, DB_PASS, DB_NAME, DB_PORT);
    assert(_db.isConnected() && "数据库连接失败！");
    std::cout << "[成功] 数据库连接正常\n"
              << std::endl;
}

/**
 * @brief SQL字符串转义，防止SQL注入和语法错误
 * @param str
 * @return string
 */
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

/**
 * @brief 添加用户到数据库
 * @param user
 * @return true
 * @return false
 */
bool LabManager::addUserToDataBase(const User &user)
{
    // 检查数据库连接状态
    if (!_db.isConnected())
    {
        _db.connect(DB_IP, DB_USER, DB_PASS, DB_NAME, DB_PORT);
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

/**
 * @brief 删除用户
 * @param userID
 * @return true
 * @return false
 */
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

/**
 * @brief 更新用户信息
 * @param userID
 * @param newInfo
 * @return true
 * @return false
 */
bool LabManager::updateUserInDataBase(const string &userID, const User &newInfo)
{
    // 检查数据库连接状态
    if (!_db.isConnected())
    {
        // 尝试重新连接数据库
        _db.connect(DB_IP, DB_USER, DB_PASS, DB_NAME, DB_PORT);
        // 检查重连结果
        if (!_db.isConnected())
        {
            cerr << "数据库未连接，无法更新用户信息" << endl; // 修正错误信息
            // 获取并显示MySQL错误信息
            MYSQL *conn = _db.getConnection();
            if (conn)
                cerr << "MySQL错误: " << mysql_error(conn) << endl;
            return false;
        }
    }
    string sql = "UPDATE students SET name = '" + escapeSQL(newInfo.getName()) + "', gender = '" + escapeSQL(newInfo.getGender()) + "', major = '" + escapeSQL(newInfo.getMajor()) + "', " + "enrollment_year = " + to_string(newInfo.getEnrollmentYear()) + ", contact_info = '" + escapeSQL(newInfo.getContactInfo()) + "', " + "username = '" + escapeSQL(newInfo.getUsername()) + "', password = '" + escapeSQL(newInfo.getPassword()) + "', is_admin = " + (newInfo.isAdminUser() ? "1" : "0") + " " + "WHERE student_id = '" + escapeSQL(userID) + "'";
    // 输出SQL语句用于调试
    cout << "执行SQL: " << sql << endl;

    return _db.implement(sql);
}

/**
 * @brief 查询用户信息
 * @param userID
 * @param user
 * @return true
 * @return false
 */
bool LabManager::queryUserFromDataBase(const string &userID, User &user)
{
    string trimmedID = StringUtils::trim(userID);
    // 修正：添加ID验证检查
    if (!StringUtils::isValidUserID(trimmedID))
    {
        cout << "[错误] 用户ID格式无效！仅允许数字，长度1-20字符\n";
        return false;
    }
    cout << "[提示] 正在数据库中查询用户信息，用户ID: " << userID << endl;
    string sql = "SELECT * FROM students WHERE student_id = '" + escapeSQL(trimmedID) + "'";
    cout << "[SQL语句] " << sql << endl; // 调试用，正式环境可移除
    // 检查数据库连接状态
    if (!_db.isConnected())
    {
        _db.connect(DB_IP, DB_USER, DB_PASS, DB_NAME, DB_PORT);
        if (!_db.isConnected())
            return false;
    }

    string result = _db.query(sql);
    if (result.empty())
    {
        cout << "[警告] 数据库查询返回空结果" << endl;
        return false;
    }

    istringstream iss(result);
    string line;

    if (getline(iss, line) && !line.empty())
    {
        istringstream lineStream(line);
        vector<string> fields;
        string field;

        while (getline(lineStream, field, '	'))
        {
            fields.push_back(field);
        }

        if (fields.size() >= 9)
        {
            cout << "[成功] 从数据库中查询到用户信息，字段数量: " << fields.size() << endl;
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
        else
        {
            cout << "[错误] 查询结果字段数量不足，期望至少9个字段，实际获取: " << fields.size() << endl;
            return false;
        }
    }

    cout << "[错误] 未能从查询结果中读取有效数据行" << endl;
    return false;
}

/**
 * @brief 析构函数
 *
 */
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
/**
 * @brief 查找用户节点
 * @param id
 * @return Node*
 */

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

/**
 * @brief 登录
 * @param username
 * @param password
 * @return true
 * @return false
 */

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
/**
 * @brief 获取当前用户
 * @return User*
 */
User *LabManager::getCurrentUser() const
{
    return currentUser;
}

/**
 * @brief 查询用户信息
 * @param id
 * @return User*
 */
User *LabManager::queryUser(string id)
{
    Node *node = findNode(id);
    if (node)
    {
        return &(node->data);
    }
    // 本地未找到，使用现有方法从数据库查询
    User dbUser;
    if (queryUserFromDataBase(id, dbUser))
    {
        // 查询成功，添加到本地数据结构
        const_cast<LabManager *>(this)->addUser(dbUser, false);

        // 返回新添加的用户指针
        return queryUser(id); // 再次查询获取指针
    }
    return nullptr;
}

/**
 * @brief 添加用户
 * @param user
 * @return true
 * @return false
 */
bool LabManager::addUser(User user, bool toDb)
{
    if (!currentUser || !currentUser->canModifyAll())
    {
        return false;
    }

    // 检查学号是否已存在
    if (findNode(user.getID()))
    {
        cout << "错误：用户ID已存在，请使用其他ID。" << endl;
        return false;
    }
    if (!StringUtils::isValidUserID(user.getID()))
    {
        cout << "[错误] 用户ID格式无效！仅允许数字，长度1-20字符\n";
        return false;
    }

    // 添加到链表头部
    Node *newNode = new Node(user);
    newNode->next = head;
    head = newNode;
    // 再添加到数据库
    if (toDb)
        addUserToDataBase(user);

    return true;
}

/**
 * @brief 删除用户
 * @param id
 * @return true
 * @return false
 */

bool LabManager::deleteUser(string id)
{
    if (!currentUser || !currentUser->canModifyAll())
    {
        cout << "错误：权限不足，无法执行删除操作。" << endl;
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
        cout << "错误：未找到ID为 '" << id << "' 的用户。" << endl;
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
    cout << "成功：ID为 '" << id << "' 的用户已被删除。" << endl;
    return true;
}
/**
 * @brief 更新用户信息
 * @param id
 * @param newInfo
 * @return true
 * @return false
 */

bool LabManager::updateUser(string id, User newInfo)
{
    if (!currentUser)
    {
        cout << "系统错误：请重新登录系统。" << endl;
        return false;
    }

    // 检查权限
    if (!currentUser->canModifyAll() && !currentUser->canModifySelf(id))
    {
        cout << "错误：权限不足，无法更新用户信息。" << endl;
        return false;
    }

    Node *node = findNode(id);
    if (!node)
    {
        cout << "错误：未找到ID为 '" << id << "' 的用户。" << endl;
        return false;
    }
    // 保存原始ID（可能被修改）
    string originalID = node->data.getID();

    // 更新信息（保留原信息，只替换新提供的信息）
    // if (!newInfo.getID().empty())
    //     node->data.setID(newInfo.getID());
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

    // 只有管理员可以修改
    if (currentUser->canModifyAll())
    {
        if (newInfo.isAdminUser() != node->data.isAdminUser())
            node->data.setAdmin(newInfo.isAdminUser());
    }

    // 管理员或本人可以修改密码
    if (currentUser->canModifyAll() || currentUser->canModifySelf(id))
    {
        if (!newInfo.getPassword().empty())
            node->data.setPassword(newInfo.getPassword());
        if (!newInfo.getUsername().empty())
            node->data.setUsername(newInfo.getUsername());
    }
    // 更新数据库
    bool res = updateUserInDataBase(originalID, node->data);
    if (res)
        cout << "成功：用户信息已更新。" << endl;
    else
        cout << "数据库更新失败。" << endl;

    return res;
}

/**
 * @brief 按学号排序用户信息
 *
 */
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
/**
 * @brief 保存用户信息到文件
 * @param filename
 */
void LabManager::saveToFile(string filename) const
{
    // 以写入模式打开指定路径的文件
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
        // 检查写入状态
        if (file.fail())
        {
            cerr << "写入文件失败，位置: " << current->data.getUsername() << endl;
            break; // 终止写入
        }
        current = current->next;
    }

    file.close();
}
/**
 * @brief 从文件加载用户信息
 * @param filename
 */

void LabManager::loadFromFile(string filename)
{
    // 打开文件并创建输入文件流对象
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

    // 存储读取内容的字符串
    string line;
    // 逐行读取文件内容
    while (getline(file, line))
    {
        if (line.empty())
            continue;

        // 解析每行数据到User对象
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

/**
 * @brief 显示所有用户信息
 *
 */
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