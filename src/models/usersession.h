#ifndef USERSESSION_H
#define USERSESSION_H

#include "user.h" // 引入 User 头文件
#include <memory> // 用于智能指针（可选，但这里我们直接存对象或指针）

class UserSession
{
public:
    static UserSession& instance();

    UserSession(const UserSession&) = delete;
    void operator=(const UserSession&) = delete;

    // --- 修改开始 ---

    // 设置当前登录用户（传入完整的 User 对象和数据库ID）
    void setUser(const User& user, int dbId);

    // 获取当前用户对象
    const User& getUser() const;

    // 获取数据库 ID
    int getUserId() const;

    // 判断是否已登录
    bool isLoggedIn() const;

    // 清除会话
    void clear();

private:
    UserSession(); // 构造函数私有化

    // 使用默认构造的空用户作为初始状态
    User currentUser;
    int currentUserId; // 数据库中的主键 ID
    bool loggedIn;     // 登录状态标记
};

#endif // USERSESSION_H
