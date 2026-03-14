#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QList>
#include <QVariant>

class DBManager {
public:
    DBManager(const QString& dbName, const QString& user, const QString& password);
    ~DBManager();

    bool connect();
    void disconnect();
    QSqlQuery executeQuery(const QString& queryStr);
    QList<QVariantList> fetchResults(QSqlQuery& query);
    QSqlDatabase db;


private:



    QString dbName;
    QString user;
    QString password;
    static constexpr const char* kConnName = "FlightTicketDB";
};

#endif // DBMANAGER_H
