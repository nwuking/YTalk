/**
 * @file User.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_USER_H
#define YTALK_USER_H

#include <cstdint>
#include <string>
#include <list>
#include <vector>

namespace YTalk
{

namespace IMServer
{

typedef struct Friend {
    std::int32_t            f_id;           // 好友id
    std::string             f_remarks;     // 好友备注
    std::string             f_team;         // 好友在哪个分组
} Friend;

typedef struct User {
    std::int32_t            u_id;           // 0xofffffff以上是群号，以下是用户
    std::string             u_name;         // 用户名
    std::string             u_password;     // 用户密码
    std::string             u_nickname;     // 用户昵称或者群名字
    std::string             u_gender;       // 用户性别
    std::string             u_signature;    // 用户签名
    std::int32_t            u_birthday;     // 用户生日
    std::string             u_teaminfo;     // 用户分组信息，群则为空
    std::int32_t            g_ownerid;      // 群主id
    std::int32_t            u_faceType;     // 头像类型
    std::string             u_face;         // 头像
    std::list<Friend>       u_friends;      // 好友列表
} User;

enum ClientType {
    CLIENT_TYPE_UNKNOW = 0, 
    CLIENT_TYPE_PC = 1,
    CLIENT_TYPE_ANDROID = 2,
    CLIENT_TYPE_IOS = 3
};

typedef struct OnlineUserInfo {
    std::int32_t            u_id;           // 用户id
    std::string             u_name;
    std::string             u_nickname;
    std::int32_t            u_clientType;   // 客户端类型，2为Android端
    std::int32_t            u_status;       // 在线状态，0离线 1在线 2忙碌 3离开 4隐身
} OnlineUserInfo;

}  // namespace IMServer

}  /// namespace YTalk

#endif  // YTALK_USER_H