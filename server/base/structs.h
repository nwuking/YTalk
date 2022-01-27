/*================================================================================   
 *    Date: 2021-12-16
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_STRUCTS_H
#define YTALK_STRUCTS_H

#include <string>

namespace YTalk
{

typedef struct {
    int flag;
    std::string msg;
} GateConText;

enum MySqlStatus {
    MYSQL_SUCCESS = 0,
    MYSQL_DB_NOT_EXITS,
    MYSQL_CONN_IS_NULLPTR,
    MYSQL_UPDATE_SUCCESS,
    MYSQL_UPDATE_FAIL,
    MYSQL_OPTION_NOT_USE,
};
enum DBProxyStatus {
    DBPROXY_SUCCESS = 0,
    DBPROXY_SERVER_ERROR,
    DBPROXY_CLIENT_ERROR,
    DBPROXY_FAIL
};

enum LoginStatus {
    LOGIN_SUCCESS = 0,
    LOGIN_FAIL,
    LOGIN_JSON_DEFECT_U_OR_P,
    LOGIN_SERVER_ERROR,
    LOGIN_RPC_FAIL,
    LOGIN_CLIENT_ERROR
};

enum RegisterStatus {
    REGISTER_SUCCESS = 0,
    REGISTER_CLIENT_ERROR
};

enum IMFLAG {
    FLAG_IM = 1
};

enum SERVERFLAG {
    FLAG_GATE_SERVER = 1,
    FLAG_IM_SERVER
};

enum ROUTE_STATUS {
    ROUTE_STATUS_OK = 0,
    ROUTE_STATUS_FAIL
};

enum IM_STATUS {
    IM_STATUS_OK = 0,
    IM_STATUS_FAIL
};

enum GATE_STATUS {
    GATE_STATUS_OK = 0,
    GATE_STATUS_FAIL
};

struct UserRegisterInfo {
    int u_id;
    std::string u_name;
    std::string u_nickname;
    std::string u_password;
    std::string u_gender;
    int u_birthday;
    //std::string u_signature;
    //TODO
};

struct NewUserInfo {
    int status;
    int u_id;
    //TODO
};

#define U_ID "u_id"
#define U_NAME "u_name"
#define U_NICKNAME "u_nickname"
#define U_PASSWORD "u_password"
#define U_GENDER "u_gender"
#define U_BIRTHDAY "u_birthday"
#define U_SIGNATURE "u_signature"

}     /// namespace YTalk

#endif