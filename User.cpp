#include "User.h"
#include <iomanip>

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

void User::inputInfo(bool partial)
{
    string temp;
    int num;

    if (!partial || studentID.empty())
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
        cin >> enrollmentYear;
        cin.ignore(); // 清除缓冲区的换行符
    }

    if (!partial || contactInfo.empty())
    {
        cout << "联系方式: ";
        getline(cin, contactInfo);
    }

    // 管理员可以设置密码
    if (isAdmin && (!partial || password.empty()))
    {
        cout << "密码: ";
        getline(cin, password);
    }
    // 允许所有用户修改自己的密码（修改模式下）
    if (partial)
    {
        cout << "密码(不修改请留空): ";
        string newPassword;
        getline(cin, newPassword);
        if (!newPassword.empty())
        {
            password = newPassword;
        }
    }
}

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

bool User::canModifySelf(string id) const
{
    return (studentID == id);
}

bool User::canModifyAll() const
{
    return isAdmin; // 直接返回isAdmin成员变量
}

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

istream &operator>>(istream &is, User &user)
{
    getline(is, user.studentID, '\t');
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

// 析构函数实现
User::~User()
{
    // 析构函数逻辑（如果需要）
}