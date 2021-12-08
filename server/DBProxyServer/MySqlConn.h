/*================================================================================   
 *    Date: 2021-12-08
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <mysql/mysql.h>
#include <unordered_map>
#include <string>

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
    //TODO

private:
    MySqlPool *_mySqlPool;
    MYSQL *_mysql;
};    // class MySqlConn

}    // namespace YTalk