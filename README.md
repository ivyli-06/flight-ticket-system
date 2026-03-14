# 航班票务管理系统

## 项目简介
本项目是一个基于Qt和MySQL的航班票务管理系统，旨在提供用户友好的界面以管理航班信息和订单。系统支持用户登录、航班查询和订单管理功能。

## 功能
- **用户登录**：支持管理员和普通用户登录。
- **航班查询**：用户可以根据出发地、目的地和日期查询航班信息。
- **订单管理**：用户可以查看自己的订单并取消订单。

## 项目结构
```
flight-ticket-system
├── src
│   ├── main.cpp                  # 应用程序入口点
│   ├── database
│   │   ├── dbmanager.h           # 数据库管理类头文件
│   │   └── dbmanager.cpp         # 数据库管理类实现文件
│   ├── models
│   │   ├── user.h                # 用户模型头文件
│   │   ├── flight.h              # 航班模型头文件
│   │   └── order.h               # 订单模型头文件
│   └── ui
│       ├── loginwindow.h         # 登录界面头文件
│       ├── loginwindow.cpp       # 登录界面实现文件
│       ├── loginwindow.ui        # 登录界面UI文件
│       ├── flightsearchwindow.h   # 航班查询界面头文件
│       ├── flightsearchwindow.cpp # 航班查询界面实现文件
│       ├── flightsearchwindow.ui  # 航班查询界面UI文件
│       ├── ordermanagerwindow.h   # 订单管理界面头文件
│       ├── ordermanagerwindow.cpp # 订单管理界面实现文件
│       └── ordermanagerwindow.ui  # 订单管理界面UI文件
├── flight-ticket-system.pro       # Qt项目文件
└── README.md                     # 项目文档
```

## 使用说明
1. 确保已安装Qt和MySQL数据库。
2. 配置数据库连接信息。
3. 使用Qt Creator打开项目文件`flight-ticket-system.pro`。
4. 编译并运行项目。

## 依赖
- Qt 5.x 或更高版本
- MySQL 5.x 或更高版本

## 贡献
欢迎任何形式的贡献！请提交问题或拉取请求。