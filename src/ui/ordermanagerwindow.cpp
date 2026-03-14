#include "ordermanagerwindow.h"
#include "ui_ordermanagerwindow.h" // 确保这个文件由构建系统生成
#include "../models/usersession.h" // 引入 Session
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase> // 如果用到了数据库连接对象
#include <QDateTime>
#include <QCheckBox>

// 构造函数：注意父类改为 QWidget，且初始化 dbManager
OrderManagerWindow::OrderManagerWindow(QWidget *parent) : QWidget(parent),
                                                          ui(new Ui::OrderManagerWindow)
{
    ui->setupUi(this);
    // 初始化数据库
    dbManager = new DBManager("flight_ticket_system", "root", "Saythename_17");

    // 设置表格列
    ui->tableOrders->setColumnCount(8);
    QStringList headers;
    headers << "订单号" << "航班号" << "出发地" << "目的地" << "起飞时间" << "到达时间" << "状态" << "选择";
    ui->tableOrders->setHorizontalHeaderLabels(headers);

    // 调整列宽
    ui->tableOrders->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableOrders->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    ui->tableOrders->setColumnWidth(7, 55);

    loadOrders();
}

OrderManagerWindow::~OrderManagerWindow()
{
    delete ui;
    delete dbManager;
}

void OrderManagerWindow::loadOrders()
{
    if (!dbManager->connect())
    {
        return;
    }

    int userId = UserSession::instance().getUserId();
    qDebug() << "当前查询订单的用户ID:" << userId; // 【调试】确认ID是否正确

    // ---------------------------------------------------------
    // 关键：联表查询 (JOIN)
    // 获取订单信息的同时，获取航班的具体信息
    // ---------------------------------------------------------
    QString sql = QString(
                      "SELECT o.id, f.flight_number, f.departure, f.arrival, f.departure_time, f.arrival_time,o.status "
                      "FROM orders o "
                      "JOIN flights f ON o.flight_id = f.id "
                      "WHERE o.user_id = %1 "
                      "ORDER BY o.order_time DESC")
                      .arg(userId);

    QSqlQuery query = dbManager->executeQuery(sql);
    // 【新增】如果查询失败，打印具体错误原因
    if (query.lastError().isValid())
    {
        qDebug() << "查询订单失败 SQL错误:" << query.lastError().text();
        return;
    }
    QList<QVariantList> dataList = dbManager->fetchResults(query);

    // 【调试】打印查到的行数
    qDebug() << "查询到订单数量:" << dataList.size();

    ui->tableOrders->setRowCount(0);

    for (int i = 0; i < dataList.size(); ++i)
    {
        ui->tableOrders->insertRow(i);
        QVariantList rowData = dataList[i];

        // ========== 0: 订单ID列（居中） ==========
        QTableWidgetItem *itemId = new QTableWidgetItem(rowData[0].toString());
        itemId->setData(Qt::UserRole, rowData[0].toInt());
        // 设置文本水平+垂直居中（核心：新增这行）
        itemId->setTextAlignment(Qt::AlignCenter);
        ui->tableOrders->setItem(i, 0, itemId);

        // ========== 1: 航班号列（居中） ==========
        QTableWidgetItem *itemFlightNum = new QTableWidgetItem(rowData[1].toString());
        itemFlightNum->setTextAlignment(Qt::AlignCenter); // 居中
        ui->tableOrders->setItem(i, 1, itemFlightNum);

        // ========== 2: 出发地列（居中） ==========
        QTableWidgetItem *itemDeparture = new QTableWidgetItem(rowData[2].toString());
        itemDeparture->setTextAlignment(Qt::AlignCenter); // 居中
        ui->tableOrders->setItem(i, 2, itemDeparture);

        // ========== 3: 目的地列（居中） ==========
        QTableWidgetItem *itemArrival = new QTableWidgetItem(rowData[3].toString());
        itemArrival->setTextAlignment(Qt::AlignCenter); // 居中
        ui->tableOrders->setItem(i, 3, itemArrival);

        // ========== 4: 起飞时间列（居中） ==========
        QString departureTimeStr = rowData[4].toDateTime().toString("yyyy-MM-dd HH:mm");
        QTableWidgetItem *itemDepTime = new QTableWidgetItem(departureTimeStr);
        itemDepTime->setTextAlignment(Qt::AlignCenter); // 居中
        ui->tableOrders->setItem(i, 4, itemDepTime);

        // ========== 5: 到达时间列（居中） ==========
        QString arrivalTimeStr = rowData[5].toDateTime().toString("yyyy-MM-dd HH:mm");
        QTableWidgetItem *itemArrTime = new QTableWidgetItem(arrivalTimeStr);
        itemArrTime->setTextAlignment(Qt::AlignCenter); // 居中
        ui->tableOrders->setItem(i, 5, itemArrTime);

        // ========== 6: 状态列（居中） ==========
        QTableWidgetItem *itemStatus = new QTableWidgetItem(rowData[6].toString());
        itemStatus->setTextAlignment(Qt::AlignCenter); // 居中
        ui->tableOrders->setItem(i, 6, itemStatus);

        // ========== 7: 复选框列（已通过布局居中，无需修改） ==========
        QWidget *pWidget = new QWidget();
        QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
        QCheckBox *pCheckBox = new QCheckBox();
        pLayout->addWidget(pCheckBox);
        pLayout->setAlignment(Qt::AlignCenter); // 复选框在单元格内居中
        pLayout->setContentsMargins(0, 0, 0, 0);
        pWidget->setLayout(pLayout);
        ui->tableOrders->setCellWidget(i, 7, pWidget);
    }

    // 【可选优化】设置表头也居中（视觉更统一）
    ui->tableOrders->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
}

void OrderManagerWindow::on_cancelButton_clicked()
{
    int successCount = 0;

    // 遍历表格
    for (int i = 0; i < ui->tableOrders->rowCount(); ++i)
    {
        QWidget *widget = ui->tableOrders->cellWidget(i, 7);
        QCheckBox *checkBox = widget->findChild<QCheckBox *>();

        if (checkBox && checkBox->isChecked())
        {
            // 获取订单 ID (从第0列的 UserRole)
            int orderId = ui->tableOrders->item(i, 0)->data(Qt::UserRole).toInt();

            // 执行删除
            // 注意：是 UPDATE orders SET status='cancelled'
            QString sql = QString("UPDATE orders SET status='cancelled' WHERE id = %1").arg(orderId);
            dbManager->executeQuery(sql);
            successCount++;
        }
    }

    if (successCount > 0)
    {
        QMessageBox::information(this, "成功", QString("成功取消了 %1 个订单。").arg(successCount));
        loadOrders(); // 刷新列表
    }
    else
    {
        QMessageBox::warning(this, "提示", "请先勾选要取消的订单。");
    }
}

void OrderManagerWindow::on_btnRefresh_clicked()
{
    loadOrders();
}

void OrderManagerWindow::on_btnDeleteOrder_clicked()
{
    // 1. 先统计选中的订单数量
    int selectedCount = 0;
    QList<int> selectedOrderIds; // 存储选中的订单ID，用于确认提示和批量删除

    // 遍历表格行，检查复选框是否选中
    for (int i = 0; i < ui->tableOrders->rowCount(); ++i)
    {
        QWidget *widget = ui->tableOrders->cellWidget(i, 7);
        QCheckBox *checkBox = widget->findChild<QCheckBox *>();

        if (checkBox && checkBox->isChecked())
        {
            // 获取订单ID（从第0列的UserRole中读取）
            int orderId = ui->tableOrders->item(i, 0)->data(Qt::UserRole).toInt();
            selectedOrderIds.append(orderId);
            selectedCount++;
        }
    }

    // 2. 未选中任何订单时提示
    if (selectedCount == 0)
    {
        QMessageBox::warning(this, "提示", "请先勾选要删除的订单！");
        return;
    }

    // 3. 弹窗询问是否确认删除（关键：防止误操作）
    int confirm = QMessageBox::question(
        this,
        "确认删除",
        QString("你确定要删除选中的 %1 个订单吗？\n删除后数据将无法恢复！").arg(selectedCount),
        QMessageBox::Yes | QMessageBox::No, // 按钮选项
        QMessageBox::No                     // 默认选中No按钮，降低误触风险
    );

    // 用户取消删除，直接返回
    if (confirm != QMessageBox::Yes)
    {
        return;
    }

    // 4. 执行删除操作（调用DBManager）
    int deleteSuccessCount = 0;
    for (int orderId : selectedOrderIds)
    {
        // 构造删除SQL（注意：DELETE操作不可逆，确保WHERE条件准确）
        QString deleteSql = QString("DELETE FROM orders WHERE id = %1").arg(orderId);

        // 调用DBManager执行删除
        QSqlQuery query = dbManager->executeQuery(deleteSql);

        // 检查删除是否成功（通过query的状态判断）
        if (query.isActive() && query.numRowsAffected() > 0)
        {
            deleteSuccessCount++;
        }
        else
        {
            // 打印删除失败的日志（方便调试）
            qDebug() << "删除订单ID " << orderId << " 失败：" << query.lastError().text();
        }
    }

    // 5. 结果提示
    if (deleteSuccessCount == selectedCount)
    {
        QMessageBox::information(this, "成功", QString("已成功删除 %1 个订单！").arg(deleteSuccessCount));
    }
    else if (deleteSuccessCount > 0)
    {
        QMessageBox::warning(this, "部分成功",
                             QString("仅成功删除 %1 个订单，%2 个订单删除失败！").arg(deleteSuccessCount).arg(selectedCount - deleteSuccessCount));
    }
    else
    {
        QMessageBox::critical(this, "失败", "所有选中的订单均删除失败！");
    }

    // 6. 刷新订单列表（和取消订单逻辑一致）
    loadOrders();
}
