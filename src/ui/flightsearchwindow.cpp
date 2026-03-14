#include "flightsearchwindow.h"
#include "ui_flightsearchwindow.h"
#include <QMessageBox>
#include <QSqlRecord>
#include <QDateTime> // 引入时间处理头文件

FlightSearchWindow::FlightSearchWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlightSearchWindow)
{
    ui->setupUi(this);

    // 初始化数据库
    dbManager = new DBManager("flight_ticket_system", "root", "Saythename_17");

    connect(ui->pushButtonSearch, &QPushButton::clicked, this, &FlightSearchWindow::searchFlights);

    // 优化表格显示：最后一列自适应，其他列根据内容调整
    // 设置表格列数
    ui->tableWidgetResults->setColumnCount(6);
    QStringList headers;
    headers << "航班号" << "出发地" << "目的地" << "起飞时间" <<"到达时间"<< "价格";
    ui->tableWidgetResults->setHorizontalHeaderLabels(headers);

    // 自动调整列宽
    ui->tableWidgetResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidgetResults->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetResults->setSelectionBehavior(QAbstractItemView::SelectRows); // 选中整行
    ui->tableWidgetResults->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 禁止编辑
}

FlightSearchWindow::~FlightSearchWindow()
{
    delete ui;
    delete dbManager;
}

void FlightSearchWindow::searchFlights()
{
    QString departure = ui->lineEditDeparture->text();
    QString arrival = ui->lineEditDestination->text();
    QString date = ui->dateEdit->date().toString("yyyy-MM-dd");

    if (!dbManager->connect()) {
        QMessageBox::critical(this, "数据库错误", "无法连接到数据库。");
        return;
    }

    // 1. 构建查询
    // 注意：使用 DATE() 函数只比较日期部分
    QString queryStr = QString("SELECT flight_number, departure, arrival, departure_time, arrival_time,price, available_seats "
                               "FROM flights "
                               "WHERE departure = '%1' AND arrival = '%2' AND DATE(departure_time) = '%3'")
                           .arg(departure).arg(arrival).arg(date);

    QSqlQuery query = dbManager->executeQuery(queryStr);
    
    // 使用 fetchResults 获取数据列表
    QList<QVariantList> results = dbManager->fetchResults(query);

    // 2. 填充表格
    ui->tableWidgetResults->setRowCount(0); // 清空旧数据

    for (int i = 0; i < results.size(); ++i) {
        ui->tableWidgetResults->insertRow(i);
        QVariantList rowData = results[i];

        // 0: 航班号
        ui->tableWidgetResults->setItem(i, 0, new QTableWidgetItem(rowData[0].toString()));
        
        // 1: 出发地
        ui->tableWidgetResults->setItem(i, 1, new QTableWidgetItem(rowData[1].toString()));
        
        // 2: 目的地
        ui->tableWidgetResults->setItem(i, 2, new QTableWidgetItem(rowData[2].toString()));
        
        // 3: 时间 (格式化优化)
        // 数据库取出的时间转为 QDateTime，然后格式化为 "年-月-日 时:分"
        QDateTime dt_departure = rowData[3].toDateTime();
        QString formattedDepartureTime = dt_departure.toString("yyyy-MM-dd HH:mm");
        ui->tableWidgetResults->setItem(i, 3, new QTableWidgetItem(formattedDepartureTime));

        QDateTime dt_arrival = rowData[4].toDateTime();
        QString formattedArrivalTime = dt_arrival.toString("yyyy-MM-dd HH:mm");
        ui->tableWidgetResults->setItem(i, 4, new QTableWidgetItem(formattedArrivalTime));

        // 5: 价格
        ui->tableWidgetResults->setItem(i, 5, new QTableWidgetItem(rowData[5].toString()));

        // 6: 剩余座位
        ui->tableWidgetResults->setItem(i, 6, new QTableWidgetItem(rowData[6].toString()));
    }

    if (results.isEmpty()) {
        QMessageBox::information(this, "无结果", "未找到符合条件的航班。\n请检查出发地、目的地和日期是否匹配。");
    }
    
    // 保持连接开启，不要在这里 disconnect，否则后续操作可能受影响
}
