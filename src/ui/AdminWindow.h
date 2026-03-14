#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QWidget>
#include <QVariantList>
#include <QSqlQuery>
#include "../database/dbmanager.h"

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

private slots:
    // 航班管理相关槽函数
    void on_btnFlightQuery_clicked();
    void on_btnFlightAdd_clicked();
    void on_btnFlightEdit_clicked();
    void on_btnFlightDelete_clicked();
    void on_tableFlights_itemSelectionChanged();

    // 用户管理相关槽函数
    void on_btnUserQuery_clicked();
    void on_btnUserEdit_clicked();
    void on_btnUserResetPwd_clicked();
    void on_tableUsers_itemSelectionChanged();

    // 订单管理相关槽函数
    void on_btnOrderQuery_clicked();
    void on_btnOrderEdit_clicked();
    void on_tableOrders_itemSelectionChanged();

private:
    Ui::AdminWindow *ui;
    DBManager *m_dbManager; // 数据库管理实例

    // 初始化表格列名
    void initFlightTable();
    void initUserTable();
    void initOrderTable();

    // 填充表格数据
    void fillFlightTable(const QList<QVariantList> &data);
    void fillUserTable(const QList<QVariantList> &data);
    void fillOrderTable(const QList<QVariantList> &data);

    // 清空输入控件
    void clearFlightInputs();
    void clearUserInputs();
    void clearOrderInputs();

    // 校验输入合法性
    bool validateFlightInputs(bool isEdit = false);
    bool validateUserInputs();
    bool validateOrderInputs();
};

#endif // ADMINWINDOW_H
