[TOC]

#### 一、项目介绍：

- 功能：基于QT客户端+Linux C服务端+MySQL数据库的学生档案管理系统
  - 管理员登录以及账号注册功能
  - 档案创建、档案查找、档案修改、档案删除、档案清空功能
- 技术点：
  - QT客户端
    - QT基本窗口组件
    - 信号和槽
    - lambda函数
    - 单例模式
    - HTTP请求报文发送与响应报文解析
  - Webserver服务端
    - 连接池
    - 线程池
    - 进程/线程通信和同步知识（互斥锁、信号量、管道等）
    - socket网络通信流程
    - C++ STL
    - 常用C字符串处理函数
    - HTTP报文解析与响应
  - MySQL数据库
    - 基本的SQL语句（建库、建表、增删改查）
    - CAPI

#### 二、快速上手：

##### 环境需求

- QT版本：Windows  5.3.1    用于编写和运行QT代码
- VMware：16.0.0   用于创建虚拟机搭载Linux系统
- Ubuntu版本：Linux  18.04   用于搭载webserver代码和数据库的Linux服务器
- GCC版本：Linux  7.5.0    用于编译C/C++的编译器
- MySQL版本：Linux   5.7.41-0ubuntu0.18.04.1   用于数据存取的数据库
- Xshell版本：7.0       用于远程连接Linux系统
- Xftp版本：7.0         用于直接在本机和虚拟机之间进行文件传输
- 代码编辑器：VS Code (关键扩展：Remote-SSH, C/C++, C/C++ Extension Pack)     用于连接服务器进行C/C++代码阅读和撰写

##### 调试准备

- WebServer部分：
  - 确认Linux系统上已安装MySQL数据库
  
    - [Linux系统安MySQL数据库](https://blog.csdn.net/weixin_44178960/article/details/130002753)
    - MySQL数据库建表

    ```mysql
    // 建立数据库,dbname换成自己想要建立数据库的名称
    create database dbname;
    
    // 创建user表（用于存储学生档案管理员账号信息）
    USE dbname;
    CREATE TABLE user(
        username char(50) NULL,
        passwd char(50) NULL
    )ENGINE=InnoDB;
    
    // 添加数据（在'name'和'passwd'中自己设定默认管理员和密码）
    INSERT INTO user(username, passwd) VALUES('name', 'passwd');
    
    // 创建studentInfo表（用于存储学生档案信息）
    CREATE TABLE studentInfo(
        name char(50) NULL,
        id char(50) NULL,
        birthday char(50) NULL,
        birthplace char(50) NULL
    )ENGINE=InnoDB;
    ```
  
    - 数据库编码格式修改（统一utf-8，否则中文会出现乱码）
  
    ```mysql
    ALTER DATABASE database_name CHARACTER SET utf8 COLLATE utf8_unicode_ci;  // database_name 改成自己的数据库名称
    ```
  
  - 修改main.cpp中的数据库初始化信息
  
  ```c++
  //数据库登录名,密码,数据库名称
  string user = "root";
  string passwd = "passwd";
  string databasename = "dbname";
  ```
  
  - build编译代码
  
  ```bash
  sh ./build.sh
  ```
  
  - 启动server
  
  ```bash
  ./server
  ```
  
- QT部分：

  - 用QT Creator打开QT部分代码中的01_First_Project.pro文件，ctrl+R即可运行客户端代码
  
  ```c++
  // 修改tcpclient.cpp中的服务器ip和端口号（改成自己webserver的ip和端口号）
  ip = "192.168.59.128";
  port = 9006;
  ```
  
  - 启动server端之后，在登录界面中输入user表中的默认管理员和密码执行登录操作进入主界面
  
  <img src="./%E5%9B%BE%E7%89%87%E5%BA%93/image-20230407014222328.png" alt="image-20230407014222328" style="zoom:80%;" />

##### 个性化运行

```C++
./server [-p port] [-l LOGWrite] [-m TRIGMode] [-o OPT_LINGER] [-s sql_num] [-t thread_num] [-c close_log] [-a actor_model]
```

温馨提示:以上参数不是非必须，不用全部使用，根据个人情况搭配选用即可.

* -p，自定义端口号
  * 默认9006
* -l，选择日志写入方式，默认同步写入
  * 0，同步写入
  * 1，异步写入
* -m，listenfd和connfd的模式组合，默认使用LT + LT
  * 0，表示使用LT + LT
  * 1，表示使用LT + ET
    * 2，表示使用ET + LT
    * 3，表示使用ET + ET
* -o，优雅关闭连接，默认不使用
  * 0，不使用
  * 1，使用
* -s，数据库连接数量
  * 默认为8
* -t，线程数量
  * 默认为8
* -c，关闭日志，默认打开
  * 0，打开日志
  * 1，关闭日志
* -a，选择反应堆模型，默认Proactor
  * 0，Proactor模型
  * 1，Reactor模型

测试示例命令与含义

```C++
./server -p 9007 -l 1 -m 0 -o 1 -s 10 -t 10 -c 1 -a 1
```



#### 三、注意事项：

- 由于数据中有中文字符，因此需要对项目中的数据传输和存储进行统一编码，项目中的数据编码格式为UTF-8
- 项目中不同类型之间的数据用&作为分隔符
- webserver部分的代码，除了HTTP报文解析和响应部分根据程序功能重写，其余代码均来自于[Linux下C++轻量级Web服务器开源代码](https://github.com/qinguoyi/TinyWebServer)
- QT部分的代码全部自己手写，UI设计部分为了练手没有采用QT Creator自带的手动UI编辑器，全部用QT代码完成，因此代码不是很简洁，界面设计部分的代码可以忽略，重点是信号和槽的使用以及客户端与服务器之间的数据交互方式
- 采用HTTP明文的方式进行数据交互，安全性方面需要改进

#### 四、相关知识学习：

- QT部分的知识来自于B站视频[最新QT从入门到实战完整版](https://www.bilibili.com/video/BV1g4411H78N)
- Webserver环境搭建、C/C++服务器编程知识来自于：
  - 牛客网视频[Linux高并发服务器开发](https://www.nowcoder.com/study/live/504/intro)
  - 《UNIX环境高级编程第三版》
- MySQL部分知识来自于B站视频[MySQL数据库入门到精通](https://www.bilibili.com/video/BV1Kr4y1i7ru)
- 项目编程和源码阅读过程中的笔记如下：
  - [QT基础](https://blog.csdn.net/weixin_44178960/article/details/130002647)
  - [常用C字符串相关函数](https://blog.csdn.net/weixin_44178960/article/details/129895968)
  - [Linux系统中MySQL数据库安装](https://blog.csdn.net/weixin_44178960/article/details/130002753)
  - [C语言操作MySQL数据库常用函数](https://blog.csdn.net/weixin_44178960/article/details/130002904)



