/**
 * @file MysqlManager.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_MYSQL_MANAGER_H
#define YTALK_MYSQL_MANAGER_H

#include "MysqlConn.h"     

#include <string>
#include <list>
#include <mutex>
#include <condition_variable>

#include <cstdint>

namespace YTalk
{

namespace mysql
{

class MysqlConn;

class MysqlManager
{
public:
    MysqlManager();
    ~MysqlManager();

    int init(const std::string &host, std::uint16_t port, 
             const std::string &user, const std::string &pw, 
             const std::string &dbName);

    MysqlConn* getMysqlConn();

    void putMysqlConn(MysqlConn *ptr);

public:
    std::string getHost() {
        return m_host;
    }

    std::uint16_t getPort() {
        return m_port;
    }

    std::string getDbName() {
        return m_dbName;
    }

    std::string getUser() {
        return m_user;
    }

    std::string getPW() {
        return m_pw;
    }

    bool getFlag() {
        return m_flag;
    }

private:
    std::string                             m_host;             // mysql的IP地址
    std::uint16_t                           m_port;             //
    std::string                             m_user;
    std::string                             m_pw;
    std::string                             m_dbName;           // 数据库名
    int                                     m_maxConnCount;     
    int                               m_initConnCount;
    std::mutex                              m_mutex;
    std::condition_variable                 m_cond;

    std::list<MysqlConn*>                   m_mysqlConns;

    bool                                    m_flag;

};   // class MysqlManager

}  // namespace mysql

}  // namespace YTalk

#endif /// YTALK_MYSQL_MANAGER_H