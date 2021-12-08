/*================================================================================   
 *    Date: 2021-12-08
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "base/Condition.h"
#include "base/Mutex.h"

#include <string>
#include <list>

namespace YTalk
{

class MySqlConn;

class MySqlPool
{
public:
    MySqlPool(const std::string &pool_name, const std::string &db_host, uint16_t db_port,
        const std::string &db_username, const std::string &db_password, const std::string &db_name, int maxconncnt);
    virtual ~MySqlPool();

    int init();

    MySqlConn* getMySqlConn();

    void retMySqlConn(MySqlConn *mySqlConn);

    const char* getPoolName() {
        return _pool_name.c_str();
    }
    const char* getDBHost() {
        return _db_host.c_str();
    }
    uint16_t getDBPort() {
        return _db_port;
    }
    const char* getDBUserName() {
        return _db_username.c_str();
    }
    const char* getDBPassword() {
        return _db_password.c_str();
    }
    const char* getDBName() {
        return _db_name.c_str();
    }
    //TODO
private:
    std::string _pool_name;
    std::string _db_host;
    uint16_t _db_port;
    std::string _db_username;
    std::string _db_password;
    std::string _db_name;
    int _db_maxconncnt;
    int _db_curconncnt;
    std::list<MySqlConn*> _free_list;

    Mutex _mutex;
    Cond _cond;
};    // class MysqlPool

}    // namespace YTalk