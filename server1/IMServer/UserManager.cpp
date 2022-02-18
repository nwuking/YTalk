/**
 * @file UserManager.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "UserManager.h"
#include "../base/Logger.h"
#include "../base/Singleton.h"
#include "../mysql/MysqlManager.h"      

#include <memory>

#define BASE_GROUP_ID 0x0fffffff

namespace YTalk
{

using namespace base;
using namespace mysql;

namespace IMServer
{

UserManager::UserManager() : 
    m_baseUserId(0), 
    m_baseGroupId(BASE_GROUP_ID)   
{
    //
}

UserManager::~UserManager() {
    //
}

int UserManager::init() {
    // 从数据库中加载所有的用户(群)信息
    if(getAllUsers()) {
        return 1;
    }
    for(User &user : m_users) {
        if(getFriendRelationship(user.u_id, user.u_friends)) {
            LOG_ERROR("Fail to get relationship about %d", user.u_id);
            continue;
        }
    }
    return 0;
}

void UserManager::getFriendIdByUserId(std::int32_t u_id, std::vector<std::int32_t> &friendId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for(const auto &it : m_users) {
        if(it.u_id == u_id) {
            for(const auto &f : it.u_friends) {
                friendId.push_back(f.f_id);
            }
            break;
        }
    }
}

void UserManager::getUserByUserName(const std::string &u_name, User &user) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for(const auto &u : m_users) {
        if(u.u_name == u_name) {
            user = u;
            return;
        }
    }
}

int UserManager::registerForUser(const User &newUser) {
    // 将用户注册信息写到数据库
    int u_id = ++m_baseUserId;
    User user = newUser;
    std::string sql = "INSERT INTO user(u_id, u_name, u_password, u_nickname, u_rg_time) "
                      "VALUES(" + std::to_string(u_id) + ", "
                              "'" + user.u_name + "', "
                              "'" + user.u_password + "', "
                              "'" + user.u_nickname + "', "
                              "NOW())";
    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();
    if(!conn || !conn->execute(sql)) {
        LOG_ERROR("Error to insert user");
        return 1;
    }
    Singleton<MysqlManager>::getInstance().putMysqlConn(conn);

    // 设置一些字段
    user.u_id = u_id;
    user.u_gender = "1";
    user.g_ownerid = 0;

   std::lock_guard<std::mutex> lock(m_mutex);
   m_users.push_back(user);
   return 0;
}

int UserManager::getAllUsers() {
    if(!Singleton<MysqlManager>::getInstance().getFlag()) {
        LOG_ERROR("You need init MysqlManager in Main.cpp");
        return 1;
    }

    std::string sql = "SELECT u_id, u_name, u_password, u_nickname, u_gender, "
                      "u_signature, u_birthday, u_teaminfo FROM user ORDER BY u_id DESC";
    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();
    if(conn == nullptr) {
        LOG_ERROR("Fail to get MysqlConn");
        return 2;
    }
    MResultSet *set = conn->query(sql);
    if(set == nullptr) {
        LOG_INFO("No result return in mysql");
        return 3;
    }

    while(set->next()) {
        User user;
        user.u_id = set->getInt("u_id");
        user.u_name = set->getString("u_name");
        user.u_password = set->getString("u_password");
        user.u_nickname = set->getString("u_nickname");
        user.u_gender = set->getString("u_gender");
        user.u_signature = set->getString("u_signature");
        user.u_birthday = set->getInt("u_bithday");
        user.u_teaminfo = set->getString("u_teaminfo");

        m_users.push_back(user);
        
        LOG_TRACE("user{u_id:%d, u_name:%s} push_back m_users", user.u_id, user.u_name.c_str());

        if(user.u_id < BASE_GROUP_ID && user.u_id > m_baseUserId) {
            m_baseUserId = user.u_id;
        }
        else if(user.u_id > m_baseGroupId) {
            m_baseGroupId = user.u_id;
        }
    }

    LOG_INFO("UserManager{m_baseUserId:%d, m_baseGroupId:%d, users:%d}", m_baseUserId, m_baseGroupId, m_users.size());

    Singleton<MysqlManager>::getInstance().putMysqlConn(conn);

    return 0;
}

int UserManager::getFriendRelationship(std::int32_t u_id, std::list<Friend> &u_friends) {
    if(!Singleton<MysqlManager>::getInstance().getFlag()) {
        LOG_ERROR("You need init MysqlManager in Main.cpp");
        return 1;
    }

    std::string sql = "SELECT u_id1, u_id2, u_id1_team, u_id1_remarks, "
                        "u_id2_team, u_id2_remarks FROM relationship "
                        "WHERE u_id1 = " + std::to_string(u_id) + " OR "
                              "u_id2 = " + std::to_string(u_id);
    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();
    if(conn == nullptr) {
        LOG_ERROR("Fail to get MysqlConn");
        return 2;
    }
    MResultSet *set = conn->query(sql);
    if(set == nullptr) {
        LOG_INFO("No result return in mysql");
        return 3;
    }

    while(set->next()) {
        std::int32_t u_id1 = set->getInt("u_id1");
        std::int32_t u_id2 = set->getInt("u_id2");
        std::string u_id1_team = set->getString("u_id1_team");
        std::string u_id1_remarks = set->getString("u_id1_remarks");
        std::string u_id2_team = set->getString("u_id2_team");
        std::string u_id2_remarks = set->getString("u_id2_remarks");

        Friend f;
        if(u_id1 == u_id) {
            f.f_id = u_id2;
            f.f_remarks = u_id1_remarks;
            f.f_team = u_id1_team;
        }
        else {
            f.f_id = u_id1;
            f.f_remarks = u_id2_remarks;
            f.f_team = u_id2_team;
        }

        u_friends.push_back(f);
        LOG_INFO("friends{u_id1=%d, u_id2=%d} push_back u_friends", u_id1, u_id2);
    }

    LOG_INFO("u_id=%d has %d friends", u_id, u_friends.size());

    Singleton<MysqlManager>::getInstance().putMysqlConn(conn);

    return 0;
}

}   // namespace IMServer

}    // namespace YTalk