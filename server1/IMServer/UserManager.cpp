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
#include <sstream>

#define BASE_GROUP_ID 0x0fffffff

const std::string DEFAULT_TEAM = "friends";

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
    user.u_faceType = 0;
    user.u_birthday = 20220101;

   std::lock_guard<std::mutex> lock(m_mutex);
    m_users.push_back(user);
   return 0;
}

int UserManager::getAllUsers() {
    if(!Singleton<MysqlManager>::getInstance().getFlag()) {
        LOG_ERROR("You need init MysqlManager in Main.cpp");
        return 1;
    }

    std::string sql = "SELECT u_id, u_name, u_password, u_nickname, u_gender, u_birthday, "
                      "u_signature, u_facetype, u_face, u_teaminfo FROM user ORDER BY u_id DESC";
    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();
    if(conn == nullptr) {
        LOG_ERROR("Fail to get MysqlConn");
        return 2;
    }
    MResultSet *set = conn->query(sql);
    if(set == nullptr) {
        LOG_INFO("execute sql=%s error", sql.c_str());
        Singleton<MysqlManager>::getInstance().putMysqlConn(conn);
        return 3;
    }

    while(set->next()) {
        User user;
        user.u_id = set->getInt("u_id");
        user.u_name = set->getString("u_name");
        user.u_password = set->getString("u_password");
        user.u_nickname = set->getString("u_nickname");
        user.u_gender = set->getString("u_gender");
        user.u_birthday = set->getInt("u_bithday");
        user.u_signature = set->getString("u_signature");
        user.u_faceType = set->getInt("u_facetype");
        user.u_face = set->getString("u_face");
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

    LOG_INFO("UserManager{m_baseUserId:%d, m_baseGroupId:%d, users:%d}", m_baseUserId.load(), m_baseGroupId.load(), m_users.size());

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
        Singleton<MysqlManager>::getInstance().putMysqlConn(conn);
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

void UserManager::getUsersByNickname(const std::string &u_nickname, std::list<User> &users) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for(const auto &user : m_users) {
        if(user.u_nickname == u_nickname) {
            users.push_back(user);
        }
    }
}

int UserManager::isFriend(std::int32_t u_id, std::int32_t f_id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for(const auto &user : m_users) {
        if(user.u_id == u_id) {
            for(const auto &f : user.u_friends) {
                if(f.f_id == f_id) {
                    return 0;
                }
            }
            return 1;
        }
    }
    return 2;
}

int UserManager::buildFriendRelationship(std::int32_t u_id, std::int32_t f_id) {
    if(u_id == f_id) {
        //LOG_WRANING("");
        return 1;
    }

    if(u_id > f_id) {
        // 确保u_id小于f_d
        std::int32_t temp = u_id;
        u_id = f_id;
        f_id = temp;
    }

    // 同步到数据库
    std::string sql = "INSERT INTO relationship(u_id1, u_id2, u_id1_team, u_id2_team) "
                      "VALUES(" + std::to_string(u_id) + ", " + std::to_string(f_id) + ", "
                               "'" + DEFAULT_TEAM + "', '" + DEFAULT_TEAM + "')";
    
    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();
    if(!conn->execute(sql)) {
        LOG_ERROR("error to insert into relationship");
        return 2;
    }
    Singleton<MysqlManager>::getInstance().putMysqlConn(conn);

    // 同步到内存
    bool op1 = false;
    bool op2 = false;
    std::lock_guard<std::mutex> lock(m_mutex);
    for(auto &user : m_users) {
        if(user.u_id == u_id) {
            Friend fri = {f_id, "", DEFAULT_TEAM};
            user.u_friends.emplace_back(fri);
            op1 = true;
            continue;
        }
        if(user.u_id == f_id) {
            Friend fri = {u_id, "", DEFAULT_TEAM};
            user.u_friends.emplace_back(fri);
            op2 = true;
        }

        if(op1 && op2) {
            return 0;
        }
    }

    return 3;
}

int UserManager::getUserByUserId(std::int32_t u_id, User &user) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for(const auto &u : m_users) {
        if(u.u_id == u_id) {
            user = u;
            return 0;
        }
    }
    return 1;
}

int UserManager::getUserByUserId(std::int32_t u_id, User* &user) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for(auto &u : m_users) {
        if(u.u_id == u_id) {
            user = &u;
            return 0;
        }
    }
    return 1;
}

int UserManager::releaseFriendRelationship(std::int32_t u_id, std::int32_t f_id) {
    if(u_id == f_id) {
        return 1;
    }
    if(u_id > f_id) {
        std::int32_t temp = u_id;
        u_id = f_id;
        f_id = temp;
    }

    std::string sql = "DELETE FROM relationship WHERE u_id1=" + std::to_string(u_id) + " AND u_id2=" + std::to_string(f_id); 

    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();
    if(!conn->execute(sql)) {
        LOG_ERROR("delete from mysql error");
        return 2;
    }

    // 解除内存中的联系
    bool op1 = false;
    bool op2 = false;
    std::lock_guard<std::mutex> lock(m_mutex);
    for(auto &user : m_users) {
        if(user.u_id == u_id) {
            for(auto it = user.u_friends.begin(); it != user.u_friends.end(); ++it) {
                if(it->f_id == f_id) {
                    user.u_friends.erase(it);
                    op1 = true;
                    break;
                }
            }
            if(op1) {
                continue;
            }
        }

        if(user.u_id == f_id) {
            for(auto it = user.u_friends.begin(); it != user.u_friends.end(); ++it) {
                if(it->f_id == u_id) {
                    user.u_friends.erase(it);
                    op2 = true;
                    break;
                }
            }
        }
        if(op1 && op2) {
            return 0;
        }
    }

    LOG_ERROR("delete friend memory error");
    return 3;
}

int UserManager::saveChatMsg2DB(std::int32_t s_id, std::int32_t a_id, const std::string &data) {
    std::string sql = "INSERT INTO message(s_id, a_id, content) VALUES(" + std::to_string(s_id) + ", " + std::to_string(a_id) + ", '" + data + "')";

    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();
    if(conn == nullptr) {
        LOG_ERROR("mysql conn is nullptr");
        return 1;
    }
    if(!conn->execute(sql)) {
        LOG_ERROR("error to execute sql:%s", sql.c_str());
        Singleton<MysqlManager>::getInstance().putMysqlConn(conn);
        return 2;
    }

    Singleton<MysqlManager>::getInstance().putMysqlConn(conn);

    return 0;
}

int UserManager::createGroup(const std::string &g_name, std::int32_t u_id, std::int32_t &g_id) {
    g_id = ++m_baseGroupId;

    // 先将消息写到数据库
    std::string sql = "INSERT INTO user(u_id, u_name, u_password, u_nickname, g_ownerid, u_rg_time) "
                        "VALUES(" + std::to_string(g_id) + ", " + std::to_string(g_id) + ", "
                        "'" + g_name + "', ''," + std::to_string(u_id) + ", NOW())";

    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();
    if(!conn) {
        LOG_ERROR("mysqlconn is nullptr");
        return 1;
    } 
    if(!conn->execute(sql)) {
        LOG_ERROR("insert user error:sql=%s", sql.c_str());
        return 2;
    }

    // 同步到内存
    User user;
    user.u_id = g_id;
    user.u_name = std::to_string(g_id);
    user.u_nickname = g_name;
    user.g_ownerid = u_id;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_users.push_back(user);
    return 0;
}

int UserManager::updateUser(std::int32_t u_id, const User &u) {
    // 同步到数据库
    std::string sql = "UPDATE user SET u_nickname='" + u.u_nickname + "', "
                      "u_facetype=" + std::to_string(u.u_faceType) + ", "
                      "u_face='" + u.u_face + "', "
                      "u_gender='" + u.u_gender + "', "
                      "u_birthday=" + std::to_string(u.u_birthday) + ", "
                      "u_signature='" + u.u_signature + "' "
                      "WHERE u_id=" + std::to_string(u_id) + " ";

    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();
    if(conn == nullptr) {
        LOG_ERROR("update user=%d in db error", u_id);
        return 1;
    }
    if(!conn->execute(sql)) {
        LOG_ERROR("error to sql:%s", sql.c_str());
        return 2;
    }

    // 同步到内存
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for(auto &user : m_users) {
            if(user.u_id == u_id) {
                user.u_nickname = u.u_nickname;
                user.u_face = u.u_face;
                user.u_faceType = u.u_faceType;
                user.u_gender = u.u_gender;
                user.u_signature = u.u_signature;
                user.u_birthday = u.u_birthday;
                return 0;
            }
        }
    }

     LOG_INFO("update user=%d in error", u_id);
     return 3;
}

int UserManager::changePassword(std::int32_t u_id, const std::string &pw) {
    // 先更新数据库
    std::string sql = "UPDATE user SET u_password='" + pw + "' WHERE u_id=" + std::to_string(u_id) + " ";
    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();
    if(!conn) {
        LOG_ERROR("mysqlconn is nullptr");
        return 1;
    }
    if(!conn->execute(sql)) {
        LOG_ERROR("error to execute sql:%s", sql.c_str());
        return 2;
    }

    // 同步到内存
    std::lock_guard<std::mutex> lock(m_mutex);
    for(auto &user : m_users) {
        if(user.u_id == u_id) {
            user.u_password = pw;
            LOG_INFO("change user=%d password successful", u_id);
            return 0;
        }
    }

    LOG_ERROR("change user=%d password fail", u_id);
    return 3;
}

int UserManager::getTeamsByUserId(std::int32_t u_id, std::string &teams) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for(const auto &user : m_users) {
        if(user.u_id == u_id) {
            teams = user.u_teaminfo;
            return 0;
        }
    }
    return 1;
}

int UserManager::moveFriend2OtherTeam(std::int32_t u_id, std::int32_t f_id, const std::string &newteam) {
    std::ostringstream sql;
    if (u_id < f_id)
    {
        sql << "UPDATE relationship SET u_id1_team='"
              << newteam << "' WHERE u_id1="
              << u_id << " AND u_id2=" << f_id;
    }
    else
    {
        sql << "UPDATE relationship SET u_id2_team='"
              << newteam << "' WHERE u_id1="
              << f_id << " AND u_id2=" << u_id;
    }

    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();

    if (!conn->execute(sql.str().c_str()))
    {
        return 1;
    }

    LOG_INFO("MoveFriendToOtherTeam db operation successfully, userid: %d, friendid: %d, sql: %s" , u_id, f_id, sql.str().c_str());

    //改变内存中用户的分组信息
    User* u = NULL;
    if (getUserByUserId(u_id, u) || u == NULL)
    {
        return 2;
    }

    for (auto& f : u->u_friends)
    {
        if (f.f_id == f_id)
        {
            f.f_team = newteam;
            return 0;
        }
    }

    return 3;
}

int UserManager::changeFriendRemarks(std::int32_t u_id, std::int32_t f_id, const std::string &newremarks) {
    std::ostringstream sql;
    if (u_id < f_id)
    {
        sql << "UPDATE relationship SET u_id1_remarks='"
            << newremarks << "' WHERE u_id1="
            << u_id << " AND u_id2=" << f_id;
    }
    else
    {
        sql << "UPDATE relationship SET u_id2_remarks='"
            << newremarks << "' WHERE u_id2="
            << u_id << " AND u_id1=" << f_id;
    }

    MysqlConn *conn = Singleton<MysqlManager>::getInstance().getMysqlConn();
    
    if (!conn->execute(sql.str().c_str()))
    {
        LOG_ERROR("Update Markname error, sql: %s", sql.str().c_str());
        Singleton<MysqlManager>::getInstance().putMysqlConn(conn);
        return 1;
    }
    Singleton<MysqlManager>::getInstance().putMysqlConn(conn);

    LOG_INFO("update markname successfully, userid: %d, friendid: %d, sql: %s", u_id, f_id, sql.str().c_str());

    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& user : m_users)
    {
        if (user.u_id == u_id)
        {
            for (auto& f : user.u_friends)
            {
                if (f.f_id == f_id)
                {
                    f.f_remarks = newremarks;
                    return 0;
                }
            }          
        }
    }

    LOG_ERROR("Failed to update markname, find no exsit user in memory error, m_allCachedUsers.size(): %d, userid: %d, friendid: %d, sql: %s",
         m_users.size(), u_id, f_id, sql.str().c_str());

    return 2;
}

}   // namespace IMServer

}    // namespace YTalk