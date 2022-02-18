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
    /**
     * @brief 通过u_id找到用户好友id
     * 
     * @param u_id 
     * @param friendId 返回用户好友id列表
     */
    void getFriendIdByUserId(std::int32_t u_id, std::vector<std::int32_t> &friendId);
    /**
     * @brief 通过用户名，查找是否有该用户
     * 
     * @param u_name 
     * @param user 
     */
    void getUserByUserName(const std::string &u_name, User &user);
    /**
     * @brief 注册用户，将新用户写到数据库，同时加入到m_users里
     * 
     * @param user 新用户
     * @return int 
     */
    int registerForUser(const User &user);
    /**
     * @brief 根据u_nickname查找用户，会找到很多因为多个用户可以有相同的昵称
     * 
     * @param u_nickname 
     * @param users 
     */
    void getUsersByNickname(const std::string &u_nickname, std::list<User> &users);
    /**
     * @brief 判断u_id和f_id是否为好友
     * 
     * @param u_id 
     * @param f_id 
     * @return int 
     */
    int isFriend(std::int32_t u_id, std::int32_t f_id);
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