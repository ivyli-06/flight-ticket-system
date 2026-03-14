#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    // 添加默认构造函数，方便 UserSession 初始化
    User() : username(""), password(""), role("") {}
    User(const QString& username, const QString& password, const QString& role)
        : username(username), password(password), role(role) {}

    QString getUsername() const { return username; }
    QString getPassword() const { return password; }
    QString getRole() const { return role; }

private:
    QString username;
    QString password;
    QString role; // "admin" or "user"
};

#endif // USER_H
