#ifndef ORDER_H
#define ORDER_H

#include <QString>

class Order {
public:
    Order(int orderId, int userId, int flightId, const QString& status)
        : orderId(orderId), userId(userId), flightId(flightId), status(status) {}

    int getOrderId() const { return orderId; }
    int getUserId() const { return userId; }
    int getFlightId() const { return flightId; }
    QString getStatus() const { return status; }
    void setStatus(const QString& status) { this->status = status; }

private:
    int orderId;
    int userId;
    int flightId;
    QString status;
};

#endif
