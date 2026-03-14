#include "usersession.h"

// 初始化静态成员：默认构造一个空用户，ID为-1
UserSession::UserSession()
    : currentUser("", "", ""), currentUserId(-1), loggedIn(false)
{
}

UserSession& UserSession::instance()
{
    static UserSession instance;
    return instance;
}

void UserSession::setUser(const User& user, int dbId)
{
    currentUser = user;
    currentUserId = dbId;
    loggedIn = true;
}

const User& UserSession::getUser() const
{
    return currentUser;
}

int UserSession::getUserId() const
{
    return currentUserId;
}

bool UserSession::isLoggedIn() const
{
    return loggedIn;
}

void UserSession::clear()
{
    // 重置为空用户
    currentUser = User("", "", "");
    currentUserId = -1;
    loggedIn = false;
}
