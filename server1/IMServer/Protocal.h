/**
 * @file Protocal.h
 * @author nwuking@qq.com
 * @brief 通信协议
 * @version 0.1
 * @date 2022-02-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_PROTOCAL_H
#define YTALK_PROTOCAL_H

#include <stdint.h>

namespace YTalk
{

namespace IMServer
{

/**
 * @brief 数据包的头部
 * 
 */
typedef struct PackageHead {
    char        ph_compress;            // "1"为压缩，"0"为不压缩
    int         ph_compress_size;       // 如果压缩了则为压缩后的大小，否则为0
    int         ph_src_size;            // 压缩前的大小
} PackageHead;

/**
 * @brief 消息指令
 * 
 */
enum MsgOrder {
    MSG_ORDER_UNKNOW = 0, 
    MSG_ORDER_REGISTER,                             // 注册
    MSG_ORDER_LOGIN,                                // 登录
    MSG_ORDER_KICK,                                 // 踢人
    MSG_ORDER_HEARTBEAT,                            // 心跳包
    MSG_ORDER_FIND_FRIEND,                          // 查找好友
    MSG_ORDER_ADD_FRIEND,                           // 加好友或者群
    MSG_ORDER_DEL_FRIEND,                           // 删好友
    MSG_ORDER_CHAT,                                 // 单聊
    MSG_ORDER_GROUP_CHAT,                           // 群聊
    MSG_ORDER_CREATE_GROUP,                         // 创建群
    MSG_ORDER_USER_STATUS_CHANGE,                   // 用户在线状态改变
    MSG_ORDER_USER_INFO_UPDATE,                     // 用户信息更新
    MSG_ORDER_CHANGE_PASSWORD,                      // 修改密码
    MSG_ORDER_TEAM_INFO_UPDATE,                     // 分组信息更新
    MSG_ORDER_MOVE_FRIEND_TO_OTHER_TEAM,            // 将好友移至其它分组
    MSG_ORDER_FRIEND_REMARKS_CHANGE,                // 修改好友备注
    MSG_ORDER_ERROR                                 // 指令错误
};

typedef struct DataHead {
    std::int32_t            dh_msgOrder;                    // 消息指令
    std::int32_t            dh_seq;
    char                    dh_reserve[16];                 // 预留空间，
} DataHead;

}   // IMServer

}   /// namespace YTalk


#endif