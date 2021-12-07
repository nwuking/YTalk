/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "MySqlPool.h"
#include "MySqlConn.h"
#include "base/Logging.h"

#define DB_MIN_CONN_CNT 2

namespace YTalk
{
MySqlPool::MySqlPool(const std::string &pool_name, const std::string &db_host, uint16_t db_port,
    const std::string &db_username, const std::string &db_password, const std::string &db_name, int maxconncnt)
    : _pool_name(pool_name), 
      _db_host(db_host),
      _db_port(db_port),
      _db_username(db_username),
      _db_password(db_password),
      _db_name(db_name),
      _db_maxconncnt(maxconncnt)
{
     _db_curconncnt = DB_MIN_CONN_CNT;
}

MySqlPool::~MySqlPool() {
    for(std::list<MySqlConn*>::iterator it = _free_list.begin(); it != _free_list.end(); ++it) {
        MySqlConn *p = *it;
        delete p;        
    }

    _free_list.clear();
}

int MySqlPool::init() {
    for(int i = 0; i < _db_curconncnt; ++i) {
        MySqlConn *mySqlConn = new MySqlConn(this);
        if(mySqlConn->init()) {
            delete mySqlConn;
            return 1;
        }

        _free_list.push_back(mySqlConn);
    }

    LOG(INFO) << "db pool: " << _pool_name << ", size: " << _free_list.size();
    return 0;
}

    //TODO
}    //namesapce YTalk