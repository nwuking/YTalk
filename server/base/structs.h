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

enum MySqlStatus {
    MYSQL_SUCCESS = 0,
    MYSQL_DB_NOT_EXITS,
    MYSQL_CONN_IS_NULLPTR,
    MYSQL_UPDATE_SUCCESS,
    MYSQL_UPDATE_FAIL,
    MYSQL_OPTION_NOT_USE,
};

enum LoginStatus {
    LOGIN_SUCCESS = 0,
    LOGIN_FAIL,
    LOGIN_JSON_DEFECT_U_OR_P,
    LOGIN_SERVER_ERROR,
    LOGIN_RPC_FAIL,
    LOGIN_CLIENT_ERROR
};

enum IMFLAG {
    FLAG_IM = 1
};

enum SERVERFLAG {
    FLAG_GATE_SERVER = 1,
    FLAG_IM_SERVER
};

struct UserRegisterInfo {
    std::string password;
    //TODO
};


}     /// namespace YTalk

#endif