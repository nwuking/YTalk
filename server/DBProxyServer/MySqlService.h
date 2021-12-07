/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

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

    virtual void request(::google::protobuf::RpcController* controller,
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