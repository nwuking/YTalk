/*================================================================================   
 *    Date: 2021-12-08
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
      _db_maxconncnt(maxconncnt),
      _mutex(),
      _cond(_mutex)
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

MySqlConn* MySqlPool::getMySqlConn() {
    MutexLock lock(_mutex);
    while(_free_list.empty()) {
        if(_db_curconncnt >= _db_maxconncnt) {
            _cond.wait();
        }
        else {
            MySqlConn *newMySqlConn = new MySqlConn(this);
            if(newMySqlConn->init()) {
                LOG(INFO) << "Init MysqlConn failed";
                delete newMySqlConn;
                return nullptr;
            }
            _free_list.push_back(newMySqlConn);
            _db_curconncnt++;
            LOG(INFO) << "new MySqlConn: " << _pool_name << ", conn_cnt: " << _db_curconncnt;
        }
    }

    MySqlConn *p = _free_list.front();
    _free_list.pop_front();
    return p;
}

void MySqlPool::retMySqlConn(MySqlConn *mySqlConn) {
    MutexLock lock(_mutex);
    std::list<MySqlConn*>::iterator it = _free_list.begin();
    for(; it != _free_list.end(); ++it) {
        if(*it == mySqlConn) {
            break;
        }
    }

    if(it == _free_list.end()) {
        _free_list.push_back(mySqlConn);
    }

    _cond.notify();
}
//TODO
}    //namesapce YTalk