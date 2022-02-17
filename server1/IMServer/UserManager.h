/**
 * @file UserManager.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_USER_MANAGER_H
#define YTALK_USER_MANAGER_H

#include <atomic>
#include <list>
#include <mutex>
#include <vector>

#include "User.h"

namespace YTalk
{

namespace IMServer
{

class UserManager
{
public:
    UserManager();
    ~UserManager();

    int init();

    void getFriendIdByUserId(std::int32_t u_id, std::vector<std::int32_t> &friendId);

private:
    int getAllUsers();
    int getFriendRelationship(std::int32_t u_id, std::list<Friend> &u_friends);

    UserManager(const UserManager &obj) = delete;
    UserManager& operator=(const UserManager &obj) = delete;

private:
    std::atomic<int>                    m_baseUserId;
    std::atomic<int>                    m_baseGroupId;
    std::list<User>                     m_users;
    std::mutex                          m_mutex;

};   /// class UserManager

}   // namespace IMServer

}    // namespace YTalk

#endif // YTALK_USER_MANAGER_H