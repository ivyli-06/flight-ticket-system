#ifndef ORDERMANAGERWINDOW_H
#define ORDERMANAGERWINDOW_H

#include <QWidget>
#include "../database/dbmanager.h"

namespace Ui {
class OrderManagerWindow;
}

class OrderManagerWindow : public QWidget {
    Q_OBJECT

public:
    // 构造函数：默认使用 UserSession 获取 ID，也可以传参
    explicit OrderManagerWindow(QWidget *parent = nullptr);
    ~OrderManagerWindow();

private slots:
    void on_cancelButton_clicked();
    void on_btnRefresh_clicked(); // 新增刷新按钮槽
    void on_btnDeleteOrder_clicked();

private:
    Ui::OrderManagerWindow *ui;
    DBManager *dbManager;
    
    void loadOrders();
};

#endif // ORDERMANAGERWINDOW_H
