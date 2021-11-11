/*================================================================================
 *    Date:
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_DB_H
#define YTALK_DB_H

#include <mysql/mysql.h>    //mysql C API

namespace YTalk
{

class DBResultSet
{
public:
    DBResultSet(MYSQL_RES *res);
    ~DBResultSet();

private:
    MYSQL_RES *_res;
};    // class DBResultSet

class DBConn
{
public:
    DBConn();
    ~DBConn();

    int init();
    DBResultSet* executeQuery(const char *query);

private:
    MYSQL *_mysql;

};    // class DBConn

}    // namespace YTalk

#endif