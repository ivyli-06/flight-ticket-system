#include "AdminWindow.h"
#include "ui_AdminWindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include<QSqlError>
#include<QCryptographicHash>

AdminWindow::AdminWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminWindow)
{
    m_dbManager = new DBManager("flight_ticket_system", "root", "Saythename_17");

    ui->setupUi(this);
    this->setWindowTitle("管理员管理系统");
    this->resize(1200, 800);

    // 初始化表格
    initFlightTable();
    initUserTable();
    initOrderTable();

    // 默认加载所有数据
    on_btnFlightQuery_clicked();
    on_btnUserQuery_clicked();
    on_btnOrderQuery_clicked();
}

AdminWindow::~AdminWindow()
{
    delete ui;
}

// -------------------------- 航班管理模块 --------------------------
void AdminWindow::initFlightTable()
{
    ui->tableFlights->setColumnCount(11);
    ui->tableFlights->setHorizontalHeaderLabels({
        "ID", "航班号", "出发地", "目的地", "出发时间",
        "到达时间", "总座位数", "可用座位数", "价格(元)", "状态", "创建时间"
    });
    ui->tableFlights->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableFlights->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableFlights->horizontalHeader()->setStretchLastSection(true);

    // 状态下拉框初始化
    ui->cbFlightStatus->addItems({"scheduled", "delayed", "cancelled"});
}

void AdminWindow::fillFlightTable(const QList<QVariantList> &data)
{
    ui->tableFlights->setRowCount(0);
    for (int i = 0; i < data.size(); ++i) {
        QVariantList row = data[i];
        ui->tableFlights->insertRow(i);
        for (int j = 0; j < row.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(row[j].toString());
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableFlights->setItem(i, j, item);
        }
    }
}

void AdminWindow::clearFlightInputs()
{
    ui->leFlightNum->clear();
    ui->leDeparture->clear();
    ui->leArrival->clear();
    ui->dtDeparture->setDateTime(QDateTime::currentDateTime());
    ui->dtArrival->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    ui->sbTotalSeats->setValue(100);
    ui->sbAvailableSeats->setValue(100);
    ui->dsPrice->setValue(1000.00);
    ui->cbFlightStatus->setCurrentIndex(0);
}

bool AdminWindow::validateFlightInputs(bool isEdit)
{
    if (ui->leFlightNum->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "航班号不能为空！");
        return false;
    }
    if (ui->leDeparture->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "出发地不能为空！");
        return false;
    }
    if (ui->leArrival->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "目的地不能为空！");
        return false;
    }
    if (ui->dtDeparture->dateTime() >= ui->dtArrival->dateTime()) {
        QMessageBox::warning(this, "警告", "到达时间必须晚于出发时间！");
        return false;
    }
    if (ui->sbTotalSeats->value() < ui->sbAvailableSeats->value()) {
        QMessageBox::warning(this, "警告", "可用座位数不能大于总座位数！");
        return false;
    }
    return true;
}

void AdminWindow::on_btnFlightQuery_clicked()
{
    QString sql = "SELECT id, flight_number, departure, arrival, departure_time, "
                  "arrival_time, total_seats, available_seats, price, status, created_at "
                  "FROM flights WHERE 1=1 ";

    // 条件查询
    if (!ui->leQueryFlightNum->text().isEmpty()) {
        sql += QString("AND flight_number LIKE '%%1%' ").arg(ui->leQueryFlightNum->text());
    }
    if (!ui->leQueryDeparture->text().isEmpty()) { // 原：leDeparture
        sql += QString("AND departure LIKE '%%1%' ").arg(ui->leQueryDeparture->text());
    }
    if (!ui->leQueryArrival->text().isEmpty()) { // 原：leArrival
        sql += QString("AND arrival LIKE '%%1%' ").arg(ui->leQueryArrival->text());
    }

    QSqlQuery query = m_dbManager->executeQuery(sql);
    QList<QVariantList> data = m_dbManager->fetchResults(query);
    fillFlightTable(data);
}

void AdminWindow::on_btnFlightAdd_clicked()
{
    if (!validateFlightInputs()) return;

    QString sql = QString(
                      "INSERT INTO flights (flight_number, departure, arrival, departure_time, "
                      "arrival_time, total_seats, available_seats, price, status) "
                      "VALUES ('%1', '%2', '%3', '%4', '%5', %6, %7, %8, '%9')"
                      ).arg(ui->leFlightNum->text())
                      .arg(ui->leDeparture->text())
                      .arg(ui->leArrival->text())
                      .arg(ui->dtDeparture->dateTime().toString("yyyy-MM-dd HH:mm:ss"))
                      .arg(ui->dtArrival->dateTime().toString("yyyy-MM-dd HH:mm:ss"))
                      .arg(ui->sbTotalSeats->value())
                      .arg(ui->sbAvailableSeats->value())
                      .arg(ui->dsPrice->value())
                      .arg(ui->cbFlightStatus->currentText());

    QSqlQuery query = m_dbManager->executeQuery(sql);
    if (query.numRowsAffected() > 0) {
        QMessageBox::information(this, "成功", "航班添加成功！");
        clearFlightInputs();
        on_btnFlightQuery_clicked();
    } else {
        QMessageBox::critical(this, "失败", "航班添加失败：" + query.lastError().text());
    }
}

void AdminWindow::on_btnFlightEdit_clicked()
{
    QList<QTableWidgetItem *> selectedItems = ui->tableFlights->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选中要修改的航班！");
        return;
    }

    int flightId = ui->tableFlights->item(selectedItems.first()->row(), 0)->text().toInt();
    if (!validateFlightInputs(true)) return;

    QString sql = QString(
                      "UPDATE flights SET flight_number='%1', departure='%2', arrival='%3', "
                      "departure_time='%4', arrival_time='%5', total_seats=%6, "
                      "available_seats=%7, price=%8, status='%9' WHERE id=%10"
                      ).arg(ui->leFlightNum->text())
                      .arg(ui->leDeparture->text())
                      .arg(ui->leArrival->text())
                      .arg(ui->dtDeparture->dateTime().toString("yyyy-MM-dd HH:mm:ss"))
                      .arg(ui->dtArrival->dateTime().toString("yyyy-MM-dd HH:mm:ss"))
                      .arg(ui->sbTotalSeats->value())
                      .arg(ui->sbAvailableSeats->value())
                      .arg(ui->dsPrice->value())
                      .arg(ui->cbFlightStatus->currentText())
                      .arg(flightId);

    QSqlQuery query = m_dbManager->executeQuery(sql);
    if (query.numRowsAffected() > 0) {
        QMessageBox::information(this, "成功", "航班修改成功！");
        on_btnFlightQuery_clicked();
    } else {
        QMessageBox::critical(this, "失败", "航班修改失败：" + query.lastError().text());
    }
}

void AdminWindow::on_btnFlightDelete_clicked()
{
    QList<QTableWidgetItem *> selectedItems = ui->tableFlights->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选中要删除的航班！");
        return;
    }

    int flightId = ui->tableFlights->item(selectedItems.first()->row(), 0)->text().toInt();
    int confirm = QMessageBox::question(this, "确认", "确定要删除该航班吗？删除后关联订单数据可能异常！",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (confirm != QMessageBox::Yes) return;

    QString sql = QString("DELETE FROM flights WHERE id=%1").arg(flightId);
    QSqlQuery query = m_dbManager->executeQuery(sql);
    if (query.numRowsAffected() > 0) {
        QMessageBox::information(this, "成功", "航班删除成功！");
        on_btnFlightQuery_clicked();
    } else {
        QMessageBox::critical(this, "失败", "航班删除失败：" + query.lastError().text());
    }
}

void AdminWindow::on_tableFlights_itemSelectionChanged()
{
    QList<QTableWidgetItem *> selectedItems = ui->tableFlights->selectedItems();
    if (selectedItems.isEmpty()) return;

    int row = selectedItems.first()->row();

    ui->leFlightNum->setText(ui->tableFlights->item(row, 1)->text());
    ui->leDeparture->setText(ui->tableFlights->item(row, 2)->text());
    ui->leArrival->setText(ui->tableFlights->item(row, 3)->text());

    // 关键修改：解析格式改为 "yyyy-MM-ddTHH:mm:ss.zzz"，适配带T和毫秒的ISO格式
    QString depTimeText = ui->tableFlights->item(row, 4)->text();
    QString arrTimeText = ui->tableFlights->item(row, 5)->text();
    // 适配 ISO 8601 格式（T分隔 + 毫秒数）
    QDateTime depDateTime = QDateTime::fromString(depTimeText, "yyyy-MM-ddTHH:mm:ss.zzz");
    QDateTime arrDateTime = QDateTime::fromString(arrTimeText, "yyyy-MM-ddTHH:mm:ss.zzz");

    // 安全判断：若解析有效，再赋值；无效则设为当前时间（可选）
    if (depDateTime.isValid()) {
        ui->dtDeparture->setDateTime(depDateTime);
    }
    if (arrDateTime.isValid()) {
        ui->dtArrival->setDateTime(arrDateTime);
    }

    ui->sbTotalSeats->setValue(ui->tableFlights->item(row, 6)->text().toInt());
    ui->sbAvailableSeats->setValue(ui->tableFlights->item(row, 7)->text().toInt());
    ui->dsPrice->setValue(ui->tableFlights->item(row, 8)->text().toDouble());
    ui->cbFlightStatus->setCurrentText(ui->tableFlights->item(row, 9)->text());
}

// -------------------------- 用户管理模块 --------------------------
void AdminWindow::initUserTable()
{
    ui->tableUsers->setColumnCount(6);
    ui->tableUsers->setHorizontalHeaderLabels({
        "ID", "用户名", "密码(加密)", "邮箱", "角色", "创建时间"
    });
    ui->tableUsers->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableUsers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableUsers->horizontalHeader()->setStretchLastSection(true);

    // 角色下拉框初始化
    ui->cbUserRole->addItems({"customer", "admin"});
}

void AdminWindow::fillUserTable(const QList<QVariantList> &data)
{
    ui->tableUsers->setRowCount(0);
    for (int i = 0; i < data.size(); ++i) {
        QVariantList row = data[i];
        ui->tableUsers->insertRow(i);
        for (int j = 0; j < row.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(row[j].toString());
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableUsers->setItem(i, j, item);
        }
    }
}

void AdminWindow::clearUserInputs()
{
    ui->leUsername->clear();
    ui->leEmail->clear();
    ui->cbUserRole->setCurrentIndex(0);
}

bool AdminWindow::validateUserInputs()
{
    if (ui->leUsername->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名不能为空！");
        return false;
    }
    if (!ui->leEmail->text().isEmpty() && !ui->leEmail->text().contains("@")) {
        QMessageBox::warning(this, "警告", "邮箱格式不正确！");
        return false;
    }
    return true;
}

void AdminWindow::on_btnUserQuery_clicked()
{
    QString sql = "SELECT id, username, password, email, role, created_at "
                  "FROM users WHERE 1=1 ";

    if (!ui->leQueryUsername->text().isEmpty()) {
        sql += QString("AND username LIKE '%%1%' ").arg(ui->leQueryUsername->text());
    }
    if (!ui->leQueryEmail->text().isEmpty()) {
        sql += QString("AND email LIKE '%%1%' ").arg(ui->leQueryEmail->text());
    }
    if (ui->cbQueryUserRole->currentIndex() != -1) {
        sql += QString("AND role='%1' ").arg(ui->cbQueryUserRole->currentText());
    }

    QSqlQuery query = m_dbManager->executeQuery(sql);
    QList<QVariantList> data = m_dbManager->fetchResults(query);
    fillUserTable(data);
}

void AdminWindow::on_btnUserEdit_clicked()
{
    QList<QTableWidgetItem *> selectedItems = ui->tableUsers->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选中要修改的用户！");
        return;
    }

    int userId = ui->tableUsers->item(selectedItems.first()->row(), 0)->text().toInt();
    if (!validateUserInputs()) return;

    QString sql = QString(
                      "UPDATE users SET username='%1', email='%2', role='%3' "
                      "WHERE id=%4"
                      ).arg(ui->leUsername->text())
                      .arg(ui->leEmail->text())
                      .arg(ui->cbUserRole->currentText())
                      .arg(userId);

    QSqlQuery query = m_dbManager->executeQuery(sql);
    if (query.numRowsAffected() > 0) {
        QMessageBox::information(this, "成功", "用户信息修改成功！");
        on_btnUserQuery_clicked();
    } else {
        QMessageBox::critical(this, "失败", "用户信息修改失败：" + query.lastError().text());
    }
}

void AdminWindow::on_btnUserResetPwd_clicked()
{
    QList<QTableWidgetItem *> selectedItems = ui->tableUsers->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选中要重置密码的用户！");
        return;
    }

    int userId = ui->tableUsers->item(selectedItems.first()->row(), 0)->text().toInt();
    int confirm = QMessageBox::question(this, "确认", "确定要将密码重置为123456吗？",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (confirm != QMessageBox::Yes) return;

    // 密码加密（示例：MD5加密，实际项目建议用更安全的加密方式）
    QString pwd = QString(QCryptographicHash::hash("123456", QCryptographicHash::Md5).toHex());
    QString sql = QString("UPDATE users SET password='%1' WHERE id=%2").arg(pwd).arg(userId);

    QSqlQuery query = m_dbManager->executeQuery(sql);
    if (query.numRowsAffected() > 0) {
        QMessageBox::information(this, "成功", "密码重置成功！默认密码：123456");
    } else {
        QMessageBox::critical(this, "失败", "密码重置失败：" + query.lastError().text());
    }
}

void AdminWindow::on_tableUsers_itemSelectionChanged()
{
    QList<QTableWidgetItem *> selectedItems = ui->tableUsers->selectedItems();
    if (selectedItems.isEmpty()) return;

    int row = selectedItems.first()->row();
    ui->leUsername->setText(ui->tableUsers->item(row, 1)->text());
    ui->leEmail->setText(ui->tableUsers->item(row, 3)->text());
    ui->cbUserRole->setCurrentText(ui->tableUsers->item(row, 4)->text());
}

// -------------------------- 订单管理模块 --------------------------
void AdminWindow::initOrderTable()
{
    ui->tableOrders->setColumnCount(10);
    ui->tableOrders->setHorizontalHeaderLabels({
        "ID", "用户ID", "航班ID", "乘客姓名", "身份证号",
        "座位数", "总金额(元)", "状态", "下单时间", "航班号"
    });
    ui->tableOrders->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableOrders->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableOrders->horizontalHeader()->setStretchLastSection(true);

    // 订单状态下拉框初始化
    ui->cbOrderStatus->addItems({"reserved", "confirmed", "cancelled"});
}

void AdminWindow::fillOrderTable(const QList<QVariantList> &data)
{
    ui->tableOrders->setRowCount(0);
    for (int i = 0; i < data.size(); ++i) {
        QVariantList row = data[i];
        ui->tableOrders->insertRow(i);
        for (int j = 0; j < row.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(row[j].toString());
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableOrders->setItem(i, j, item);
        }
    }
}

void AdminWindow::clearOrderInputs()
{
    ui->leUserId->clear();
    ui->leFlightId->clear();
    ui->lePassenger->clear();
    ui->leIdCard->clear();
    ui->sbSeatCount->setValue(1);
    ui->dsTotalAmount->setValue(0.00);
    ui->cbOrderStatus->setCurrentIndex(0);
}

bool AdminWindow::validateOrderInputs()
{
    if (ui->leUserId->text().isEmpty() || !ui->leUserId->text().toInt()) {
        QMessageBox::warning(this, "警告", "用户ID必须为有效数字！");
        return false;
    }
    if (ui->leFlightId->text().isEmpty() || !ui->leFlightId->text().toInt()) {
        QMessageBox::warning(this, "警告", "航班ID必须为有效数字！");
        return false;
    }
    if (ui->lePassenger->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "乘客姓名不能为空！");
        return false;
    }
    if (ui->leIdCard->text().isEmpty() || ui->leIdCard->text().length() != 18) {
        QMessageBox::warning(this, "警告", "身份证号必须为18位！");
        return false;
    }
    if (ui->sbSeatCount->value() < 1) {
        QMessageBox::warning(this, "警告", "座位数不能小于1！");
        return false;
    }
    return true;
}

void AdminWindow::on_btnOrderQuery_clicked()
{
    QString sql = "SELECT o.id, o.user_id, o.flight_id, o.passenger_name, o.passenger_id, "
                  "o.seat_count, o.total_amount, o.status, o.order_time, f.flight_number "
                  "FROM orders o JOIN flights f ON o.flight_id = f.id WHERE 1=1 ";

    if (!ui->leQueryUserId->text().isEmpty()) {
        sql += QString("AND o.user_id=%1 ").arg(ui->leQueryUserId->text());
    }
    if (!ui->leQueryFlightId->text().isEmpty()) {
        sql += QString("AND o.flight_id=%1 ").arg(ui->leQueryFlightId->text());
    }
    if (!ui->leQueryPassenger->text().isEmpty()) {
        sql += QString("AND o.passenger_name LIKE '%%1%' ").arg(ui->leQueryPassenger->text());
    }

    QSqlQuery query = m_dbManager->executeQuery(sql);
    QList<QVariantList> data = m_dbManager->fetchResults(query);
    fillOrderTable(data);
}

void AdminWindow::on_btnOrderEdit_clicked()
{
    QList<QTableWidgetItem *> selectedItems = ui->tableOrders->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选中要修改的订单！");
        return;
    }

    int orderId = ui->tableOrders->item(selectedItems.first()->row(), 0)->text().toInt();
    if (!validateOrderInputs()) return;

    QString sql = QString(
                      "UPDATE orders SET user_id=%1, flight_id=%2, passenger_name='%3', "
                      "passenger_id='%4', seat_count=%5, total_amount=%6, status='%7' "
                      "WHERE id=%8"
                      ).arg(ui->leUserId->text())
                      .arg(ui->leFlightId->text())
                      .arg(ui->lePassenger->text())
                      .arg(ui->leIdCard->text())
                      .arg(ui->sbSeatCount->value())
                      .arg(ui->dsTotalAmount->value())
                      .arg(ui->cbOrderStatus->currentText())
                      .arg(orderId);

    QSqlQuery query = m_dbManager->executeQuery(sql);
    if (query.numRowsAffected() > 0) {
        QMessageBox::information(this, "成功", "订单修改成功！");
        on_btnOrderQuery_clicked();
    } else {
        QMessageBox::critical(this, "失败", "订单修改失败：" + query.lastError().text());
    }
}

void AdminWindow::on_tableOrders_itemSelectionChanged()
{
    QList<QTableWidgetItem *> selectedItems = ui->tableOrders->selectedItems();
    if (selectedItems.isEmpty()) return;

    int row = selectedItems.first()->row();
    ui->leUserId->setText(ui->tableOrders->item(row, 1)->text());
    ui->leFlightId->setText(ui->tableOrders->item(row, 2)->text());
    ui->lePassenger->setText(ui->tableOrders->item(row, 3)->text());
    ui->leIdCard->setText(ui->tableOrders->item(row, 4)->text());
    ui->sbSeatCount->setValue(ui->tableOrders->item(row, 5)->text().toInt());
    ui->dsTotalAmount->setValue(ui->tableOrders->item(row, 6)->text().toDouble());
    ui->cbOrderStatus->setCurrentText(ui->tableOrders->item(row, 7)->text());
}
