#include <iostream>
#include <cassert>
#include "DBConnector.h"

// 数据库连接配置（请根据实际环境修改）
const std::string DB_IP = "localhost";
const std::string DB_USER = "root";
const std::string DB_PASS = "qz284781";
const std::string DB_NAME = "grad_student_management";
const int DB_PORT = 3306;

// 测试工具类
class DBTest
{
private:
    DBConnector db;
    bool isConnected = false;

public:
    // 测试数据库连接
    bool testConnect()
    {
        std::cout << "[测试] 数据库连接..." << std::endl;
        isConnected = db.connect(DB_IP, DB_USER, DB_PASS, DB_NAME, DB_PORT);
        assert(isConnected && "数据库连接失败！");
        std::cout << "[成功] 数据库连接正常\n"
                  << std::endl;
        return isConnected;
    }

    // 测试获取表字段数
    void testGetTableField()
    {
        if (!isConnected)
        {
            std::cout << "[跳过] 请先确保连接成功\n"
                      << std::endl;
            return;
        }

        std::cout << "[测试] 获取表字段数..." << std::endl;
        // 假设存在users表
        int fieldCount = db.getTableField("students");
        assert(fieldCount > 0 && "获取表字段失败！");
        std::cout << "[成功] 表字段数: " << fieldCount << "\n"
                  << std::endl;
    }

    // 测试查询表数据
    void testQueryTable()
    {
        if (!isConnected)
        {
            std::cout << "[跳过] 请先确保连接成功\n"
                      << std::endl;
            return;
        }

        std::cout << "[测试] 查询表数据..." << std::endl;
        // 假设存在users表
        std::string result = db.query("students");
        assert(!result.empty() && "查询表数据失败！");
        std::cout << "[成功] 查询结果:\n"
                  << result << std::endl;
    }

    // 测试执行SQL命令
    void testExecuteSQL()
    {
        if (!isConnected)
        {
            std::cout << "[跳过] 请先确保连接成功\n"
                      << std::endl;
            return;
        }

        std::cout << "[测试] 执行SQL命令..." << std::endl;
        // 创建测试表
        bool createSuccess = db.implement(
            "CREATE TABLE IF NOT EXISTS students (id INT, name VARCHAR(20))");
        assert(createSuccess && "创建表失败！");

        // 插入测试数据
        bool insertSuccess = db.implement(
            "INSERT INTO students (id, name) VALUES (1, 'test')");
        assert(insertSuccess && "插入数据失败！");

        // 查询验证
        std::string result = db.query("students");
        assert(result.find("1\ttest\t") != std::string::npos && "查询不到插入的数据！");

        // 清理测试表
        db.implement("DROP TABLE test_table");
        std::cout << "[成功] SQL命令执行正常\n"
                  << std::endl;
    }

    // 运行所有测试
    void runAllTests()
    {
        std::cout << "=== 数据库测试套件开始 ===\n"
                  << std::endl;
        testConnect();
        testGetTableField();
        testQueryTable();
        testExecuteSQL();
        std::cout << "\n=== 所有测试完成 ===" << std::endl;
    }
};

int main()
{
    // 初始化MySQL库
    mysql_library_init(0, nullptr, nullptr);

    DBTest test;
    test.runAllTests();

    // 清理MySQL库
    mysql_library_end();
    return 0;
}