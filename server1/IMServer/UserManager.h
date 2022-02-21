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
    /**
     * @brief u_id和f_id建立好友关系，并同步到数据库
     * 
     * @param u_id 
     * @param f_id 
     * @return int 
     */
    int buildFriendRelationship(std::int32_t u_id, std::int32_t f_id);
    /**
     * @brief 通过用户id找到相关用户
     * 
     * @param u_id 
     * @param user 
     * @return int 
     */
    int getUserByUserId(std::int32_t u_id, User &user);
    int getUserByUserId(std::int32_t u_id, User* &user);
    /**
     * @brief 接触好友关系，同步到数据库
     * 
     * @param u_id 
     * @param f_id 
     * @return int 
     */
    int releaseFriendRelationship(std::int32_t u_id, std::int32_t f_id);
    /**
     * @brief 将聊天消息保存到数据库中
     * 
     * @param s_id 发送者id
     * @param a_id 接收者id
     * @param data 
     * @return int 
     */
    int saveChatMsg2DB(std::int32_t s_id, std::int32_t a_id, const std::string &data);
    /**
     * @brief 建群
     * 
     * @param g_name 
     * @param u_id 
     * @param g_id 
     * @return int 
     */
    int createGroup(const std::string &g_name, std::int32_t u_id, std::int32_t &g_id);
    /**
     * @brief 更新用户大的基本信息
     * 
     * @param u_id 
     * @param u 
     * @return int 
     */
    int updateUser(std::int32_t u_id, const User &u);
    /**
     * @brief 修改用户密码
     * 
     * @param u_id 
     * @param pw 
     * @return int 
     */
    int changePassword(std::int32_t u_id, const std::string &pw);
    /**
     * @brief 获取用户u_d的分组信息
     * 
     * @param u_id 
     * @param teams 
     * @return int 
     */
    int getTeamsByUserId(std::int32_t u_id, std::string &teams);
    /**
     * @brief 将好友移至其它分组
     * 
     * @param u_id 
     * @param f_id 
     * @param newteam 
     * @return int 
     */
    int moveFriend2OtherTeam(std::int32_t u_id, std::int32_t f_id, const std::string &newteam);
    /**
     * @brief 修改好友的备注
     * 
     * @param u_id 
     * @param f_id 
     * @param newremarks 
     * @return int 
     */
    int changeFriendRemarks(std::int32_t u_id, std::int32_t f_id, const std::string &newremarks);
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