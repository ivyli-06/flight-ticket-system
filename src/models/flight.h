#ifndef FLIGHT_H
#define FLIGHT_H

#include <QString>
#include <QDate>

class Flight {
public:
    Flight(const QString &flightNumber, const QString &departure, const QString &destination, const QDate &date)
        : flightNumber(flightNumber), departure(departure), destination(destination), date(date) {}

    QString getFlightNumber() const { return flightNumber; }
    QString getDeparture() const { return departure; }
    QString getDestination() const { return destination; }
    QDate getDate() const { return date; }

private:
    QString flightNumber; // 航班号
    QString departure;     // 出发地
    QString destination;   // 目的地
    QDate date;           // 日期
};

#endif // FLIGHT_H
