#include <iostream>
#include <string>
#include <limits>
#include <locale>
#include <vector>
#include <algorithm>
#include <functional>
#include "LabManager.h"
#include "User.h"

using namespace std;

// 权限检查辅助函数
bool checkLoggedIn(bool loggedIn)
{
    if (!loggedIn)
    {
        cout << "请先登录！\n";
        return false;
    }
    return true;
}

bool checkAdminPermission(User *user)
{
    if (!user->isAdminUser())
    {
        cout << "您没有管理员权限！\n";
        return false;
    }
    return true;
}

// 菜单结构体定义
struct MenuItem
{
    int id;
    string name;
    bool requireLogin;
    bool requireAdmin;
    function<void(LabManager &, User *)> action;
};

// 显示菜单
void showMenu(bool loggedIn, bool isAdmin, const string &userName, const vector<MenuItem> &items)
{
    cout << "\n=== 研究生工作管理系统 ===" << endl;
    if (!loggedIn)
    {
        cout << "1. 登录系统\n";
        cout << "0. 退出系统\n";
    }
    else
    {
        cout << "欢迎回来，" << userName << " (" << (isAdmin ? "管理员" : "学生") << ")\n";
        for (const auto &item : items)
        {
            cout << item.id << ". " << item.name << "\n";
        }
    }
    cout << "------------------------" << endl;
    cout << "请输入你的选择: ";
}

// 生成菜单选项
vector<MenuItem> getMenuItems(bool isAdmin)
{
    vector<MenuItem> items;

    if (isAdmin)
    {
        items = {
            {1, "查询人员", true, false, [](LabManager &m, User *)
             {
                 string id;
                 cout << "请输入要查询的学号: ";
                 getline(cin, id);
                 User *user = m.queryUser(id);
                 if (user)
                     user->displayInfo();
                 else
                     cout << "未找到该学生信息！\n";
             }},
            {2, "增加人员", true, true, [](LabManager &m, User *)
             {
                 User newUser;
                 newUser.inputInfo();
                 string defaultPassword = "123456";
                 newUser.setPassword(defaultPassword);
                 if (m.addUser(newUser))
                 {
                     cout << "添加成功！默认密码: " << defaultPassword << endl;
                 }
                 else
                 {
                     cout << "添加失败！学号已存在！\n";
                 }
             }},
            {3, "删除人员", true, true, [](LabManager &m, User *)
             {
                 string id;
                 cout << "请输入要删除的学号: ";
                 getline(cin, id);
                 if (m.deleteUser(id))
                 {
                     cout << "删除成功！\n";
                 }
                 else
                 {
                     cout << "删除失败！未找到该用户！\n";
                 }
             }},
            {4, "修改人员", true, true, [](LabManager &m, User *)
             {
                 string id;
                 cout << "请输入要修改的学号: ";
                 getline(cin, id);
                 User newInfo;
                 cout << "请输入新的信息（不修改的项直接回车）:\n";
                 newInfo.inputInfo(true);
                 if (m.updateUser(id, newInfo))
                 {
                     cout << "修改成功！\n";
                 }
                 else
                 {
                     cout << "修改失败！未找到该用户！\n";
                 }
             }},
            {5, "排序人员", true, true, [](LabManager &m, User *)
             {
                 m.sortByID();
                 cout << "已按学号排序！\n";
                 m.displayAllUsers(); // 添加此行以显示排序后的信息
             }},
            {6, "显示所有人员", true, true, [](LabManager &m, User *)
             {
                 m.displayAllUsers();
             }},
            {0, "退出登录", true, false, [](LabManager &, User *) {}}};
    }
    else
    {
        items = {
            {1, "查询人员", true, false, [](LabManager &m, User *)
             {
                 string id;
                 cout << "请输入要查询的学号: ";
                 getline(cin, id);
                 User *user = m.queryUser(id);
                 if (user)
                     user->displayInfo();
                 else
                     cout << "未找到该学生信息！\n";
             }},
            {2, "修改个人信息", true, false, [](LabManager &m, User *u)

             {
                 string id = u->getID();
                 User newInfo;
                 cout << "请输入新的信息（不修改的项直接回车）:\n";
                 newInfo.inputInfo(true);
                 if (m.updateUser(id, newInfo))
                 {
                     cout << "修改成功！\n";
                 }
                 else
                 {
                     cout << "修改失败！\n";
                 }
             }},
            {0, "退出登录", true, false, [](LabManager &, User *) {}}};
    }
    return items;
}

// 登录处理函数
void handleLogin(LabManager &manager, string &username, string &password, bool &loggedIn)
{
    cout << "请输入用户名: ";
    getline(cin, username);
    cout << "请输入密码: ";
    getline(cin, password);

    loggedIn = manager.login(username, password);
    if (loggedIn)
    {
        cout << "登录成功！\n";
    }
    else
    {
        cout << "用户名或密码错误！\n";
    }
}

int main()
{
    // 设置中文环境
    // std::locale::global(std::locale("Chinese (Simplified)_China.65001"));

    LabManager manager;
    int choice;
    string username, password;
    bool loggedIn = false;
    User *currentUser = nullptr;

    while (true)
    {
        vector<MenuItem> menuItems;
        if (loggedIn)
        {
            currentUser = manager.getCurrentUser();
            bool isAdmin = currentUser->isAdminUser();
            menuItems = getMenuItems(isAdmin);
            showMenu(true, isAdmin, currentUser->getName(), menuItems);
        }
        else
        {
            showMenu(false, false, "", menuItems);
        }

        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 清除输入缓冲区

        if (!loggedIn)
        {
            switch (choice)
            {
            case 1:
                handleLogin(manager, username, password, loggedIn);
                break;
            case 0:
                manager.saveToFile("data.dat");
                return 0;
            default:
                cout << "无效的选择，请重新输入！\n";
            }
        }
        else
        {
            auto it = find_if(menuItems.begin(), menuItems.end(),
                              [choice](const MenuItem &item)
                              { return item.id == choice; });

            if (it != menuItems.end())
            {
                if (it->requireLogin && !checkLoggedIn(loggedIn))
                    continue;
                if (it->requireAdmin && !checkAdminPermission(currentUser))
                    continue;

                if (choice == 0)
                {
                    // 退出登录
                    loggedIn = false;
                    cout << "已退出登录！\n";
                }
                else
                {
                    it->action(manager, currentUser);
                }
            }
            else
            {
                cout << "无效的选择，请重新输入！\n";
            }
        }
    }
}