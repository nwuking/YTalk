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
#define GETMAXUID_KEY "u_id"

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

    rapidjson::Document document;
    document.Parse(request->message().c_str());
/*{
    "u_id": 11111,
    "u_name": "181764****0",
    "u_nickname": "nwuking",
    "u_password": "************",
    "u_gender": "1",
    "u_birthday": 20220101,
}*/
    if(!document.HasMember(U_ID) || !document.HasMember(U_NAME) || !document.HasMember(U_NICKNAME) || !document.HasMember(U_PASSWORD)) {
        LOG(ERROR) << "Bad request";
        response->set_status(DBPROXY_CLIENT_ERROR);
        return;
    }
    int u_id = document[U_ID].GetInt();
    std::string u_name = document[U_NAME].GetString();
    std::string u_nickname = document[U_NICKNAME].GetString();
    std::string u_password = document[U_PASSWORD].GetString();
    std::string u_gender = document[U_GENDER].GetString();
    int u_birthday = document[U_BIRTHDAY].GetInt();
    //std::string u_signature = document[U_SIGNATURE].GetString();

    std::unordered_map<std::string, MySqlPool*>::iterator it = _MysqlPool_map.find(USER);
    if(it == _MysqlPool_map.end()) {
        LOG(ERROR) << "Defect db: " << USER;
        response->set_status(DBPROXY_SERVER_ERROR);
        return;
    }
    MySqlPool *pool = it->second;
    MySqlConn *conn = pool->getMySqlConn();
    if(!conn) {
        LOG(ERROR) << "MySqlConn unuseful in:" << pool->getDBName();
        response->set_status(DBPROXY_SERVER_ERROR);
        return;
    }

    char query[256] = {0};  
    snprintf(query, 256, "INSERT INTO user(u_id, u_name, u_nickname, u_password, u_gender, u_birthday, u_rg_time) "
                        "VALUES(%d, '%s', '%s', '%s', '%s', %d, NOW())", u_id, u_name.c_str(), u_nickname.c_str(), 
                                                              u_password.c_str(), u_gender.c_str(), u_birthday);
    if(!conn->executeUpdate(query)) {
        LOG(ERROR) << "insert user err: " << query;
        response->set_status(DBPROXY_SERVER_ERROR);
    }
    else {
        LOG(INFO) << "insert user success: " << query;
        response->set_status(DBPROXY_SUCCESS);
    }

    pool->retMySqlConn(conn);
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
        response->set_status(DBPROXY_SERVER_ERROR);
        return;
    }

    MySqlPool *pool = it->second;
    MySqlConn *conn = pool->getMySqlConn();
    if(!conn) {
        LOG(ERROR) << "MySqlConn unuseful in:" << pool->getDBName();
        response->set_status(DBPROXY_SERVER_ERROR);
        return;
    }

    std::string query = "select MAX(u_id) as u_id from user";
    MResultSet *set = conn->executeQuery(query.c_str());
    if(!set) {
        LOG(ERROR) << "MResultSet is nullptr";
        response->set_status(DBPROXY_SERVER_ERROR);
        pool->retMySqlConn(conn);
        return;
    }

    if(set->next()) {
        char* u_id = set->getString(GETMAXUID_KEY);
        if(u_id == nullptr) {
            response->set_message("0");
        }
        else {
            response->set_message(u_id);
        }
    }
    response->set_status(DBPROXY_SUCCESS);

    MResultSet::freeMResultSet(set);
    pool->retMySqlConn(conn);
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