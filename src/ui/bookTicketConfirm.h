#ifndef BOOKTICKETCONFIRM_H
#define BOOKTICKETCONFIRM_H

#include <QWidget>
#include <QDateTime>
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include "../database/dbmanager.h"

namespace Ui {
class bookTicketConfirm;
}

class bookTicketConfirm : public QWidget
{
    Q_OBJECT

public:
    explicit bookTicketConfirm(QWidget *parent = nullptr);
    ~bookTicketConfirm();

    // 设置航班信息（用于外部传入航班ID/航班号等初始化界面）
    void setFlightInfo(int flightId, const QString& flightNumber = "");

private slots:
    // 确认购票按钮点击事件
    void on_confirmButton_clicked();
    // 取消按钮点击事件
    void on_cancelButton_clicked();
    // 计算总价（票数变化时触发）
    void calculateTotalPrice();

private:
    Ui::bookTicketConfirm *ui;
    int m_flightId;          // 当前选中航班ID
    double m_singlePrice;    // 单张机票价格
    DBManager *dbManager; // 数据库管理实例
    // 从数据库加载航班详情到界面
    bool loadFlightDetails(int flightId);
};

#endif // BOOKTICKETCONFIRM_H
