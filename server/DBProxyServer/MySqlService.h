/*================================================================================   
 *    Date: 2021-12-17
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_MYSQLSERVICE_H
#define YTALK_MYSQLSERVICE_H

#include "protobuf/mysql.pb.h"

#include <unordered_map>
#include <string>

namespace YTalk 
{

class MySqlPool;
class ConfigParse;

class MySqlServiceImpl : public ::DBProxyServer::MySqlService
{
public:
    MySqlServiceImpl() = default;
    virtual ~MySqlServiceImpl();

    virtual void Login(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::MySqlRequest* request,
                       ::DBProxyServer::MySqlResponse* response,
                       ::google::protobuf::Closure* done); 

    virtual void Register(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::MySqlRequest* request,
                       ::DBProxyServer::MySqlResponse* response,
                       ::google::protobuf::Closure* done);

    int init(const std::string &configFile);
    int init(ConfigParse *cParse);
    //TODO
private:
    std::unordered_map<std::string, MySqlPool*> _MysqlPool_map;
    //TODO
};    // class MysqlServiceImpl
//TODO

}    // namespace YTalk

#endif