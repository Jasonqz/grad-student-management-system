#include "LabManager.h"
#include <algorithm>
#include <sstream> // 添加此行以使用istringstream

LabManager::LabManager() : head(nullptr), currentUser(nullptr)
{
    // 从文件加载数据
    loadFromFile("data.dat");
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
        if (!newInfo.checkPassword(""))
            node->data.setPassword(newInfo.checkPassword("") ? newInfo.getUsername() : "");
        if (newInfo.isAdminUser() != node->data.isAdminUser())
            node->data.setAdmin(newInfo.isAdminUser());
    }

    return true;
}

void LabManager::sortByID()
{
    if (!head || !head->next)
        return;

    Node *dummy = new Node(User()); // 创建哑节点
    Node *sorted = dummy; // 已排序部分的尾指针
    Node *current = head; // 当前待插入节点

    while (current)
    {
        Node *next = current->next; // 保存下一个节点
        Node *prev = dummy; // 用于遍历已排序部分

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