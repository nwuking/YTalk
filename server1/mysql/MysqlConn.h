/**
 * @file MysqlConn.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_MYSQL_CONN_H
#define YTALK_MYSQL_CONN_H

#include <mysql/mysql.h>
#include <string>
#include <unordered_map>

namespace YTalk
{

namespace mysql
{

class MysqlManager;

class MResultSet
{
public:
    MResultSet(MYSQL_RES *res);
    ~MResultSet();

    bool next();
    int getInt(const std::string &key);
    std::string getString(const std::string &key);

private:
    int getIndex(const std::string &key);

private:
    MYSQL_RES*                              m_res;
    MYSQL_ROW                               m_row;
    std::unordered_map<std::string, int>    m_map;

};   // class MResultSet;

class MysqlConn
{
public:
    MysqlConn(MysqlManager *manager);
    ~MysqlConn();

    int init();

    MResultSet* query(const std::string &sql);
    bool execute(const std::string &sql);

private:
    MysqlManager*               m_manager;
    MYSQL*                      m_mysql;

};   // class MysqlConn

}   /// namespace mysql

}  // namespace YTalk

#endif /// YTALK_MYSQL_CONN_H