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
#include "UserManager.h"
#include "CacheManager.h"

#include "rapidjson/document.h"      // 第三方库
#include "rapidjson/prettywriter.h"  // 第三方库

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
/**
 * 注册数据，账号规定为11位(一般为手机号码)、昵称、密码
 *   {"u_name": "18176447770", "u_nickname": "nwuking", "u_password": "520"}
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 非法的数据，不处理
        LOG_ERROR("Fail to parse data, illegal json from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("u_name") || !d.HasMember("u_nickname"), !d.HasMember("u_password")) {
        // 注册的数据不能缺少期中一个
        LOG_ERROR("Illegal json from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::string response;

    User newUser;
    newUser.u_name = d["u_name"].GetString();
    newUser.u_nickname = d["u_nickname"].GetString();
    newUser.u_password = d["u_password"].GetString();

    // 查找u_name是否有用户已经注册了
    User user;
    bzero(&user, sizeof(User));
    Singleton<UserManager>::getInstance().getUserByUserName(newUser.u_name, user);
    if(user.u_id != 0) {
        // 该账号已经注册了
        response = "{"
                        "\"code\": 101, "
                        "\"msg\": \"registered already\""
                    "}";
    }
    else {
        if(Singleton<UserManager>::getInstance().registerForUser(newUser)) {
            // 注册失败
            response = "{"
                            "\"code\": 100, "
                            "\"msg\": \"register fail\""
                        "}";
        }
        else {
            // 注册成功
            response = "{"
                            "\"code\": 0, "
                            "\"msg\": \"ok\""
                        "}";
        }
    }

    std::string rspMsg;
    // 插入DataHead,构造完整的data
    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    dh.dh_msgOrder = MSG_ORDER_REGISTER;
    dh.dh_seq = m_seq;
    rspMsg.append(reinterpret_cast<char*>(&dh), sizeof(DataHead));
    rspMsg += response;

    send(rspMsg);
    LOG_INFO("Response 2 client:%s, MsgOrder: register, data:%s", conn->peerAddress().toIpPort().c_str(), response.c_str());
}

void ChatSession::toLogin(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 登录数据格式,账号、密码、客户端类型、用户在线状态
 *      {"u_name": "18176447770", "u_password": "520", "clienttype": 2, "status":1}
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 登录数据错误，不处理
        LOG_ERROR("Error login data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据，
    if(!d.HasMember("u_name") || !d["u_name"].IsString()) {
        LOG_ERROR("Error login data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("u_password") || !d["u_password"].IsString()) {
        LOG_ERROR("Error login data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("clienttype") || !d["clienttype"].IsInt()) {
        LOG_ERROR("Error login data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("status") || !d["status"].IsInt()) {
        LOG_ERROR("Error login data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::string response;

    // 先判断该账号有没有注册
    User user;
    bzero(&user, sizeof(User));
    user.u_name = d["u_name"].GetString();
    Singleton<UserManager>::getInstance().getUserByUserName(user.u_name, user);
    if(user.u_id == 0) {
        // 用户没有注册
        response = "{"
                        "\code\": 102, "
                        "\msg\": \"not register\""
                    "}";
    }
    else {
        // 用户已经注册，验证密码
        if(user.u_password != d["u_password"].GetString()) {
            // 密码不正确
            response = "{"
                            "\"code\": 103, "
                            "\"msg\": \"error passeord\""
                        "}";
        }
        else { // 密码正确
            // 检查是否有其它地方在线，支持不同的客户端同时在线，相同客户端视为冲突
            std::shared_ptr<ChatSession> clashSession = Singleton<ChatService>::getInstance().getSessionByUserIdAndClientType(user.u_id, d["clienttype"].GetInt());
            if(clashSession) {
                // 存在冲突的session，踢人下线
                DataHead dh;
                bzero(&dh, sizeof(DataHead));
                dh.dh_msgOrder = MSG_ORDER_KICK;
                dh.dh_seq = clashSession->getSeq();
                std::string temp;
                temp.append(reinterpret_cast<char*>(&dh), sizeof(DataHead));
                clashSession->send(temp);
                clashSession->setInvaild();
            }

            // 此时，该session变得可用
            m_onlineUserInfo.u_id = user.u_id;
            m_onlineUserInfo.u_name = user.u_name;
            m_onlineUserInfo.u_nickname = user.u_nickname;
            m_onlineUserInfo.u_clientType = d["clienttype"].GetInt();
            m_onlineUserInfo.u_status = d["status"].GetInt();

            // 返回个人基本信息给用户
            response = "{"
                            "\"code\": 0, "
                            "\"msg\": \"ok\", "
                            "\"u_id\": " + std::to_string(user.u_id) + ", "
                            "\"u_name\": \"" + user.u_name + "\", "
                            "\"u_nickname\": \"" + user.u_nickname + "\", "
                            "\"u_gender\": \"" + user.u_gender + "\", "
                            "\"u_birthday\": " + std::to_string(user.u_birthday) + ", "
                            "\"u_signature\": \"" + user.u_signature + "\", "
                            "\"u_faceType\": " + std::to_string(user.u_faceType) + ", "
                            "\"u_face\": \"" + user.u_face + "\""
                        "}";

            // 设置登录成功标志
            m_isLogin = true;
        }
    }

    // 回复用户的登录情况
    std::string rspData;
    DataHead dataH;
    bzero(&dataH, sizeof(DataHead));
    dataH.dh_msgOrder = MSG_ORDER_LOGIN;
    dataH.dh_seq = m_seq;
    rspData.append(reinterpret_cast<char*>(&dataH), sizeof(dataH));
    rspData += response;
    send(rspData);
    
    LOG_INFO("Response login to client:%s", conn->peerAddress().toIpPort().c_str());

    if(m_isLogin) { // 登录成功才会走这里
        //TODO： 推送离线聊天消息
        std::list<CacheMsg> lcm;
        Singleton<CacheManager>::getInstance().getCacheMsg(m_onlineUserInfo.u_id, lcm);
        for(const auto &cm : lcm) {
            send(cm.m_data);
        }

        //TODO： 推送离线通知消息：比喻说别人把我删了的通知
        std::list<CacheNotify> lcn;
        Singleton<CacheManager>::getInstance().getCacheNotify(m_onlineUserInfo.u_id, lcn);
        for(const auto &cn : lcn) {
            send(cn.n_data);
        }

        //TODO： 通知好友我已经上线
        std::vector<std::int32_t> friendIds;
        Singleton<UserManager>::getInstance().getFriendIdByUserId(m_onlineUserInfo.u_id, friendIds);
        for(const auto &f_id : friendIds) {
            // f_id可能存在多个session，因为支持多端登录
            std::list<std::shared_ptr<ChatSession>> f_sessions;
            Singleton<ChatService>::getInstance().getSessionsByUserId(f_id, f_sessions);
            for(auto &f_session : f_sessions) {
                if(f_session->vaild()) {
                    // 有效的session才处理
                    f_session->sendWhenFriendStatusChange(m_onlineUserInfo.u_id, 1, m_onlineUserInfo.u_status);
                    
                    LOG_INFO("Send user:%s's status{type:1, status:%d} to user:%d", m_onlineUserInfo.u_id, m_onlineUserInfo.u_status, f_session->getUserId());
                }
            }
        }
    }
}

void ChatSession::toHeartBeat(const std::shared_ptr<netlib::TcpConnection> &conn) {
    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    dh.dh_msgOrder = MSG_ORDER_HEARTBEAT;
    dh.dh_seq = m_seq;

    std::string data(reinterpret_cast<char*>(&dh), sizeof(DataHead));
}

void ChatSession::toFindFriend(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 用户输入***，会查找到u_name=***或者u_nickname=***的用户
 *     {"u_name": "18176447770", "u_nickname": "18176447770"}
 *     {"u_name": "nwuking", "u_nickname": "nwuking"}
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 登录数据错误，不处理
        LOG_ERROR("Error find_friend data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据，
    if(!d.HasMember("u_name") || !d["u_name"].IsString()) {
        LOG_ERROR("Error find_friend data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("u_nickname") || !d["u_nickname"].IsString()) {
        LOG_ERROR("Error find_friend data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    std::string u_name = d["u_name"].GetString();
    std::string u_nickname = d["u_nickname"].GetString();

    // 
    std::string response = "{"
                                "\"code\": 0, "
                                "\"msg\": \"ok\", "
                                "\"userinfo\": [";

    // 根据u_nickname查找用户，会找到很多user
    std::list<User> users;
    Singleton<UserManager>::getInstance().getUsersByNickname(u_nickname, users);

    // 根据u_name查找用户，只有一个用户
    User user;
    bzero(&user, sizeof(User));
    Singleton<UserManager>::getInstance().getUserByUserName(u_name, user);
    if(user.u_id != 0) {
        users.push_back(user);
    }

    // 遍历users，构造response
    for(const auto &u : users) {
        std::string temp = "{"
                                "\"u_id\": " + std::to_string(u.u_id) + ", "
                                "\"u_name\": \"" + u.u_name + "\", "
                                "\"u_nickname\": \"" + u.u_nickname + "\""
                            "}";
        response += temp;
    }

    response += "]}";

    //std::string rspMsg;
    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    dh.dh_msgOrder = MSG_ORDER_FIND_FRIEND;
    dh.dh_seq = m_seq;
    std::string rspMsg(reinterpret_cast<char*>(&dh), sizeof(dh));
    
    rspMsg += response;

    send(rspMsg);

    LOG_INFO("Send find_friend response to user:%d, MsgOrder: FIND_FRIEND", m_onlineUserInfo.u_id);
}

void ChatSession::toAddFriend(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 发出好友申请或加群申请, 服务端根据u_id推送申请
 *      {"u_id": 123}
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 登录数据错误，不处理
        LOG_ERROR("Error add_friend data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据，
    if(!d.HasMember("u_id") || !d["u_id"].IsInt()) {
        LOG_ERROR("Error add_friend data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::int32_t f_id = d["u_id"].GetInt();

    if(f_id >= 0x0fffffff) { // 加群申请
        if(Singleton<UserManager>::getInstance().isFriend(m_onlineUserInfo.u_id, f_id)) {
            // 已经是f_id的成员了，不处理
            LOG_INFO("user:%d is member with group:%d", m_onlineUserInfo.u_id, f_id);
            return;
        }

        // 加群不用群主同意
        toAddGroup(conn, f_id);

        LOG_INFO("user:%d join group:%d", m_onlineUserInfo.u_id, f_id);
    }
    else {        // 加好友申请
        // 判断于u_id是否为好友
        if(Singleton<UserManager>::getInstance().isFriend(m_onlineUserInfo.u_id, f_id)) {
            // 已经是好友了，不处理
            LOG_INFO("user:%d is friend with user:%d", m_onlineUserInfo.u_id, f_id);
            return;
        }

        // 服务端构造完整的申请加好友数据
        std::string temp = "{"
                                "\"u_id\": " + std::to_string(m_onlineUserInfo.u_id) + ", "
                                "\"u_name\": \"" + m_onlineUserInfo.u_name + "\", "
                                "\"u_nickname\": \"" + m_onlineUserInfo.u_nickname + "\""
                            "}";
        DataHead dh;
        bzero(&dh, sizeof(DataHead));
        dh.dh_msgOrder = MSG_ORDER_ADD_FRIEND;
        dh.dh_seq = m_seq;

        std::string applyData(reinterpret_cast<char*>(&dh), sizeof dh);
        applyData += temp;

        // 判断f_id是否在线
        std::list<std::shared_ptr<ChatSession>> f_sessions;
        Singleton<ChatService>::getInstance().getSessionsByUserId(f_id, f_sessions); 
        if(f_sessions.empty()) {
            // 对方离线，缓存消息
            Singleton<CacheManager>::getInstance().addCacheMsg(f_id, applyData);
            
            LOG_INFO("Cache user:%d's apply friend to user:%d", m_onlineUserInfo.u_id, f_id);
        }
        else {
            // 对方在线，直接推送
            for(auto & f_session : f_sessions) {
                f_session->send(applyData);
            }

            LOG_INFO("user:%d send apply friend to user:%d", m_onlineUserInfo.u_id, f_id);
        }
    }
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