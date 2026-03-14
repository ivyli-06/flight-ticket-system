#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QMessageBox>
#include "src/database/dbmanager.h" // 你的DBManager头文件
#include "registerwindow.h" // 注册窗口
#include "mainwindow.h"     // 用户主窗口（队友负责，无需改动）
#include "adminwindow.h"    // 管理员窗口（队友负责，无需改动）

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    // 三个按钮的点击事件（必须和ui文件中的按钮名一致）
    void on_loginButton_2_clicked(); // 新用户注册（loginButton_2）
    void on_loginButton_clicked();   // 用户登录（loginButton）
    void on_loginButton_3_clicked(); // 管理员登录（loginButton_3）

    // 辅助：验证用户名密码，返回用户角色
    QString verifyUser(const QString& username, const QString& password);

private:
    Ui::LoginWindow *ui;
    DBManager* m_dbManager; // 你的DBManager实例
    RegisterWindow* m_regWindow; // 注册窗口
    MainWindow* m_mainWindow;     // 用户主窗口
    AdminWindow* m_adminWindow;   // 管理员窗口
};

#endif // LOGINWINDOW_H
