#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "../models/usersession.h" // 【新增】引入 UserSession
#include "../models/user.h"        // 【新增】引入 User 模型

// 构造函数
LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    // 1. 初始化DBManager（和注册窗口一致，填你的MySQL信息）
    m_dbManager = new DBManager("flight_ticket_system", "root", "Saythename_17"); // 改成你的用户名和密码！
    m_dbManager->connect();

    // 2. 初始化各个窗口
    m_regWindow = new RegisterWindow();
    m_mainWindow = new MainWindow();
    m_adminWindow = new AdminWindow();

    // 3. 关联注册窗口的信号：注册完成后显示登录窗口
    connect(m_regWindow, &RegisterWindow::registerFinished, this, [=]() {
        this->show();
    });
}

// 析构函数
LoginWindow::~LoginWindow()
{
    m_dbManager->disconnect();
    delete m_dbManager;
    delete m_regWindow;
    delete m_mainWindow;
    delete m_adminWindow;
    delete ui;
}

// 【核心修改】验证用户并载入 Session
QString LoginWindow::verifyUser(const QString &username, const QString &password)
{
    // 1. 修改SQL：查询 id, username, password, email, role
    // 注意：SELECT 的顺序必须与下面解析的顺序一致
    QString sql = QString("SELECT id, username, password, email, role FROM users WHERE username = '%1' AND password = '%2'")
                      .arg(username)
                      .arg(password);

    // 2. 执行SQL查询
    QSqlQuery query = m_dbManager->executeQuery(sql);
    QList<QVariantList> results = m_dbManager->fetchResults(query);

    // 3. 判断是否查询到用户
    if (!results.isEmpty()) {
        QVariantList row = results.first();
        
        // 解析数据 (索引对应 SQL 中的字段顺序)
        int userId = row[0].toInt();
        QString dbName = row[1].toString();
        QString dbPass = row[2].toString();
        QString dbEmail = row[3].toString();
        QString dbRole = row[4].toString();

        // 【关键步骤】将用户信息存入全局单例 UserSession
        User user(dbName, dbPass, dbEmail);
        UserSession::instance().setUser(user, userId);

        return dbRole; // 返回角色供按钮逻辑判断
    } else {
        return "";
    }
}

// 按钮1：新用户注册
void LoginWindow::on_loginButton_2_clicked()
{
    this->hide();
    m_regWindow->show();
}

// 按钮2：用户登录
void LoginWindow::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    QString role = verifyUser(username, password);

    if (role == "customer") {
        QMessageBox::information(this, "登录成功", "欢迎回来，用户 " + username);
        ui->usernameLineEdit->clear();
        ui->passwordLineEdit->clear();
        this->hide();
        m_mainWindow->show();
    } else {
        // 如果验证失败或角色不对，清除可能误存的 Session
        UserSession::instance().clear();
        QMessageBox::warning(this, "登录失败", "用户未注册或账号密码错误！");
    }
}

// 按钮3：管理员登录
void LoginWindow::on_loginButton_3_clicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    QString role = verifyUser(username, password);

    if (role == "admin") {
        QMessageBox::information(this, "登录成功", "欢迎回来，管理员 " + username);
        ui->usernameLineEdit->clear();
        ui->passwordLineEdit->clear();
        this->hide();
        m_adminWindow->show();
    } else {
        // 如果验证失败或角色不对，清除可能误存的 Session
        UserSession::instance().clear();
        QMessageBox::warning(this, "登录失败", "管理员账号不存在或密码错误！");
    }
}