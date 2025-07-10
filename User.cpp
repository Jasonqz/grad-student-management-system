#include "User.h"
#include <iomanip>
#include <windows.h>
#include "StringUtils.h"

// 用户基类实现
User::User(string id, string name, string gender, string major, int year, string contact,
           string uname, string pwd, bool admin)
    : studentID(id), name(name), gender(gender), major(major),
      enrollmentYear(year), contactInfo(contact),
      username(uname), password(pwd), isAdmin(admin) {}

string User::getID() const { return studentID; }
string User::getName() const { return name; }
string User::getGender() const { return gender; }
string User::getMajor() const { return major; }
int User::getEnrollmentYear() const { return enrollmentYear; }
string User::getContactInfo() const { return contactInfo; }
string User::getUsername() const { return username; }
string User::getPassword() const { return password; }
bool User::checkPassword(string pwd) const { return password == pwd; }
bool User::isAdminUser() const { return isAdmin; }

void User::setID(string id) { studentID = id; }
void User::setName(string name) { this->name = name; }
void User::setGender(string gender) { this->gender = gender; }
void User::setMajor(string major) { this->major = major; }
void User::setEnrollmentYear(int year) { enrollmentYear = year; }
void User::setContactInfo(string contact) { contactInfo = contact; }
void User::setUsername(string uname) { username = uname; }
void User::setPassword(string pwd) { password = pwd; }
void User::setAdmin(bool admin) { isAdmin = admin; }

void User::inputInfo(bool partial, bool isAdminMode)
{
    string temp;
    int num;

    // 仅在新增模式(partial=false)下显示学号输入
    if (!partial)
    {
        cout << "学号: ";
        getline(cin, studentID);

        // 如果是新增用户，默认用户名和学号相同
        if (username.empty())
        {
            username = studentID;
        }
    }

    if (!partial || name.empty())
    {
        cout << "姓名: ";
        getline(cin, name);
    }

    if (!partial || gender.empty())
    {
        cout << "性别: ";
        getline(cin, gender);
    }

    if (!partial || major.empty())
    {
        cout << "专业: ";
        getline(cin, major);
    }

    if (!partial || enrollmentYear == 0)
    {
        cout << "入学年份: ";
        string yearInput;
        getline(cin, yearInput);
        yearInput = StringUtils::trim(yearInput); // 使用现有trim方法去除空白
        if (!yearInput.empty())
        {
            if (StringUtils::isValidUserID(yearInput))
            {
                enrollmentYear = stoi(yearInput);
            }
            else
            {
                cout << "无效的年份输入！请输入数字年份。" << endl;
                // 添加循环直到获取有效输入
                while (true)
                {
                    cout << "入学年份: ";
                    getline(cin, yearInput);
                    yearInput = StringUtils::trim(yearInput);
                    if (yearInput.empty() || StringUtils::isValidUserID(yearInput))
                    {
                        if (!yearInput.empty())
                        {
                            enrollmentYear = stoi(yearInput);
                        }
                        break;
                    }
                    cout << "无效的年份输入！请输入数字年份。" << endl;
                }
            }
        }
    }

    if (!partial || contactInfo.empty())
    {
        cout << "联系方式: ";
        getline(cin, contactInfo);
    }

    // 管理员可以设置密码
    // if (isAdminMode && (!partial || password.empty()))
    // {
    //     cout << "密码: ";
    //     getline(cin, password);
    // }
    // 用户名密码修改选项（仅在修改模式下）
    if (partial)
    {
        cout << "用户名(不修改请留空): ";
        string newUsername;
        getline(cin, newUsername);
        if (!newUsername.empty())
        {
            username = newUsername;
        }

        cout << "密码(不修改请留空): ";
        string newPassword;
        getline(cin, newPassword);
        if (!newPassword.empty())
        {
            password = newPassword;
        }
    }
    // // 允许所有用户修改自己的密码（修改模式下）
    // if (partial)
    // {
    //     }

    // 新增：管理员修改模式下显示角色修改选项
    if (isAdminMode || !partial)
    {
        cout << "是否管理员(1-是, 0-否，其他-否 不修改请留空): ";
        string adminInput;
        getline(cin, adminInput);
        if (!adminInput.empty())
        {
            isAdmin = (adminInput == "1");
        }
    }
}

/**
 * @brief 显示用户信息
 *
 */
void User::displayInfo() const
{
    cout << left << setw(12) << "学号: " << studentID << endl;
    cout << left << setw(12) << "姓名: " << name << endl;
    cout << left << setw(12) << "性别: " << gender << endl;
    cout << left << setw(12) << "专业: " << major << endl;
    cout << left << setw(12) << "入学年份: " << enrollmentYear << endl;
    cout << left << setw(12) << "联系方式: " << contactInfo << endl;
    cout << left << setw(12) << "用户名: " << username << endl;
    cout << left << setw(12) << "角色: " << (isAdmin ? "管理员" : "学生") << endl;
}

/**
 * @brief 权限校验
 *
 * @param id 学号
 * @return true 权限校验通过
 */
bool User::canModifySelf(string id) const
{
    return (studentID == id);
}
/**
 * @brief 管理员权限校验
 *
 * @return true 权限校验通过
 * @return false 权限校验失败
 */
bool User::canModifyAll() const
{
    return isAdmin; // 直接返回isAdmin成员变量
}

/**
 * @brief 输出操作运算符重载
 *      这个重载函数决定了对象数据如何被格式化为文本流
 * @param os 输出流对象
 * @param user 要输出的用户对象
 * @return ostream& 输出流对象
 */
ostream &operator<<(ostream &os, const User &user)
{
    os << user.studentID << "\t"
       << user.name << "\t"
       << user.gender << "\t"
       << user.major << "\t"
       << user.enrollmentYear << "\t"
       << user.contactInfo << "\t"
       << user.username << "\t"
       << user.password << "\t"
       << user.isAdmin;
    return os;
}
/**
 * @brief 输入操作运算符重载
 * 从输入流(如文件、控制台)读取格式化数据并构造 User 对象。
 * 重载 >> 运算符，接受输入流 is 和 User 对象引用 user ，返回输入流引用以支持链式调用
 * @param is 输入流对象
 * @param user 要输入的用户对象
 * @return istream& 输入流对象
 */
istream &operator>>(istream &is, User &user)
{
    getline(is, user.studentID, '\t'); // 读取到制表符\t为止，存入studentID
    getline(is, user.name, '\t');
    getline(is, user.gender, '\t');
    getline(is, user.major, '\t');

    string yearStr;
    getline(is, yearStr, '\t');
    user.enrollmentYear = stoi(yearStr);

    getline(is, user.contactInfo, '\t');
    getline(is, user.username, '\t');
    getline(is, user.password, '\t');

    string adminStr;
    getline(is, adminStr);
    user.isAdmin = (adminStr == "1");

    return is;
}

/**
 * @brief 析构函数
 *
 */
User::~User()
{
    // 析构函数逻辑（如果需要）
}