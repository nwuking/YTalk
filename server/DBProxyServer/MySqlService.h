/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_MYSQLSERVICE_H
#define YTALK_MYSQLSERVICE_H

#include "protobuf/mysql.pb.h"

#include <unordered_map>
#include <string>

/// for test
#include "base/Logging.h"
#include "MySqlConn.h"
#include "MySqlPool.h"

namespace YTalk 
{

class MySqlPool;
class ConfigParse;

class MySqlServiceImpl : public ::DBProxyServer::MySqlService
{
public:
    MySqlServiceImpl() = default;
    virtual ~MySqlServiceImpl();

    virtual void Request(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::MySqlRequest* request,
                       ::DBProxyServer::MySqlResponse* response,
                       ::google::protobuf::Closure* done); 

    int init(const std::string &configFile);
    int init(ConfigParse *cParse);
    //TODO

    /// for test
    void update(const std::string &pool, const char *sql) {
        std::unordered_map<std::string, MySqlPool*>::iterator it = _MysqlPool_map.find(pool);
        if(it != _MysqlPool_map.end()) {
            MySqlConn *p = it->second->getMySqlConn();
            if(p->executeUpdate(sql)) {
                LOG(INFO) << "update successful";
            }
            else {
                LOG(INFO) << "update failed";
            }

            it->second->retMySqlConn(p);
        }
    }
    void query(const std::string &pool, const char *sql) {
        std::unordered_map<std::string, MySqlPool*>::iterator it = _MysqlPool_map.find(pool);
        if(it != _MysqlPool_map.end()) {
            MySqlConn *p = it->second->getMySqlConn();
            MResultSet *set = p->executeQuery(sql);

            if(set) {
                if(!set->ergodic()) {
                    LOG(INFO) << "query no data";
                }
            }
            else {
                LOG(INFO) << "query failed: no data";
            }

            it->second->retMySqlConn(p);
            delete set;
        }
    }   
    /// for test end
private:
    std::unordered_map<std::string, MySqlPool*> _MysqlPool_map;
    //TODO
};    // class MysqlServiceImpl
//TODO

}    // namespace YTalk

#endif