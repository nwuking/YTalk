/*================================================================================   
 *    Date: 2021-12-08
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_MYSQLCONN_H
#define YTALK_MYSQLCONN_H

#include <mysql/mysql.h>
#include <unordered_map>
#include <string>

//for test
#include "base/Logging.h"

namespace YTalk
{

class MySqlPool;

class MResultSet
{
public:
    MResultSet(MYSQL_RES *res);
    virtual ~MResultSet();

    bool next();
    int getInt(const char *key);
    char* getString(const char *key);
    //TODO

    /// for test
    bool ergodic() {
        std::unordered_map<std::string, int>::iterator it = _key_map.begin();
        if(it == _key_map.end()) {
            return false;
        }
        for(; it != _key_map.end(); ++it) {
            LOG(INFO) << "SQL RESULT: " << _row[it->second];
        }
    }
    /// for test end

private:
    int _getIndex(const char *key);

    MYSQL_RES *_res;
    MYSQL_ROW _row;
    std::unordered_map<std::string, int> _key_map;
};    // class MResultSet

class MySqlConn
{
public:
    MySqlConn(MySqlPool *mysqlPool);
    virtual ~MySqlConn();

    int init();

    MResultSet* executeQuery(const char *query);
    bool executeUpdate(const char *update);
    //TODO

private:
    MySqlPool *_mySqlPool;
    MYSQL *_mysql;
};    // class MySqlConn

}    // namespace YTalk

#endif