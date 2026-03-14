#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ordermanagerwindow.h"
#include "../models/usersession.h" // 引入 UserSession

#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QDateTime>        // 修复：处理时间需要
#include <QHeaderView>      // 修复：调整列宽需要
#include <QTableWidgetItem> // 修复：设置表格内容需要
#include <QInputDialog> // 需要引入这个来输入身份证号
#include <QCheckBox>     // 必须引入
#include <QMessageBox>   // 必须引入
#include <QSqlError>
#include <QRadioButton>

#include "bookTicketConfirm.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化数据库连接
    dbManager = new DBManager("flight_ticket_system", "root", "Saythename_17");

    // 2. 初始化航班表格
    initFlightTable();
    initCityComboBoxes();

    // 加载数据
    loadAllFlights();

    //初始化搜索状态
    this->isSearching=false;

    // 可选：单程/往返单选框联动（控制返回时间是否可用）
    connect(ui->radioOneWay, &QRadioButton::toggled, this, [=](bool checked) {
        ui->groupBox_4->setEnabled(!checked);
    });
    ui->groupBox_4->setEnabled(false); // 默认禁用返回时间
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dbManager;
}

// 初始化航班表格
void MainWindow::initFlightTable()
{
    // 设置表格列数
    ui->tableFlights->setColumnCount(9);
    QStringList headers;
    headers << "航班号" << "出发地" << "目的地" << "起飞时间" << "到达时间" <<"总座位数"<<"剩余座位数"<< "价格" << "选择";
    ui->tableFlights->setHorizontalHeaderLabels(headers);

    ui->tableFlights->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 最后一列固定宽度
    ui->tableFlights->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Fixed);
    ui->tableFlights->setColumnWidth(8, 55);
}

void MainWindow::initCityComboBoxes()
{
    // 1. 定义所有城市列表
    QStringList cityList = {
        "北京", "上海", "广州", "深圳", "厦门", "青岛",
        "成都", "郑州", "昆明", "杭州", "南京", "武汉",
        "重庆", "西安"
    };

    // 2. 初始化出发地下拉框
    ui->comboDeparture->clear();
    ui->comboDeparture->addItems(cityList);
    ui->comboDeparture->setEditable(true);
    ui->comboDeparture->setCurrentText("北京");
    ui->comboDeparture->setInsertPolicy(QComboBox::NoInsert);
    // 新增：给下拉框的内部编辑框设置居中对齐
    ui->comboDeparture->lineEdit()->setAlignment(Qt::AlignCenter);
    // 去掉编辑框的边框
    ui->comboDeparture->lineEdit()->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid transparent;
            border-bottom: none;
            background-color: transparent;
            padding: 2px;
        }
        QLineEdit:focus {
            border: 1px solid transparent;
            border-bottom: none !important;
            outline: none;
            background-color: transparent;
        }
    )");

    // 3. 初始化目的地下拉框
    ui->comboArrival->clear();
    ui->comboArrival->addItems(cityList);
    ui->comboArrival->setEditable(true);
    ui->comboArrival->setCurrentText("上海");
    ui->comboArrival->setInsertPolicy(QComboBox::NoInsert);
    // 新增：给内部编辑框设置居中对齐
    ui->comboArrival->lineEdit()->setAlignment(Qt::AlignCenter);
    // 去掉编辑框的边框
    ui->comboArrival->lineEdit()->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid transparent;
            border-bottom: none;
            background-color: transparent;
            padding: 2px;
        }
        QLineEdit:focus {
            border: 1px solid transparent;
            border-bottom: none !important;
            outline: none;
            background-color: transparent;
        }
    )");
}

void MainWindow::loadAllFlights()
{
    if (!dbManager->connect()) return;

    // 查询所有需要的字段，包括 id
    QString sql = "SELECT id, flight_number, departure, arrival, departure_time, arrival_time, total_seats, available_seats, price FROM flights";
    QSqlQuery query = dbManager->executeQuery(sql);
    QList<QVariantList> dataList = dbManager->fetchResults(query);

    ui->tableFlights->setRowCount(0);
    ui->labelFlightTitle->setText("所有航班信息");

    for (int i = 0; i < dataList.size(); ++i) {
        ui->tableFlights->insertRow(i);
        QVariantList rowData = dataList[i];

        // --- 航班号列（你的原有逻辑，保留手动居中） ---
        int flightId = rowData[0].toInt();
        QTableWidgetItem *itemFlightNum = new QTableWidgetItem(rowData[1].toString());
        itemFlightNum->setTextAlignment(Qt::AlignCenter); // 手动居中（保留）
        itemFlightNum->setData(Qt::UserRole, flightId);
        ui->tableFlights->setItem(i, 0, itemFlightNum);

        // --- 其他列：逐个显式设置居中（核心修改，百分百生效） ---
        // 1. 出发地列
        QTableWidgetItem *itemDeparture = new QTableWidgetItem(rowData[2].toString());
        itemDeparture->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 1, itemDeparture);

        // 2. 目的地列
        QTableWidgetItem *itemArrival = new QTableWidgetItem(rowData[3].toString());
        itemArrival->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 2, itemArrival);

        // 3. 起飞时间列
        QString dt_dep = rowData[4].toDateTime().toString("yyyy-MM-dd HH:mm");
        QTableWidgetItem *itemDepTime = new QTableWidgetItem(dt_dep);
        itemDepTime->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 3, itemDepTime);

        // 4. 到达时间列
        QString dt_arr = rowData[5].toDateTime().toString("yyyy-MM-dd HH:mm");
        QTableWidgetItem *itemArrTime = new QTableWidgetItem(dt_arr);
        itemArrTime->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 4, itemArrTime);

        // 5. 总座位数列
        QTableWidgetItem *itemTotalSeats = new QTableWidgetItem(rowData[6].toString());
        itemTotalSeats->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 5, itemTotalSeats);

        // 6. 剩余座位数列
        QTableWidgetItem *itemAvailableSeats = new QTableWidgetItem(rowData[7].toString());
        itemAvailableSeats->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 6, itemAvailableSeats);

        // 7. 价格列
        QTableWidgetItem *itemPrice = new QTableWidgetItem(rowData[8].toString());
        itemPrice->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 7, itemPrice);

        // --- 添加复选框 ---
        QWidget *pWidget = new QWidget();
        QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
        QCheckBox *pCheckBox = new QCheckBox();
        pLayout->addWidget(pCheckBox);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0,0,0,0);
        pWidget->setLayout(pLayout);
        ui->tableFlights->setCellWidget(i, 8, pWidget);
    }
}

// 构造查询SQL语句（核心：宽松查找 + 时间精确匹配）
QString MainWindow::buildQuerySql()
{
    // 1. 获取控件输入值
    QString departureKey = ui->comboDeparture->currentText().trimmed(); // 出发地关键字（如“北京”）
    QString arrivalKey = ui->comboArrival->currentText().trimmed();     // 目的地关键字（如“上海”）
    QDate departDate = ui->dateEditDeparture->date();               // 出发日期
    QDate returnDate = ui->dateEditReturn->date();                 // 返回日期（往返时使用）
    bool isRoundTrip = ui->radioRoundTrip->isChecked();             // 是否往返
    bool isOneWay = ui->radioOneWay->isChecked();                   // 是否单程

    // 2. 构造基础SQL
    QString sql = "SELECT id, flight_number, departure, arrival, departure_time, arrival_time, total_seats, available_seats, price FROM flights WHERE 1=1";

    // 3. 宽松查找：起落地包含关键字（LIKE %关键字%，匹配机场名中的地名）
    if (!departureKey.isEmpty()) {
        // 拼接宽松查询条件（注意：%需要转义，或用arg拼接）
        sql += QString(" AND departure LIKE '%%1%'").arg(departureKey);
    }
    if (!arrivalKey.isEmpty()) {
        sql += QString(" AND arrival LIKE '%%1%'").arg(arrivalKey);
    }

    // 4. 时间精确匹配：按日期构造时间区间（匹配当天所有时间点）
    if (isOneWay || isRoundTrip) {
        // 出发时间区间：所选日期 00:00:00 到 23:59:59
        QString departStartTime = departDate.toString("yyyy-MM-dd") + " 00:00:00";
        QString departEndTime = departDate.toString("yyyy-MM-dd") + " 23:59:59";
        sql += QString(" AND departure_time BETWEEN '%1' AND '%2'").arg(departStartTime).arg(departEndTime);

        // 往返时：额外添加返回时间区间
        if (isRoundTrip) {
            QString returnStartTime = returnDate.toString("yyyy-MM-dd") + " 00:00:00";
            QString returnEndTime = returnDate.toString("yyyy-MM-dd") + " 23:59:59";
            sql += QString(" AND arrival_time BETWEEN '%1' AND '%2'").arg(returnStartTime).arg(returnEndTime);
        }
    }

    // 5. 可选：按价格升序排序
    sql += " ORDER BY price ASC";

    qDebug() << "构造的SQL语句：" << sql;
    return sql;
}

// 填充航班表格
void MainWindow::fillFlightTable(const QList<QVariantList> &flightData)
{
    if (!dbManager->connect()) return;

    // 先清空表格原有数据
    ui->tableFlights->setRowCount(0);
    ui->labelFlightTitle->setText("筛选航班信息");

    for (int i = 0; i < flightData.size(); ++i) {
        ui->tableFlights->insertRow(i);
        QVariantList rowData = flightData[i];

        // --- 航班号列（你的原有逻辑，保留手动居中） ---
        int flightId = rowData[0].toInt();
        QTableWidgetItem *itemFlightNum = new QTableWidgetItem(rowData[1].toString());
        itemFlightNum->setTextAlignment(Qt::AlignCenter); // 手动居中（保留）
        itemFlightNum->setData(Qt::UserRole, flightId);
        ui->tableFlights->setItem(i, 0, itemFlightNum);

        // --- 其他列：逐个显式设置居中（核心修改，百分百生效） ---
        // 1. 出发地列
        QTableWidgetItem *itemDeparture = new QTableWidgetItem(rowData[2].toString());
        itemDeparture->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 1, itemDeparture);

        // 2. 目的地列
        QTableWidgetItem *itemArrival = new QTableWidgetItem(rowData[3].toString());
        itemArrival->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 2, itemArrival);

        // 3. 起飞时间列
        QString dt_dep = rowData[4].toDateTime().toString("yyyy-MM-dd HH:mm");
        QTableWidgetItem *itemDepTime = new QTableWidgetItem(dt_dep);
        itemDepTime->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 3, itemDepTime);

        // 4. 到达时间列
        QString dt_arr = rowData[5].toDateTime().toString("yyyy-MM-dd HH:mm");
        QTableWidgetItem *itemArrTime = new QTableWidgetItem(dt_arr);
        itemArrTime->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 4, itemArrTime);

        // 5. 总座位数列
        QTableWidgetItem *itemTotalSeats = new QTableWidgetItem(rowData[6].toString());
        itemTotalSeats->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 5, itemTotalSeats);

        // 6. 剩余座位数列
        QTableWidgetItem *itemAvailableSeats = new QTableWidgetItem(rowData[7].toString());
        itemAvailableSeats->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 6, itemAvailableSeats);

        // 7. 价格列
        QTableWidgetItem *itemPrice = new QTableWidgetItem(rowData[8].toString());
        itemPrice->setTextAlignment(Qt::AlignCenter); // 新增：居中
        ui->tableFlights->setItem(i, 7, itemPrice);

        // --- 添加复选框 ---
        QWidget *pWidget = new QWidget();
        QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
        QCheckBox *pCheckBox = new QCheckBox();
        pLayout->addWidget(pCheckBox);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0,0,0,0);
        pWidget->setLayout(pLayout);
        ui->tableFlights->setCellWidget(i, 8, pWidget);
    }
}

// 查询按钮点击事件
void MainWindow::on_btnQuerySearch_clicked()
{
    // 1. 检查数据库连接
    if (!dbManager->connect()) {
        qDebug() << "查询失败：数据库未连接！";
        return;
    }

    // 2. 构造查询SQL
    QString sql = buildQuerySql();

    // 3. 执行查询（调用DBManager接口）
    QSqlQuery query = dbManager->executeQuery(sql);

    // 4. 获取查询结果
    QList<QVariantList> flightData = dbManager->fetchResults(query);

    // 5. 填充表格
    initFlightTable();
    fillFlightTable(flightData);

    //6 更新搜索状态
    this->isSearching=true;

    qDebug() << "查询完成，共获取" << flightData.count() << "条航班数据";
}

void MainWindow::on_btnMyOrders_clicked()
{
    // 打开订单管理窗口
    OrderManagerWindow *orderWin = new OrderManagerWindow();
    
    // 修复：设置窗口关闭时自动释放内存，防止内存泄漏
    orderWin->setAttribute(Qt::WA_DeleteOnClose);
    
    orderWin->show();
}

void MainWindow::on_btnRefresh_clicked()
{
    if(this->isSearching)
        //在搜索状态 保留当前查询条件 再次查询
        on_btnQuerySearch_clicked();
    else
        //不在 查询所有航班信息
        loadAllFlights();
}

void MainWindow::on_btnReset_clicked(){
    this->isSearching=false;
    loadAllFlights();
}

void MainWindow::on_btnBookTicket_clicked()
{
    // 1. 检查登录状态
    if (!UserSession::instance().isLoggedIn()) {
        QMessageBox::warning(this, "提示", "请先登录！");
        return;
    }

    // 获取当前用户信息
    int userId = UserSession::instance().getUserId();
    QString userName = UserSession::instance().getUser().getUsername();

    // 2. 遍历表格寻找选中的航班
    int selectedRow = -1;
    for (int i = 0; i < ui->tableFlights->rowCount(); ++i) {
        QWidget *widget = ui->tableFlights->cellWidget(i, 8); // 第8列是复选框（你的原有逻辑，保留）
        QCheckBox *checkBox = widget->findChild<QCheckBox *>();
        if (checkBox && checkBox->isChecked()) {
            selectedRow = i;
            break; // 暂时只支持单选
        }
    }

    if (selectedRow == -1) {
        QMessageBox::warning(this, "提示", "请先勾选一个航班。");
        return;
    }

    // 3. 从选中行第0列提取航班号和航班ID（适配你的表格结构，关键修改）
    QTableWidgetItem *flightItem = ui->tableFlights->item(selectedRow, 0); // 列索引改为0（你的航班号列）
    if (!flightItem) {
        QMessageBox::warning(this, "提示", "无法获取选中航班的信息，请重试！");
        return;
    }

    // 提取航班号（Item的显示文本）
    QString flightNumber = flightItem->text().trimmed();
    // 提取航班ID（绑定在Item的Qt::UserRole上的自定义数据）
    int flightId = flightItem->data(Qt::UserRole).toInt();

    // 校验航班信息有效性
    if (flightId <= 0 || flightNumber.isEmpty()) {
        QMessageBox::warning(this, "提示", "无效的航班信息，请重试！");
        return;
    }

    // 4. 创建购票确认窗口并设置航班信息
    bookTicketConfirm *ticketWindow = new bookTicketConfirm();

    // 保留窗口自动释放内存的属性
    ticketWindow->setAttribute(Qt::WA_DeleteOnClose);

    // 调用setFlightInfo传递航班ID和航班号
    ticketWindow->setFlightInfo(flightId, flightNumber);

    // 5. 显示购票确认窗口
    ticketWindow->show();
    this->isSearching = false;
    loadAllFlights();
}
void MainWindow::on_btnSwap_clicked(){
    QString temp=ui->comboDeparture->currentText();
    ui->comboDeparture->setCurrentText(ui->comboArrival->currentText());
    ui->comboArrival->setCurrentText(temp);
}
