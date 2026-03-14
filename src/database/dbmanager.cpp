#include "dbmanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

DBManager::DBManager(const QString& dbName, const QString& user, const QString& password) {
    this->dbName = dbName;
    this->user = user;
    this->password = password;
}
// DBManager的析构函数（.h里声明~DBManager();）
DBManager::~DBManager() {
    //关闭连接
    if (db.isOpen()) {
        db.close();
    }
}

bool DBManager::connect() {
    // 1. 检查默认连接是否已经存在
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        // 如果存在，直接获取现有的连接实例
        db = QSqlDatabase::database(QSqlDatabase::defaultConnection);
    } else {
        // 2. 如果不存在，才添加新的数据库驱动
        db = QSqlDatabase::addDatabase("QODBC");
        db.setHostName("127.0.0.1");
        db.setPort(3306);
        db.setDatabaseName("local_DB"); // 注意：这里应该是你的ODBC数据源名称
    }

    // 3. 设置用户名和密码（即使获取了现有连接，重新设置凭证也是安全的）
    db.setUserName(user);
    db.setPassword(password);

    // 4. 如果连接已经打开，直接返回成功
    if (db.isOpen()) {
        return true;
    }

    // 5. 尝试打开连接
    bool ok = db.open();
    if (!ok) {
        qDebug() << "Database connection failed:" << db.lastError().text();
        return false;
    }
    return true;
}

void DBManager::disconnect() {

}

QSqlQuery DBManager::executeQuery(const QString& queryStr) {
    QSqlQuery query;
    // 显式指定使用当前的 db 连接，防止多连接混淆
    query = QSqlQuery(db);
    if (!query.exec(queryStr)) {
        qDebug() << "Query execution failed:" << query.lastError().text();
    }
    return query;
}

QList<QVariantList> DBManager::fetchResults(QSqlQuery& query) {
    QList<QVariantList> results;
    while (query.next()) {
        QVariantList row;
        for (int i = 0; i < query.record().count(); ++i) {
            row << query.value(i);
        }
        results << row;
    }
    return results;
}
