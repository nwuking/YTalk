/**
 * @file MysqlConn.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "MysqlConn.h"
#include "MysqlManager.h"
#include "../base/Logger.h"

#include <cstring>

namespace YTalk
{
using namespace base;
namespace mysql
{

MResultSet::MResultSet(MYSQL_RES *res) {
    m_res = res;
    int num_fields = mysql_num_fields(m_res);     // 每一行的字段数
    MYSQL_FIELD *fields = mysql_fetch_fields(m_res);
    for(int i = 0; i < num_fields; ++i) {
        m_map.insert(std::make_pair(fields[i].name, i));
    }
}

MResultSet::~MResultSet() {
    if(m_res) {
        mysql_free_result(m_res);
        m_res = nullptr;
    }
}

int MResultSet::getIndex(const std::string &key) {
    std::unordered_map<std::string, int>::iterator it = m_map.find(key);
    if(it != m_map.end()) {
        return it->second;
    }
    return -1;
}

bool MResultSet::next() {
    m_row = mysql_fetch_row(m_res);
    if(m_row) {
        return true;
    }
    return false;
}

int MResultSet::getInt(const std::string &key) {
    int index = getIndex(key);
    if(index == -1) {
        return 0;
    }
    return atoi(m_row[index]);
}

std::string MResultSet::getString(const std::string &key) {
    int index = getIndex(key);
    if(index == -1 || m_row[index] == nullptr) {
        return std::string();
    }
    return m_row[index];
}

/**************  MResultSet end  ********************/

MysqlConn::MysqlConn(MysqlManager *manager) {
    m_manager = manager;
    m_mysql = nullptr;
}

MysqlConn::~MysqlConn() {
    if(!m_mysql) {
        mysql_close(m_mysql);
        m_mysql = nullptr;
    }
}

int MysqlConn::init() {
    m_mysql = mysql_init(nullptr);
    if(!m_mysql) {
        return 1;
    }

    my_bool reconnect = true;
    mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &reconnect);

    if(!mysql_real_connect(m_mysql, m_manager->getHost().c_str(), 
                                    m_manager->getUser().c_str(), 
                                    m_manager->getPW().c_str(),
                                    m_manager->getDbName().c_str(),
                                    m_manager->getPort(), 
                                    nullptr, 0))
    {
        return 2;
    }
    return 0;
}


MResultSet* MysqlConn::query(const std::string &sql) {
    mysql_ping(m_mysql);

    if(mysql_real_query(m_mysql, sql.c_str(), sql.size())) {
        //LOG(INFO) << "mysql_real_query failed: " << mysql_error(_mysql) << ", sql: " << query;
        return nullptr;
    }

    MYSQL_RES *res = mysql_store_result(m_mysql);
    if(!res) {
        //LOG(INFO) << "mysql_store_result failed: " << mysql_error(_mysql);
        return nullptr;
    }

    MResultSet *resultSet = new MResultSet(res);
    return resultSet;
}

bool MysqlConn::execute(const std::string &sql) {
    mysql_ping(m_mysql);

    if(mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0) {
        //LOG(INFO) << "mysql_real_query failed: " << mysql_error(_mysql) << ", sql: " << update;
        return false;
    }

    if(mysql_affected_rows(m_mysql) > 0) {
        return true;
    }

    return false;
}

}   /// namespace mysql

}  // namespace YTalk