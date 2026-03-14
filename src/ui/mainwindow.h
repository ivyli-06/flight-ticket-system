#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QList>
#include <QVariantList>
#include <QDate>
#include <QStringList>  // 新增：用于城市列表（可选，也可放cpp里）
#include <QComboBox>    // 新增：可选，明确用到的控件类型
#include "../database/dbmanager.h"
#include "bookTicketConfirm.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnMyOrders_clicked();
    void on_btnQuerySearch_clicked();
    void on_btnRefresh_clicked();

    void on_btnReset_clicked();
    void on_btnSwap_clicked();

    void on_btnBookTicket_clicked();

private:
    Ui::MainWindow *ui;
    DBManager *dbManager;
    bool isSearching;

    // 初始化航班表格（设置列名、列宽等）
    void initFlightTable();
    // 新增：初始化出发地/目的地城市下拉框
    void initCityComboBoxes();
    //载入所有航班信息
    void loadAllFlights();
    // 填充航班表格（将数据库查询结果加载到QTableWidget）
    void fillFlightTable(const QList<QVariantList> &flightData);
    // 构造查询SQL语句（实现宽松查找 + 时间精确匹配）
    QString buildQuerySql();
};

#endif // MAINWINDOW_H
