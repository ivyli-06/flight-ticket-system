#ifndef FLIGHTSEARCHWINDOW_H
#define FLIGHTSEARCHWINDOW_H

#include <QWidget>
#include <QSqlQuery> // 增加这一行
#include "../database/dbmanager.h" // 确保路径正确

namespace Ui {
class FlightSearchWindow;
}

class FlightSearchWindow : public QWidget {
    Q_OBJECT

public:
    explicit FlightSearchWindow(QWidget *parent = nullptr);
    ~FlightSearchWindow();

private slots:
    void searchFlights(); // 槽函数改名，与cpp保持一致

private:
    Ui::FlightSearchWindow *ui;
    DBManager *dbManager; // 建议作为成员变量
};

#endif // FLIGHTSEARCHWINDOW_H
