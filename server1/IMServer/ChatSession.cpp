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
#include "../base/BinaryStream.h"

#include "rapidjson/document.h"      // 第三方库
#include "rapidjson/prettywriter.h"  // 第三方库

#define MAX_PACKAGE_SIZE    10 * 1024 * 1024

const std::string DEFAULT_TEAM = "friends";

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

        LOG_INFO("TEST %d, %d", head.ph_compress_size, head.ph_src_size);

        if(head.ph_compress_size < 0 || head.ph_compress_size > MAX_PACKAGE_SIZE || head.ph_src_size < 0 || head.ph_src_size > MAX_PACKAGE_SIZE) {
            //包头错误， 服务端主动关闭连接
            conn->forceClose();
            return;
        }
        if(buffer->readableBytes() < static_cast<size_t>(head.ph_src_size) + sizeof(PackageHead)) {
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
        LOG_INFO("The lasr package time: %d", m_lastPakcageTime);
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
    BinaryStreamReader reader(inBuf.data(), inBuf.size());
    DataHead dh;
    //bzero(&dh, sizeof(DataHead));
    if(!reader.ReadInt32(dh.dh_msgOrder)) {
        LOG_ERROR("Fail to read MSG_ORDER from client:%s", conn->peerAddress().toIpPort().c_str());
        return 1;
    }
    if(!reader.ReadInt32(dh.dh_seq)) {
        LOG_ERROR("Fail to read SEQ from client:%s", conn->peerAddress().toIpPort().c_str());
        return 1;
    }
    
    if(dh.dh_msgOrder <= MSG_ORDER_UNKNOW || dh.dh_msgOrder >= MSG_ORDER_ERROR) {
        // 指令错误
        LOG_INFO("Msg order error from client:%s", conn->peerAddress().toIpPort().c_str());
        return 2;
    }
    m_seq = dh.dh_seq;
    std::string data;
    size_t dataLen;
    if(!reader.ReadString(&data, 0, dataLen)) {
        LOG_ERROR("Fail to read data from client:%s", conn->peerAddress().toIpPort().c_str());
        return 2;
    }

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
                        break;
                    case MSG_ORDER_RESPONSE_FRIEND_APPLY:
                        // 回复好友申请
                        toResponseFriendApply(conn, data);
                        break;
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
                    case MSG_ORDER_GET_FRIENDS_LIST:
                        // 获取好友列表
                        toGetFriendsList(conn, data);
                        break;
                    case MSG_ORDER_GET_GROUP_MEMBER:
                        // 获取指定群成员
                        toGetGroupMember(conn, data);
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
    user.u_id = 0;
    //bzero(&user, sizeof(User));
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
            response = "{\"code\": 0, \"msg\": \"ok\"}";
        }
    }
    std::string rspMsg;
    // 插入DataHead,构造完整的data
    BinaryStreamWriter writer(&rspMsg);
    writer.WriteInt32(MSG_ORDER_REGISTER);
    writer.WriteInt32(m_seq);
    writer.WriteCString(response.data(), response.size());
    writer.Flush();
    
    //rspMsg.append(response);
    send(rspMsg);
    int p = m_seq;
    LOG_INFO("Response 2 client:%s, MsgOrder: register, data:%s, size=%d, seq=%d", conn->peerAddress().toIpPort().c_str(), rspMsg.c_str(), rspMsg.size(), p);
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
    //bzero(&user, sizeof(User));
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
                //bzero(&dh, sizeof(DataHead));
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
                            "\"u_gender\": " + user.u_gender + ", "
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
    //bzero(&dataH, sizeof(DataHead));
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

void ChatSession::toAddGroup(const std::shared_ptr<TcpConnection> &conn, std::int32_t g_id) {
/**
 * @brief 在toAddFriend里被调用， 加群不需要群主同意
 *       一个群相当于一个特殊的好友
 */
    // 建立好友关系，同步到数据库
    if(Singleton<UserManager>::getInstance().buildFriendRelationship(m_onlineUserInfo.u_id, g_id)) {
        // 建立好友关系失败
        LOG_ERROR("Error to build relationship friends(u_id:%d, g_id:%d)", m_onlineUserInfo.u_id, g_id);
        return;
    }
    
    User gUser;
    if(Singleton<UserManager>::getInstance().getUserByUserId(g_id, gUser)) {
        LOG_ERROR("error to get group_user");
        return;
    }
    // 给群友推送新成员消息
    std::vector<std::int32_t> groupMemberIds;
    Singleton<UserManager>::getInstance().getFriendIdByUserId(g_id, groupMemberIds);
    for(const auto &gm_id : groupMemberIds) {
        std::list<std::shared_ptr<ChatSession>> sessions;
        Singleton<ChatService>::getInstance().getSessionsByUserId(gm_id, sessions);
        
        for(const auto &session : sessions) {
            if(session) {
                session->sendWhenFriendStatusChange(g_id, 3);
            }
        }
    }
    
}

void ChatSession::toDelFriend(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 删除好友、退群
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

    // 解除好友关系，同步得到数据库和内存
    if(Singleton<UserManager>::getInstance().releaseFriendRelationship(m_onlineUserInfo.u_id, f_id)) {
        LOG_ERROR("error to delete friend:%d", f_id);
        return;
    }

    User fUser;
    if(Singleton<UserManager>::getInstance().getUserByUserId(f_id, fUser)) {
        LOG_ERROR("error to get uer:%d", f_id);
        return;
    }

    // 给主动删除的一方发送通知
    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    dh.dh_msgOrder = MSG_ORDER_DEL_FRIEND;
    dh.dh_seq = m_seq;
    std::string rspMsg(reinterpret_cast<char*>(&dh), sizeof(dh));
    std::string msg = "{"
                            "\"u_id\": " + std::to_string(f_id) + ", "
                            "\"type\: 5, "
                            "\"u_name\": \"" + fUser.u_name + "\""
                        "}";
    rspMsg += msg;
    send(rspMsg);

    // 给被删除的一方发送通知
    if(f_id < 0x0fffffff) {
        // 删好友
        std::list<std::shared_ptr<ChatSession>> fSessions;
        Singleton<ChatService>::getInstance().getSessionsByUserId(f_id, fSessions);

        msg = "{"
                    "\"u_id\": " + std::to_string(m_onlineUserInfo.u_id) + ", "
                    "\"type\: 5, "
                    "\"u_name\": \"" + m_onlineUserInfo.u_name + "\""
                "}";
        std::string rspMsg1(reinterpret_cast<char*>(&dh), sizeof(dh));
        rspMsg1 += msg;

        for(auto &fSession : fSessions) {
            // 只用给在线用户推送这个消息，不用管离线的
            if(fSession && fSession->vaild()) {
                fSession->send(rspMsg1);

                LOG_INFO("send to user:%d, MsgOrder=DELFRIEND", f_id);
            }
        }
    }
    else {
        // 退群, 只给在线的群成员推送消息
        std::vector<std::int32_t> groupMemberIds;
        Singleton<UserManager>::getInstance().getFriendIdByUserId(f_id, groupMemberIds);
        for(const auto &gm_id : groupMemberIds) {
            std::list<std::shared_ptr<ChatSession>> gm_sessions;
            Singleton<ChatService>::getInstance().getSessionsByUserId(gm_id, gm_sessions);
            for(auto &gm_session : gm_sessions) {
                if(gm_session && gm_session->vaild()) {
                    gm_session->sendWhenFriendStatusChange(f_id, 3);
                }
            }
        }
    }
}

void ChatSession::toResponseFriendApply(const std::shared_ptr<TcpConnection> &conn, const std::string &data) {
/**
 * @brief 回复加好友的申请, accept=1表示同意，其它不同意
 *      {"u_id": 123, "accept": 1}
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 登录数据错误，不处理
        LOG_ERROR("Error rsp_friend_apply data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据，
    if(!d.HasMember("u_id") || !d["u_id"].IsInt()) {
        LOG_ERROR("Error rsp_friend_apply data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("accept") || !d["accept"].IsInt()) {
        LOG_ERROR("Error rsp_friend_apply data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::int32_t f_id = d["u_id"].GetInt();
    std::int32_t accept = d["accept"].GetInt();

    if(accept == 1) {
        // 同意好友申请，与f_id建立好友关系
        if(Singleton<UserManager>::getInstance().buildFriendRelationship(m_onlineUserInfo.u_id, f_id)) {
            LOG_ERROR("error to build relation, user:%d", m_onlineUserInfo.u_id);
            return;
        }
    }

    // 告诉自己好友添加成功
    User fUser;
    if(Singleton<UserManager>::getInstance().getUserByUserId(f_id, fUser)) {
        // 获取好友信息失败，无法通知，不要紧
        return;
    }
    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    dh.dh_msgOrder = MSG_ORDER_RESPONSE_FRIEND_APPLY;
    dh.dh_seq = m_seq;
    std::string rspMsg(reinterpret_cast<char*>(&dh), sizeof(dh));
    std::string selfData = "{"
                                "\"u_id\": " + std::to_string(f_id) + ", "
                                "\"u_name\": \"" + fUser.u_name + "\", "
                                "\"accept\": " + std::to_string(accept) + " "
                            "}";
    rspMsg += selfData;
    send(rspMsg);

    LOG_INFO("response to user:%d, MsgOrder:RESPONSE_FRIEND_APPLY", m_onlineUserInfo.u_id);

    // 通知对方好友添加成功
    std::string rspMsg1(reinterpret_cast<char*>(&dh), sizeof(dh));
    std::string fData = "{"
                                "\"u_id\": " + std::to_string(m_onlineUserInfo.u_id) + ", "
                                "\"u_name\": \"" + m_onlineUserInfo.u_name + "\", "
                                "\"accept\": " + std::to_string(accept) + " "
                            "}";
    rspMsg1 += fData;
    std::list<std::shared_ptr<ChatSession>> fSessions;
    Singleton<ChatService>::getInstance().getSessionsByUserId(f_id, fSessions);
    if(fSessions.empty()) {
        // 用户离线，缓存通知
        Singleton<CacheManager>::getInstance().addCacheNotify(f_id, rspMsg1);
        LOG_INFO("user:%d offline, cache notify:%s", f_id, rspMsg1.c_str());
        return;
    }
    else {
        // 对方在线
        for(auto &fSession : fSessions) {
            fSession->send(rspMsg1);
        }
        LOG_INFO("send notify:%s to user:%d", rspMsg1.c_str(), f_id);
    }
}

void ChatSession::toChat(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 单聊, 消息格式
 *      {
 *          "acceptor": 接收者，
 *          "sender": 发送者
 *          "msgType": 0,     消息类型，0为未知类型，1文本，2窗口抖动，3文件
 *          "time": 1234123,
 *          "clientType": 0   客户端类型，0未知，2安卓端
 *          "content": [
 *                         {"msgText": text1},
 *                         {"msgText": text2},
 *                         {.....}
 *                     ]
 *       }
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 数据错误
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据
    if(!d.HasMember("acceptor") || !d["acceptor"].IsInt() || d["acceptor"].GetInt() >= 0x0fffffff) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("time") || !d["time"].IsInt()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("sender") || !d["sender"].IsInt()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::int32_t a_id = d["acceptor"].GetInt();
    std::int32_t s_id = d["sender"].GetInt();

    // 因为网络的差异，消息的确切时间以服务端为准
    unsigned int now = static_cast<unsigned int>(time(nullptr));
    d["time"].SetUint(now);

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    d.Accept(writer);
    
    std::string newData(sb.GetString(), sb.GetSize());
    
    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    dh.dh_msgOrder = MSG_ORDER_CHAT;
    dh.dh_seq = m_seq;
    std::string rsp(reinterpret_cast<char*>(&dh), sizeof(dh));

    rsp += newData;

    // 先将消息保存到数据库
    if(Singleton<UserManager>::getInstance().saveChatMsg2DB(s_id, a_id, rsp)) {
        LOG_ERROR("fail to save msg:sender=%d, acceptor=%d, data=%s", s_id, a_id, rsp.c_str());
    }

    std::list<std::shared_ptr<ChatSession>> a_sessions;
    Singleton<ChatService>::getInstance().getSessionsByUserId(a_id, a_sessions);
    if(a_sessions.empty()) {
        // 对方离线，缓存消息
        Singleton<CacheManager>::getInstance().addCacheMsg(a_id, rsp);
        LOG_INFO("cache chat msg: sender=%d, acceptor=%d, data=%s", s_id, a_id, rsp.c_str());
    }
    else {
        for(auto &a_session : a_sessions) {
            if(a_session && a_session->vaild()) {
                a_session->send(rsp);
            }
        }
    }
} 

void ChatSession::toGroupChat(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 数据错误
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据
    if(!d.HasMember("acceptor") || !d["acceptor"].IsInt() || d["acceptor"].GetInt() < 0x0fffffff) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("time") || !d["time"].IsInt()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("sender") || !d["sender"].IsInt()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::int32_t a_id = d["acceptor"].GetInt();    // 为群号
    std::int32_t s_id = d["sender"].GetInt();

    // 因为网络的差异，消息的确切时间以服务端为准
    unsigned int now = static_cast<unsigned int>(time(nullptr));
    d["time"].SetUint(now);

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    d.Accept(writer);
    
    std::string newData(sb.GetString(), sb.GetSize());

    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    dh.dh_msgOrder = MSG_ORDER_GROUP_CHAT;
    dh.dh_seq = m_seq;
    std::string rsp(reinterpret_cast<char*>(&dh), sizeof(dh));

    rsp += newData;

    // 先将消息保存到数据库
    if(Singleton<UserManager>::getInstance().saveChatMsg2DB(s_id, a_id, rsp)) {
        LOG_ERROR("fail to save msg:sender=%d, acceptor=%d, data=%s", s_id, a_id, rsp.c_str());
    }

    std::vector<std::int32_t> groupMemberIds;
    Singleton<UserManager>::getInstance().getFriendIdByUserId(a_id, groupMemberIds);
    for(auto &gm_id : groupMemberIds) {
        if(gm_id == m_onlineUserInfo.u_id) {
            // 除了自己
            continue;
        }

        std::list<std::shared_ptr<ChatSession>> gm_sessions;
        Singleton<ChatService>::getInstance().getSessionsByUserId(gm_id, gm_sessions);
        if(gm_sessions.empty()) {
            // 不在线，缓存消息
            Singleton<CacheManager>::getInstance().addCacheMsg(gm_id, rsp);
            continue;
        }
        for(auto &gm_session : gm_sessions) {
            if(gm_session)
                gm_session->send(rsp);
        }
    }
}

void ChatSession::toCreateGroup(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 建群
 *      {"groupname":"qqq"}
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 数据错误
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据
    if(!d.HasMember("g_name") || !d["g_name"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    
    std::string applyData;

    std::string g_name = d["g_name"].GetString();
    std::int32_t g_id;
    if(Singleton<UserManager>::getInstance().createGroup(g_name, m_onlineUserInfo.u_id, g_id)) {
        LOG_ERROR("error to create group");
        applyData = "{"
                        "\"code\": 106, "
                        "\"msg\": \"fail to create group\""
                    "}";
    }
    else {
        applyData = "{"
                        "\"code\": 0, "
                        "\"msg\": \"ok\", "
                        "\"g_id\": " + std::to_string(g_id) + ","
                        "\"g_name\": \"" + g_name + "\""
                    "}";
    }

    // 建群成功后，自动加群，建立关系
    if(Singleton<UserManager>::getInstance().buildFriendRelationship(m_onlineUserInfo.u_id, g_id)) {
        LOG_ERROR("error to join group:g_id=%d, u_id=%d", g_id, m_onlineUserInfo.u_id);
        return;
    }

    // 回复用户
    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    dh.dh_msgOrder = MSG_ORDER_CREATE_GROUP;
    dh.dh_seq = m_seq;
    std::string rsp(reinterpret_cast<char*>(&dh), sizeof(dh));

    applyData = rsp + applyData;

    send(applyData);
}

void ChatSession::toChangeUserStatus(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 修改自己的在线状态
 *       {"onlinestatus": 1}
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 数据错误
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据
    if(!d.HasMember("onlinestatus") || !d["onlinestatus"].IsInt()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::int32_t status = d["onlinestatus"].GetInt();

    if(m_onlineUserInfo.u_status == status) {
        //TODO：这一步，可以在客户端判断
        return;
    }

    m_onlineUserInfo.u_status = status;

    // 通知好友我的状态发生改变
    std::vector<std::int32_t> f_ids;
    Singleton<UserManager>::getInstance().getFriendIdByUserId(m_onlineUserInfo.u_id, f_ids);
    for(auto &f_id : f_ids) {
        std::list<std::shared_ptr<ChatSession>> f_sessions;
        Singleton<ChatService>::getInstance().getSessionsByUserId(f_id, f_sessions);
        for(auto &f_session : f_sessions) {
            if(f_session)
                f_session->sendWhenFriendStatusChange(m_onlineUserInfo.u_id, 1, status);
        }
    }
}

void ChatSession::toChangeUserInfo(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 修改用户信息, 
 *      {"u_nickname": , "u_facetype": , "u_face": , "u_gender": , "u_birthday": , 
 *       "u_signature": , ""}
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 数据错误
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据
    if(!d.HasMember("u_nickname") || !d["u_nickname"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("u_face") || !d["u_face"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("u_signature") || !d["u_signature"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("u_facetype") || !d["u_facetype"].IsInt()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("u_birthday") || !d["u_birthday"].IsInt()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("u_gender") || !d["u_gender"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    User u;
    u.u_nickname = d["u_nickname"].GetString();
    u.u_face = d["u_face"].GetString();
    u.u_signature = d["u_signature"].GetString();
    u.u_faceType = d["u_faceType"].GetInt();
    u.u_birthday = d["u_birthday"].GetInt();
    u.u_gender = d["u_gender"].GetString();

    std::string response;

    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    dh.dh_msgOrder = MSG_ORDER_USER_INFO_UPDATE;
    std::string msg(reinterpret_cast<char*>(&dh), sizeof(dh));

    // 更新用户的信息,同步到数据库
    if(Singleton<UserManager>::getInstance().updateUser(m_onlineUserInfo.u_id, u)) {
        response = "{"
                        "\"code\": 104, "
                        "\"msg\": \"fail to update user\""
                    "}";

        response = msg + response;
        send(response);
        return;
    }
    else {
        response = "{"
                        "\"code\": 0, "
                        "\"msg\": \"ok\""
                    "}";
        
        response = msg + response;
        send(response);

        // 给好友发生变更消息
        std::vector<std::int32_t> f_ids;
        Singleton<UserManager>::getInstance().getFriendIdByUserId(m_onlineUserInfo.u_id, f_ids);
        for(auto &f_id : f_ids) {
            // 发给在线的用户就可以了
            std::list<std::shared_ptr<ChatSession>> f_sessons;
            for(auto &f_session : f_sessons) {
                if(f_session) {
                    f_session->sendWhenFriendStatusChange(m_onlineUserInfo.u_id, 3);
                }
            }
        }
    }
}

void ChatSession::toChangePassword(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 修改密码
 *      {"oldpassword": , "newpassword": }
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 数据错误
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据
    if(!d.HasMember("oldpassword") || !d["oldpassword"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("newpassword") || !d["newpassword"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::string oldpassword = d["oldpassword"].GetString();
    std::string newpassword = d["newpassword"].GetString();

    User user;
    if(Singleton<UserManager>::getInstance().getUserByUserId(m_onlineUserInfo.u_id, user)) {
        LOG_ERROR("GER USER ERROR");
        return;
    }

    std::string response;

    if(user.u_password != oldpassword) {
        response = "{"
                        "\"code\": 103, "
                        "\"msg\": \"old password is wrong\""
                    "}"; 
    }
    else {
        if(Singleton<UserManager>::getInstance().changePassword(m_onlineUserInfo.u_id, newpassword)) {
            response = "{"
                            "\"code\": 105, "
                            "\"msg\": \"change password error\""
                        "}";
        }
        else {
            response = "{"
                            "\"code\": 0, "
                            "\"msg\": \"ok\""
                        "}";
        }
    }

    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    dh.dh_msgOrder = MSG_ORDER_CHANGE_PASSWORD;
    std::string msg(reinterpret_cast<char*>(&dh), sizeof(dh));

    response = msg + response;

    send(response);
    LOG_INFO("change user=%d successful, response to client", m_onlineUserInfo.u_id, conn->peerAddress().toIpPort().c_str());
}

void ChatSession::toChangeTeamInfo(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 修改好友分组的信息
 *      {"operation": 1, "oldteam": "ww", "newteam": "qq"}
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 数据错误
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据
    if(!d.HasMember("oldteam") || !d["oldteam"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("newteam") || !d["newteam"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("operation") || !d["operation"].IsInt()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::int32_t operation = d["operation"].GetInt();
    std::string oldteam = d["oldteam"].GetString();
    std::string newteam = d["newteam"].GetString();

    // 获取用户的所有分组信息
    std::string teams;
    if(Singleton<UserManager>::getInstance().getTeamsByUserId(m_onlineUserInfo.u_id, teams)) {
        return;
    }
    if(teams.empty()) {
        teams = "[{\"u_teaminfo\": \"";
        teams += DEFAULT_TEAM;
        teams += "\", \"members\":[]}]";
    }

    // teams是一个json对象
    rapidjson::Document doc;
    if(doc.Parse(teams.c_str()).HasParseError() || !doc.IsArray()) {
        LOG_ERROR("Get teaminfo from user error, please check it");
        return;
    }

    if(operation == 0) {
        // 增加分组
        std::int32_t teamNumbers = doc.Size();
        for(int i = 0; i < teamNumbers; ++i) {
            // 查看分组是否已经存在
            if(!doc[i]["u_teaminfo"].IsNull() && doc[i]["u_teaminfo"].GetString() == newteam) {
                LOG_INFO("team is exits");
                return;
            }
        }

        //TODO
    }
    else if(operation == 1) {
        // 删除分组
    }
    else if(operation == 2) {
        // 修改分组
    }
    else {
        // 数据错误，不处理
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
}

void ChatSession::toMoveFriend2OtherTeam(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 将好友移至其它分组
 *      {"f_id": 123, "oldteam": "dd", "newteam":"qww"}
 */
    rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 数据错误
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据
    if(!d.HasMember("oldteam") || !d["oldteam"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("newteam") || !d["newteam"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("f_id") || !d["f_id"].IsInt()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::int32_t f_id = d["f_id"].GetInt();
    std::string oldteam = d["oldteam"].GetString();
    std::string newteam = d["newteam"].GetString();

    if(oldteam == newteam) {
        return;
    }

    // 判断f_id是否是你的好友
    if(Singleton<UserManager>::getInstance().isFriend(m_onlineUserInfo.u_id, f_id)) {
        LOG_ERROR("f_id=%d isn't user=%d friend", f_id, m_onlineUserInfo.u_id);
        return;
    }

    User user;
    if(Singleton<UserManager>::getInstance().getUserByUserId(m_onlineUserInfo.u_id, user)) {
        LOG_ERROR("user=%d isn't in memory", m_onlineUserInfo.u_id);
        return;
    }

    std::string teams = user.u_teaminfo;
    if(teams.empty()) {
        teams = "[{\"u_teaminfo\": \"";
        teams += DEFAULT_TEAM;
        teams += "\", \"members\":[]}]";
    }

    // teams是一个json对象
    rapidjson::Document doc;
    if(doc.Parse(teams.c_str()).HasParseError() || !doc.IsArray()) {
        LOG_ERROR("Get teaminfo from user error, please check it");
        return;
    }

    bool op1 = false;
    bool op2 = false;
    std::int32_t teamNumbers = doc.Size();
    for(int i = 0; i < teamNumbers; ++i) {
        if(doc[i]["u_teaminfo"].IsString()) {
            std::string t = doc[i]["u_teaminfo"].GetString();
            if(t == newteam) {
                op1 = true;
                continue;
            }
            if(t == oldteam) {
                op2 = true;
            }
            if(op1 && op2) {
                break;
            }
        }
    }

    if(!op1 || !op2) {
        return;
    }
    if(Singleton<UserManager>::getInstance().moveFriend2OtherTeam(m_onlineUserInfo.u_id, f_id, newteam)) {
        LOG_ERROR("move f_id=%d to newteam=%s error in user=%d", f_id, newteam.c_str(), m_onlineUserInfo.u_id);
        return;
    }

    //TODO
}

void ChatSession::toChangeFriendRemarks(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data) {
/**
 * @brief 修改好友备注
 *      {"f_id": 12, "newremarks":"ww"}
 */
     rapidjson::Document d;
    if(d.Parse(data.c_str()).HasParseError()) {
        // 数据错误
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    // 检查数据
    if(!d.HasMember("newremarks") || !d["newremarks"].IsString()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }
    if(!d.HasMember("f_id") || !d["f_id"].IsInt()) {
        LOG_ERROR("error data from client:%s", conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::int32_t f_id = d["f_id"].GetInt();
    std::string newremarks = d["newremarks"].GetString();

    if (Singleton<UserManager>::getInstance().changeFriendRemarks(m_onlineUserInfo.u_id, f_id, newremarks)){
        //TODO: 失败应答客户端
        LOG_ERROR("Update markname failed, userid: %d, friendid: %d, client: %s", m_onlineUserInfo.u_id, f_id, conn->peerAddress().toIpPort().c_str());
        return;
    }

    std::string response;
    response = "{"
                    "\"code\": 0, " 
                    "\"msg\": \"ok\" "
                "}";
   
    DataHead dh;
    bzero(&dh, sizeof(DataHead));
    dh.dh_msgOrder = MSG_ORDER_FRIEND_REMARKS_CHANGE;
    std::string msg(reinterpret_cast<char*>(&dh), sizeof(dh));

    response = msg + response;

    send(response);

    LOG_INFO("Response to client, userid: %d, MSGoRDER=FRIEND_REMARKS_CHANGE, data: %s", m_onlineUserInfo.u_id, response.c_str());
}

void ChatSession::toGetFriendsList(const std::shared_ptr<TcpConnection> &conn, const std::string &data) {
/**
 * @brief 获取好友列表，data为空
 *   
 */
    // 首先从数据库中获取好友分组信息
    std::string teamsJson;
    if( Singleton<UserManager>::getInstance().getTeamsByUserId(m_onlineUserInfo.u_id, teamsJson)) {
        LOG_ERROR("Fail to get Teams from user=%d", m_onlineUserInfo.u_id);
        return;
    }
    if(teamsJson.empty()) {
        teamsJson = "["
                        "{"
                            "\"teamname\": \"myFriends\", "
                            "\"members\": []"
                        "}"
                    "]";
    }

    rapidjson::Document document;
    if(document.Parse(teamsJson.data()).HasParseError() || !document.IsArray()) {
        LOG_ERROR("Internal error, fail to parse json:%s", teamsJson.c_str());
        return;
    }
    rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

    User user;  
    User f_user;
    Singleton<UserManager>::getInstance().getUserByUserId(m_onlineUserInfo.u_id, user);
    for(auto &f : user.u_friends) {
        for(auto &team : document.GetArray()) {
            // 每个team是一个object
            if(!team.HasMember("members")) {
                rapidjson::Value value(rapidjson::kArrayType);
                team.AddMember("members", value, allocator);
            }
            if(team["teamname"].GetString() != f.f_team) {
                continue;
            }

            // 获取好友用户信息
            if(Singleton<UserManager>::getInstance().getUserByUserId(f.f_id, f_user)) {
                continue;
            }

            // 填充好友的详细信息
            rapidjson::Value value(rapidjson::kObjectType);
            value.AddMember("u_id", f_user.u_id, allocator);
            rapidjson::Value name(f_user.u_name.c_str(), f_user.u_name.size());
            value.AddMember("u_name", name, allocator);
            rapidjson::Value nickname(f_user.u_nickname.c_str(), f_user.u_nickname.size());
            value.AddMember("u_nickname", nickname, allocator);
            value.AddMember("u_faceType", f_user.u_faceType, allocator);
            rapidjson::Value face(f_user.u_face.c_str(), f_user.u_face.size());
            value.AddMember("u_face", face, allocator);
            rapidjson::Value gender(f_user.u_gender.c_str(), f_user.u_gender.size());
            value.AddMember("u_gender", gender, allocator);
            rapidjson::Value signature(f_user.u_signature.c_str(), f_user.u_signature.size());
            value.AddMember("u_signature", signature, allocator);
            rapidjson::Value remarks(f.f_remarks.c_str(), f.f_remarks.size());
            value.AddMember("remarks", remarks, allocator);
            std::int32_t cType = Singleton<ChatService>::getInstance().getClientTypeByUserId(f_user.u_id);
            value.AddMember("clienttype", cType, allocator);
            std::int32_t status = Singleton<ChatService>::getInstance().getStatusByUserId(f_user.u_id);
            value.AddMember("status", status, allocator);

            team["members"].PushBack(value, allocator);
            break;
        }
    }

    // 将数据返回给客户端
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> pWriter(buffer);
    document.Accept(pWriter);
    if(buffer.GetString()) {
        std::string temp(buffer.GetString(), buffer.GetSize());
        teamsJson = temp;
    }
    std::ostringstream resJson;
    resJson << "{\"code\": 0, \"msg\": \"ok\", \"userinfo\": " << teamsJson << "}";
    DataHead dh;
    dh.dh_msgOrder = MSG_ORDER_GET_FRIENDS_LIST;
    dh.dh_seq = m_seq;
    std::string response(reinterpret_cast<char*>(&dh), sizeof(DataHead));
    response += resJson.str();
    send(response);

    LOG_INFO("Response to client:%s, MsgOrder:GET_FRIENDS_LIST, data:%s", resJson.str().c_str());
}

void ChatSession::toGetGroupMember(const std::shared_ptr<TcpConnection> &conn, const std::string &data) {
    //TODO
}

};   /// namespace IMServer

}   /// namesapce YTalk



