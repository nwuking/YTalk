/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "MySqlConn.h"
#include "MySqlPool.h"
#include "base/Logging.h"

#include <string.h>

namespace YTalk
{

MResultSet::MResultSet(MYSQL_RES *res) {
    _res = res;
    int num_fields = mysql_num_fields(_res);     // 每一行的字段数
    MYSQL_FIELD *fields = mysql_fetch_fields(_res);
    for(int i = 0; i < num_fields; ++i) {
        _key_map.insert(std::make_pair(fields[i].name, i));
    }
}

MResultSet::~MResultSet() {
    if(_res) {
        mysql_free_result(_res);
        _res = nullptr;
    }
}

int MResultSet::_getIndex(const char *key) {
    std::unordered_map<std::string, int>::iterator it = _key_map.find(key);
    if(it != _key_map.end()) {
        return it->second;
    }
    return -1;
}

bool MResultSet::next() {
    _row = mysql_fetch_row(_res);
    if(_row) {
        return true;
    }
    return false;
}

int MResultSet::getInt(const char *key) {
    int index = _getIndex(key);
    if(index == -1) {
        return 0;
    }
    return atoi(_row[index]);
}

char* MResultSet::getString(const char *key) {
    int index = _getIndex(key);
    if(index == -1) {
        return nullptr;
    }
    return _row[index];
}

////////////////////////////////////////////////////////////////

MySqlConn::MySqlConn(MySqlPool *mySqlPool) {
    _mySqlPool = mySqlPool;
    _mysql = nullptr;
}

MySqlConn::~MySqlConn() {
    if(!_mysql) {
        mysql_close(_mysql);
        _mysql = nullptr;
    }
}

int MySqlConn::init() {
    _mysql = mysql_init(nullptr);
    if(!_mysql) {
        LOG(INFO) << "mysql_init failed";
        return 1;
    }

    my_bool reconnect = true;
    mysql_options(_mysql, MYSQL_OPT_RECONNECT, &reconnect);

    if(!mysql_real_connect(_mysql, _mySqlPool->getDBHost(), _mySqlPool->getDBUserName(), _mySqlPool->getDBPassword(),
                        _mySqlPool->getDBName(), _mySqlPool->getDBPort(), nullptr, 0))
    {
        LOG(INFO) << "mysql_real_connect failed: " << mysql_error(_mysql);
        return 2;
    }
    return 0;
}

MResultSet* MySqlConn::executeQuery(const char *query) {
    mysql_ping(_mysql);

    if(mysql_real_query(_mysql, query, ::strlen(query))) {
        LOG(INFO) << "mysql_real_query failed: " << mysql_error(_mysql) << ", sql: " << query;
        return nullptr;
    }

    MYSQL_RES *res = mysql_store_result(_mysql);
    if(!res) {
        LOG(INFO) << "mysql_store_result failed: " << mysql_error(_mysql);
        return nullptr;
    }

    MResultSet *resultSet = new MResultSet(res);
    return resultSet;
}

bool MySqlConn::executeUpdate(const char *update) {
    mysql_ping(_mysql);

    if(mysql_real_query(_mysql, update, ::strlen(update))) {
        LOG(INFO) << "mysql_real_query failed: " << mysql_error(_mysql) << ", sql: " << update;
        return false;
    }

    if(mysql_affected_rows(_mysql) > 0) {
        return true;
    }
    return false;
}

}    // namesapce YTalk