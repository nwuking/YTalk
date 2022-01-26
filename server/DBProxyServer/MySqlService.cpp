/*================================================================================   
 *    Date: 2021-12-09
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "MySqlPool.h"
#include "MySqlService.h"
#include "MySqlConn.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"
#include "rapidjson/document.h"
#include "base/structs.h"

#include <brpc/server.h>

//const std::string DBINSTANCES_KEY = "dbInstances";
#define DBINSTANCES_KEY "DBInstances"
#define MYSQL_QUERY "query"
#define MYSQL_UPDATE "update"

// for json
#define USERNAME "username"
#define PASSWORD "password"

/////
#define USERACCOUNT "YTalk"
#define USER "YTalk"

namespace YTalk
{

MySqlServiceImpl::~MySqlServiceImpl() {
    for(auto &p : _MysqlPool_map) {
        delete p.second;
        p.second = nullptr;
    }
}

void MySqlServiceImpl::Login(::google::protobuf::RpcController* controller,
                            const ::DBProxyServer::MySqlRequest* request,
                            ::DBProxyServer::MySqlResponse* response,
                            ::google::protobuf::Closure* done)
{
    ::brpc::ClosureGuard done_guard(done);
    ::brpc::Controller *cntl = static_cast<::brpc::Controller*>(controller);

    rapidjson::Document document;
    document.Parse(request->message().c_str());
    if(!document.HasMember(USERNAME) || !document.HasMember(PASSWORD)) {
        response->set_status(LOGIN_JSON_DEFECT_U_OR_P);
        return;
    }

    std::string username = document[USERNAME].GetString();
    std::string password = document[PASSWORD].GetString();

    std::unordered_map<std::string, MySqlPool*>::iterator it = _MysqlPool_map.find(USERACCOUNT);
    if(it == _MysqlPool_map.end()) {
        LOG(ERROR) << "Defect db: " << USERACCOUNT;
        response->set_status(LOGIN_SERVER_ERROR);
        return;
    }
    MySqlPool *userAccount = it->second;
    MySqlConn *conn = userAccount->getMySqlConn();
    if(!conn) {
        LOG(ERROR) << "MySqlConn unuseful in:" << userAccount->getDBName();
        response->set_status(LOGIN_SERVER_ERROR);
        return;
    }

    std::string query = "select User_password from UserAccount where User_name=\"" + username+"\"";
    MResultSet *set = conn->executeQuery(query.c_str());
    if(!set) {
        LOG(ERROR) << "MResultSet is nullptr";
        response->set_status(LOGIN_SERVER_ERROR);
        userAccount->retMySqlConn(conn);
        return;
    }

    std::string p;
    while(set->next()) {
        p = set->getString("User_password");
        if(p == password) {
            break;    
        }
    }
    if(p == password) {
        response->set_status(LOGIN_SUCCESS);
    }
    else {
        response->set_status(LOGIN_FAIL);
    }

    MResultSet::freeMResultSet(set);
    userAccount->retMySqlConn(conn);
}

void MySqlServiceImpl::Register(::google::protobuf::RpcController* controller,
                            const ::DBProxyServer::MySqlRequest* request,
                            ::DBProxyServer::MySqlResponse* response,
                            ::google::protobuf::Closure* done)
{
    ::brpc::ClosureGuard done_guard(done);
    ::brpc::Controller *cntl = static_cast<::brpc::Controller*>(controller);
    //////TODO
}

void MySqlServiceImpl::GetMaxUserId(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::MySqlRequest* request,
                       ::DBProxyServer::MySqlResponse* response,
                       ::google::protobuf::Closure* done)
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);

    std::unordered_map<std::string, MySqlPool*>::iterator it = _MysqlPool_map.find(USER);
    if(it == _MysqlPool_map.end()) {
        LOG(ERROR) << "Defect db: " << USER;
        response->set_status(DBPROXY_DEFECT_DB);
        return;
    }

    MySqlPool *pool = it->second;
    MySqlConn *conn = pool->getMySqlConn();
    if(!conn) {
        LOG(ERROR) << "MySqlConn unuseful in:" << pool->getDBName();
        response->set_status(DBPROXY_CONN_ERROR);
        return;
    }

    std::string query = "";
    //TODO
}

int MySqlServiceImpl::init(const std::string &configFile) {
    ConfigParse *cParse = new ConfigParse;
    if(!cParse) {
        LOG(ERROR) << "new ConfigParse failed";
        delete cParse;
        return 1;
    }
    cParse->parse(configFile);
    if(!cParse->status()) {
        LOG(ERROR) << "The configuration file could not be resolved in MysqlServiceImpl::init";
        delete cParse;
        return 2;
    }

    int s = init(cParse);
    delete cParse;
    return s;
}

int MySqlServiceImpl::init(ConfigParse *cParse) {
    std::vector<std::string> dbInstances;
    cParse->getValue(DBINSTANCES_KEY, dbInstances);
    if(dbInstances.empty()) {
        LOG(INFO) << "not configure DBInstances";
        return 3;
    }

    char host[64];
    char port[64];
    char dbname[64];
    char username[64];
    char password[64];
    char maxconncnt[64];

    for(int i = 0; i < dbInstances.size(); ++i) {
        snprintf(host, 64, "%s_host", dbInstances[i].c_str());
        snprintf(port, 64, "%s_port", dbInstances[i].c_str());
        snprintf(dbname, 64, "%s_dbname", dbInstances[i].c_str());
        snprintf(username, 64, "%s_username", dbInstances[i].c_str());
        snprintf(password, 64, "%s_password", dbInstances[i].c_str());
        snprintf(maxconncnt, 64, "%s_maxconncnt", dbInstances[i].c_str());

        std::string db_host, db_port_str, db_dbname, db_username, db_password, db_maxconncnt_str;
        cParse->getValue(host, db_host);
        cParse->getValue(port, db_port_str);
        cParse->getValue(dbname, db_dbname);
        cParse->getValue(username, db_username);
        cParse->getValue(password, db_password);
        cParse->getValue(maxconncnt, db_maxconncnt_str);
        if(db_host.empty() || db_port_str.empty() || db_dbname.empty() || db_username.empty() || db_password.empty(), db_maxconncnt_str.empty()) {
            LOG(INFO) << "not configure db instance: " << dbInstances[i];
            return 4;
        }

        int db_port = std::stoi(db_port_str);
        int db_maxconncnt = std::stoi(db_maxconncnt_str);

        MySqlPool *mySqlPool = new MySqlPool(dbInstances[i], db_host, db_port, db_username, db_password, db_dbname, db_maxconncnt);
        if(mySqlPool->init()) {
            LOG(ERROR) << "init db instance failed: " << dbInstances[i];
            return 5;
        }

        _MysqlPool_map.insert(std::make_pair(dbInstances[i], mySqlPool));
    }
    return 0;
}
//TODO

}    // namespace YTalk