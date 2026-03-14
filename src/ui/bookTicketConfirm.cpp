#include "bookTicketConfirm.h"
#include "ui_bookTicketConfirm.h"
#include "../models/usersession.h"
#include <QSqlQuery>
#include <QVariant>
#include <QString>
#include <QMessageBox>
#include <QDebug>

bookTicketConfirm::bookTicketConfirm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::bookTicketConfirm),
    m_flightId(-1),
    m_singlePrice(0.0)
{
    ui->setupUi(this);
    dbManager = new DBManager("flight_ticket_system", "root", "Saythename_17");

    // 连接票数变化信号，自动计算总价
    connect(ui->spinBoxTicketCount, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &bookTicketConfirm::calculateTotalPrice);
}

bookTicketConfirm::~bookTicketConfirm()
{
    delete ui;
}

void bookTicketConfirm::setFlightInfo(int flightId, const QString &flightNumber)
{
    m_flightId = flightId;
    // 如果传入了航班号，先显示
    if (!flightNumber.isEmpty()) {
        ui->labelFlightNumber->setText(flightNumber);
    }
    // 从数据库加载完整航班详情
    loadFlightDetails(flightId);
}

bool bookTicketConfirm::loadFlightDetails(int flightId)
{
    if (flightId <= 0) {
        QMessageBox::warning(this, "警告", "无效的航班ID！");
        return false;
    }

    // 1. 初始化数据库连接
    if (!dbManager->connect()) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return false;
    }

    // 2. 构造查询航班详情的SQL语句
    QString queryStr = QString( "SELECT id, flight_number, departure, arrival, departure_time, "
                  "arrival_time, total_seats, available_seats, price, status, created_at "
                  "FROM flights WHERE id = %1").arg(flightId);

    // 3. 执行查询
    QSqlQuery query = dbManager->executeQuery(queryStr);
    QList<QVariantList> results = dbManager->fetchResults(query);

    // 4. 处理查询结果
    if (results.isEmpty()) {
        QMessageBox::warning(this, "提示", "未查询到该航班的详细信息！");
        return false;
    }

    // 提取航班信息（对应SQL查询的字段顺序）
    QVariantList flightData = results.first();
    QString flightNum = flightData.at(1).toString();
    QString departure = flightData.at(2).toString();
    QString arrival = flightData.at(3).toString();
    QString departTime = flightData.at(4).toString();
    QString arriveTime = flightData.at(5).toString();
    m_singlePrice = flightData.at(8).toDouble();
    int availableSeats = flightData.at(7).toInt();

    // 5. 更新UI界面显示
    ui->labelFlightNumber->setText(flightNum);
    ui->labelRoute->setText(QString("%1 -> %2").arg(departure).arg(arrival));
    ui->labelDepartureTime->setText(departTime);
    ui->labelArrivalTime->setText(arriveTime);
    ui->labelPrice->setText(QString("¥%1").arg(m_singlePrice, 0, 'f', 2));
    ui->labelAvailableSeats->setText(QString::number(availableSeats));

    // 6. 更新票数选择框的最大值（不能超过剩余座位数）
    ui->spinBoxTicketCount->setMaximum(qMin(10, availableSeats));
    // 初始化总价
    calculateTotalPrice();

    return true;
}

void bookTicketConfirm::calculateTotalPrice()
{
    int ticketCount = ui->spinBoxTicketCount->value();
    double totalPrice = m_singlePrice * ticketCount;
    ui->labelTotalPrice->setText(QString("¥%1").arg(totalPrice, 0, 'f', 2));
}

void bookTicketConfirm::on_confirmButton_clicked()
{
    // 1. 验证输入信息
    QString passengerName = ui->lineEditPassengerName->text().trimmed();
    QString idCard = ui->lineEditIDCard->text().trimmed();
    QString phone = ui->lineEditPhone->text().trimmed();
    QString email = ui->lineEditEmail->text().trimmed();
    int ticketCount = ui->spinBoxTicketCount->value();

    if (passengerName.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入乘客姓名！");
        ui->lineEditPassengerName->setFocus();
        return;
    }
    if (idCard.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入身份证号！");
        ui->lineEditIDCard->setFocus();
        return;
    }
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入手机号！");
        ui->lineEditPhone->setFocus();
        return;
    }
    if (m_flightId <= 0) {
        QMessageBox::warning(this, "提示", "请先选择有效航班！");
        return;
    }

    // 2. 获取总价
    double totalAmount = m_singlePrice * ticketCount;

    // 3. 数据库操作（创建订单）
    if (!dbManager->connect()) {
        QMessageBox::critical(this, "错误", "数据库连接失败，无法提交订单！");
        return;
    }

    // 1. 获取UserSession全局单例
    UserSession& userSession = UserSession::instance();

    // 2. 二次校验登录状态（双重保障，避免异常场景）
    if (!userSession.isLoggedIn()) {
        QMessageBox::warning(this, "提示", "登录状态已失效，请重新登录！");
        return;
    }

    // 3. 获取当前登录用户的真实ID（替换硬编码的1）
    int userId = userSession.getUserId();

    // 4. 校验用户ID有效性（UserSession未登录时userId为-1，此处再次兜底）
    if (userId <= 0) {
        QMessageBox::warning(this, "提示", "无效的用户信息，请重新登录！");
        return;
    }
    // 构造插入订单的SQL语句
    QString insertOrderSql = QString("INSERT INTO orders (user_id, flight_id, passenger_name, "
                                     "passenger_id, seat_count, total_amount, status) "
                                     "VALUES (%1, %2, '%3', '%4', %5, %6, 'confirmed')")
                                 .arg(userId)
                                 .arg(m_flightId)
                                 .arg(passengerName) // 实际项目中建议使用绑定参数防止SQL注入，此处为简化直接拼接
                                 .arg(idCard)
                                 .arg(ticketCount)
                                 .arg(totalAmount, 0, 'f', 2);

    // 执行插入订单操作
    QSqlQuery orderQuery = dbManager->executeQuery(insertOrderSql);
    if (orderQuery.lastInsertId().toInt() <= 0) {
        QMessageBox::critical(this, "错误", "订单创建失败！");
        return;
    }

    // 4. 更新航班剩余座位数
    int currentAvailableSeats = ui->labelAvailableSeats->text().toInt();
    int newAvailableSeats = currentAvailableSeats - ticketCount;
    QString updateFlightSql = QString("UPDATE flights SET available_seats = %1 WHERE id = %2")
                                  .arg(newAvailableSeats)
                                  .arg(m_flightId);
    dbManager->executeQuery(updateFlightSql);

    // 5. 提示成功并清空输入
    QMessageBox::information(this, "成功", QString("购票成功！\n订单总价：¥%1\n剩余座位：%2")
                                               .arg(totalAmount, 0, 'f', 2)
                                               .arg(newAvailableSeats));
    ui->lineEditPassengerName->clear();
    ui->lineEditIDCard->clear();
    ui->lineEditPhone->clear();
    ui->lineEditEmail->clear();
    ui->spinBoxTicketCount->setValue(1);

    this->close();
}

void bookTicketConfirm::on_cancelButton_clicked()
{
    // 关闭购票确认窗口
    this->close();
}
