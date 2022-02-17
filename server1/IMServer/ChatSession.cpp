/**
 * @file ChatSession.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ChatSession.h"
#include "../base/Logger.h"
#include "../netlib/EventLoop.h"
#include "Protocal.h"
#include "../base/Singleton.h"
#include "ChatService.h"

#define MAX_PACKAGE_SIZE    10 * 1024 * 1024   

namespace YTalk
{

using namespace netlib;
using namespace base;

namespace IMServer
{

ChatSession::ChatSession(const std::shared_ptr<TcpConnection> &conn, std::int32_t sessionId) :
    Session(conn), 
    m_sessionId(sessionId), 
    m_seq(0),
    m_isLogin(false)
{
    m_onlineUserInfo.u_id = 0;
    m_lastPakcageTime = time(nullptr);
}

ChatSession::~ChatSession() {
    std::shared_ptr<TcpConnection> conn = getTcpConnectionPtr();
    if(conn) {
        LOG_INFO("Remove timer of {u_id=%d, clientType=%d, clientAddr=%s", 
                m_onlineUserInfo.u_id, m_onlineUserInfo.u_clientType, 
                conn->peerAddress().toIpPort().c_str());

        conn->getLoop()->remove(m_timerId);
    }
}

void ChatSession::onRead(const std::shared_ptr<netlib::TcpConnection> &conn, Buffer *buffer, TimeStamp receTime) {
    while(true) {
        if(buffer->readableBytes() < sizeof(PackageHead)) {
            return;
        }

        // 读取PackageHead
        PackageHead head;
        memcpy(&head, buffer->peek(), sizeof(PackageHead));
        std::string inBuf;

        if(head.ph_compress_size < 0 || head.ph_compress_size > MAX_PACKAGE_SIZE || head.ph_src_size < 0 || head.ph_src_size > MAX_PACKAGE_SIZE) {
            //包头错误， 服务端主动关闭连接
            conn->forceClose();
            return;
        }
        if(buffer->readableBytes() < static_cast<size_t>(head.ph_compress_size) + sizeof(PackageHead)) {
            // 不是一个完整的包，不做处理
            return;
        }
        buffer->retrieve(sizeof(PackageHead));

        if(head.ph_compress == '1') {
            // 压缩数据
            std::string msg;
            msg.append(buffer->peek(), head.ph_compress_size);
            buffer->retrieve(head.ph_compress_size);
            //TODO: 对数据进行解压
            inBuf = msg;
        }
        else {
            // 数据没被压缩
            inBuf.append(buffer->peek(), head.ph_src_size);
            buffer->retrieve(head.ph_src_size);
        }

        // 处理受到的数据(inBuf)
        if(handleData(conn, inBuf)) {
            // 客户端发送错误的消息，服务端强制关闭
            LOG_ERROR("Recve error mag from client:%s", conn->peerAddress().toIpPort());
            conn->forceClose();
            return;
        }

        m_lastPakcageTime = time(nullptr);
    }
}

void ChatSession::sendWhenFriendStatusChange(std::int32_t f_id, int type, int status/*=0*/) {
    std::string msg;
    if(type == 1) {
        // f_id上线
        std::int32_t clientType = Singleton<ChatService>::getInstance().getClientTypeByUserId(f_id);
        msg = "{"
                    "\"type\": 1, "
                    "\"onlinestatus\": " + std::to_string(status) + ", "
                    "\"clienttype\": " + std::to_string(clientType) + ", "
                    "\"friend\": " + std::to_string(f_id) + " "
              "}";
    }
    else if(type == 2) {
        // f_id下线
        msg = "{"
                    "\"type\": 2, "
                    "\"friend\": " + std::to_string(f_id) + " "
              "}";
    }
    else if(type == 3) {
        // f_id的个人信息修改
        msg = "{"
                    "\"type\": 3, "
                    "\"friend\": " + std::to_string(f_id) + " "
              "}";
    }

    std::string outBuf;
    DataHead dh;
    dh.dh_msgOrder = MSG_ORDER_USER_STATUS_CHANGE;
    dh.dh_seq = m_seq;
    outBuf.append(reinterpret_cast<char*>(&dh), sizeof(dh));
    outBuf += msg;

    send(outBuf);

    LOG_INFO("Send to client: u_id=%d, cmd=MSG_ORDER_USER_STATUS_CHANGE, data=%s", m_onlineUserInfo.u_id, outBuf.data());
}

int ChatSession::handleData(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &inBuf) {
    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    const char *buf = inBuf.data();
    memcpy(&dh, buf, sizeof(DataHead));
    std::int16_t check;
    memcpy(&check, dh.dh_reserve, 16);
    if(check != 0) {
        // 非法的数据，不处理
        LOG_ERROR("Illegal data from client:%s", conn->peerAddress().toIpPort().c_str());
        return 1;
    }
    if(dh.dh_msgOrder <= MSG_ORDER_UNKNOW || dh.dh_msgOrder >= MSG_ORDER_ERROR) {
        // 指令错误
        LOG_INFO("Msg order error from client:%s", conn->peerAddress().toIpPort().c_str());
        return 2;
    }
    m_seq = dh.dh_seq;
    buf += sizeof(DataHead);

    std::string data;
    data.append(buf, inBuf.size()-sizeof(DataHead));

    switch(dh.dh_msgOrder) {
        case MSG_ORDER_REGISTER:
            // 注册
            toRegister(conn, data);
            break;
        case MSG_ORDER_LOGIN:
            // 登录
            toLogin(conn, data);
            break;
        case MSG_ORDER_HEARTBEAT:
            // 心跳包
            toHeartBeat(conn);
            break;
        default: {
            // 其它的指令只有用户登录成功才可以使用
            if(m_isLogin) {
                // 用户登录成功
                switch(dh.dh_msgOrder) {
                    // 查找好友、群
                    case MSG_ORDER_FIND_FRIEND:
                        toFindFriend(conn, data);
                        break;
                    case MSG_ORDER_ADD_FRIEND:
                        // 添加好友
                        toAddFriend(conn, data);
                        break;
                    case MSG_ORDER_DEL_FRIEND:
                        // 删好友
                        toDelFriend(conn, data);
                    case MSG_ORDER_CHAT:
                        // 单聊
                        toChat(conn, data);
                        break;
                    case MSG_ORDER_GROUP_CHAT:
                        // 群聊
                        toGroupChat(conn, data);
                        break;
                    case MSG_ORDER_CREATE_GROUP:
                        // 创建群
                        toCreateGroup(conn, data);
                        break;
                    case MSG_ORDER_USER_STATUS_CHANGE:
                        // 在线状态的变更
                        toChangeUserStatus(conn, data);
                        break;
                    case MSG_ORDER_USER_INFO_UPDATE:
                        // 个人信息的修改
                        toChangeUserInfo(conn, data);
                        break;
                    case MSG_ORDER_CHANGE_PASSWORD:
                        // 修改密码
                        toChangePassword(conn, data);
                        break;
                    case MSG_ORDER_TEAM_INFO_UPDATE:
                        // 好友分组信息修改
                        toChangeTeamInfo(conn, data);
                        break;
                    case MSG_ORDER_MOVE_FRIEND_TO_OTHER_TEAM:
                        // 将好友移至其它分组
                        toMoveFriend2OtherTeam(conn, data);
                        break;
                    case MSG_ORDER_FRIEND_REMARKS_CHANGE:
                        // 修改好友备注
                        toChangeFriendRemarks(conn, data);
                        break;
                    default:
                        LOG_ERROR("Error Msg Order from client:%s", conn->peerAddress().toIpPort().c_str());
                        return 5;
                }
            }
            else {
                // 用户没有登录
                std::string msg = "{"
                                        "\"code\": 2, "
                                        "\"msg\": \"Please login first\""
                                   "}";
                send(msg);
                LOG_INFO("Client:%s is not login", conn->peerAddress().toIpPort().c_str());
            }
        }
    }
    ++m_seq;
    return 0;
}

void ChatSession::toRegister(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toLogin(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toHeartBeat(const std::shared_ptr<netlib::TcpConnection> &conn) {
    //TODO
}

void ChatSession::toFindFriend(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toAddFriend(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toDelFriend(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toChat(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
} 

void ChatSession::toGroupChat(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toCreateGroup(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toChangeUserStatus(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toChangeUserInfo(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toChangePassword(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toChangeTeamInfo(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toMoveFriend2OtherTeam(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

void ChatSession::toChangeFriendRemarks(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    //TODO
}

};   /// namespace IMServer

}   /// namesapce YTalk