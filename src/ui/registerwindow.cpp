#include "registerwindow.h"
#include "ui_registerwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>

RegisterWindow::RegisterWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterWindow)
{
    ui->setupUi(this);
    // 初始化DBManager（替换为你的MySQL用户名、密码、ODBC数据源名）
    m_dbManager = new DBManager("flight_ticket_system", "root", "Saythename_17");
    // 连接数据库
    m_dbManager->connect();
}

RegisterWindow::~RegisterWindow()
{
    if (m_dbManager->db.isOpen()) {
        m_dbManager->db.close();
    }
    delete m_dbManager;
    delete ui;
}

void RegisterWindow::on_submitRegisterButton_clicked()
{
    // 1. 获取输入内容（去空格）
    QString username = ui->usernameLineEdit->text().trimmed();
    QString email = ui->emailLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    // 2. 检查输入非空
    if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "用户名、邮箱、密码不能为空！");
        return;
    }

    // 3. 参数绑定方式执行注册SQL（彻底避免拼接错误）
    QSqlQuery query(m_dbManager->db); // 绑定当前数据库连接
    query.prepare("INSERT INTO users (username, password, email, role, created_at) "
                  "VALUES (:username, :password, :email, 'customer', NOW())");
    // 绑定参数
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":email", email);

    // 4. 执行并处理结果
    if (query.exec()) {
        // 注册成功
        QMessageBox::information(this, "注册成功", "注册成功！即将返回登录页");
        // 清空输入
        ui->usernameLineEdit->clear();
        ui->emailLineEdit->clear();
        ui->passwordLineEdit->clear();
        // 通知登录窗口
        emit registerFinished();
        this->close();
    } else {
        // 注册失败：打印真实错误到控制台
        qDebug() << "注册失败原因：" << query.lastError().text();
        // 弹窗显示具体错误
        QMessageBox::critical(this, "注册失败",
                              "注册失败！错误原因：\n" + query.lastError().text());
    }
}
