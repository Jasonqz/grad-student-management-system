-- 创建数据库
CREATE DATABASE IF NOT EXISTS grad_student_management;
USE grad_student_management;

-- 创建学生表
-- CREATE TABLE IF NOT EXISTS students (
--     student_id INT PRIMARY KEY,
--     name VARCHAR(50) NOT NULL,
--     gender VARCHAR(10),
--     major VARCHAR(50),
--     enrollment_year INT,
--     contact_info VARCHAR(100),
--     username INT UNIQUE NOT NULL,
--     password VARCHAR(100) NOT NULL,
--     is_admin BOOLEAN DEFAULT FALSE,
--     created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
--     updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
-- );

-- 用户表：存储系统用户登录信息及基本资料
CREATE TABLE users (
    studentID VARCHAR(20) PRIMARY KEY AUTO_INCREMENT COMMENT '学号/工号，作为唯一标识',
    name VARCHAR(50) NOT NULL COMMENT '姓名',
    gender VARCHAR(10) COMMENT '性别',
    major VARCHAR(50) COMMENT '专业',
    enrollmentYear INT COMMENT '入学/入职年份',
    contactInfo VARCHAR(100) COMMENT '联系方式（电话/邮箱）',
    username VARCHAR(50) NOT NULL UNIQUE COMMENT '登录用户名，唯一',
    password VARCHAR(255) NOT NULL COMMENT '加密存储的密码',
    isAdmin BOOLEAN DEFAULT FALSE COMMENT '是否为管理员权限',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '记录创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '记录更新时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='系统用户表';