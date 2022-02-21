/**
 * @file MysqlManager.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "MysqlManager.h"
#include "MysqlConn.h"

#include <memory>


#define MAX_CONN_COUNT 16
#define INIT_CONN_COUNT 8

namespace YTalk
{

namespace mysql
{

MysqlManager::MysqlManager() :
    m_maxConnCount(MAX_CONN_COUNT), 
    m_initConnCount(INIT_CONN_COUNT), 
    m_mutex(),
    m_cond(), 
    m_flag(false)
{
    //
}

MysqlManager::~MysqlManager() {
    for(std::list<MysqlConn*>::iterator it = m_mysqlConns.begin(); 
        it != m_mysqlConns.end(); ++it)
    {
        MysqlConn *p = *it;
        delete p;
    }
    m_mysqlConns.clear();
}

int MysqlManager::init( const std::string &host, 
                        std::uint16_t port, 
                        const std::string &user, 
                        const std::string &pw, 
                        const std::string &dbName) 
{
    if(m_flag) {
        return 0;
    }

    m_host = host;
    m_port = port;
    m_user = user;
    m_pw = pw;
    m_dbName = dbName;

    for(int i = 0; i < m_initConnCount; ++i) {
        MysqlConn *conn = new MysqlConn(this);
        if(conn->init()) {
            delete conn;
            return 1;
        }

        m_mysqlConns.push_back(conn);
    }
    m_flag = true;
    return 0;
}

MysqlConn* MysqlManager::getMysqlConn() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_mysqlConns.empty()) {
        if(m_initConnCount >= m_maxConnCount) {
            m_cond.wait(lock);
        }
        else {
            MysqlConn *conn = new MysqlConn(this);
            if(conn->init()) {
                delete conn;
                return nullptr;
            }
            m_mysqlConns.push_back(conn);
            m_initConnCount++;
        }
    }

    MysqlConn *p = m_mysqlConns.front();
    m_mysqlConns.pop_front();
    return p;
}

void MysqlManager::putMysqlConn(MysqlConn *conn) {
    std::unique_lock<std::mutex> lock(m_mutex);
    std::list<MysqlConn*>::iterator it = m_mysqlConns.begin();
    for(; it != m_mysqlConns.end(); ++it) {
        if(*it == conn) {
            break;
        }
    }

    if(it == m_mysqlConns.end()) {
        m_mysqlConns.push_back(conn);
    }

    m_cond.notify_one();
}

}  // namespace mysql

}  // namespace YTalk