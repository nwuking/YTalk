/**
 * @file ChatSession.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_CHAT_SESSION_H
#define YTALK_CHAT_SESSION_H

#include "Session.h"
#include "../netlib/TcpConnection.h"
#include "../netlib/TimerId.h"
#include "User.h"
#include "../netlib/Buffer.h"
#include "../base/TimeStamp.h"

#include <memory>
#include <cstdint>
#include <list>

namespace YTalk
{

namespace IMServer
{

class ChatSession : public Session
{
public:
    ChatSession(const std::shared_ptr<netlib::TcpConnection> &conn, std::int32_t sessionId);
    virtual ~ChatSession();
    /**
     * @brief 被设置为TcpConnection的MessageCallBack
     * 
     * @param conn 
     * @param buffer 缓存读取的消息
     * @param receTime 消息到来的时间
     */
    void onRead(const std::shared_ptr<netlib::TcpConnection> &conn, netlib::Buffer *buffer, base::TimeStamp receTime);
    /**
     * @brief 当好友的状态发送改变，推送消息给user.
     *        type=1：用户上线
     *        type=2：用户下线
     *        type=3：用户个人信息的修改         
     * 
     * @param f_id 好友id
     */
    void sendWhenFriendStatusChange(std::int32_t f_id, int type, int status = 0);
    
public:
    bool vaild() {
        return m_onlineUserInfo.u_id > 0;
    }

    std::int32_t getUserId() {
        return m_onlineUserInfo.u_id;
    }
    /**
     * @brief 获取自身的客户端类型
     * 
     * @return std::int32_t 
     */
    std::int32_t getClientType() {
        return m_onlineUserInfo.u_clientType;
    }
    /**
     * @brief 获取本session的序列号
     * 
     * @return std::int32_t 
     */
    std::int32_t getSeq() {
        return m_seq;
    }
    /**
     * @brief 将session设为无效状态
     * 
     */
    void setInvaild() {
        m_onlineUserInfo.u_id = 0;
    }
    /**
     * @brief 获取session的在线状态
     * 
     * @return std::int32_t 
     */
    std::int32_t getStatus() {
        return m_onlineUserInfo.u_status;
    }
private:
    ChatSession(const ChatSession &obj) = delete;
    ChatSession& operator=(const ChatSession &obj) = delete;
    /**
     * @brief 处理数据，然后根据不同的指令进行处理
     * 
     * @param conn 
     * @param inBuf 除去包头后的data
     * @return int 
     */
    int handleData(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &inBuf);
    /**
     * @brief 注册
     * 
     * @param conn 
     * @param data 
     */
    void toRegister(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 登录
     * 
     * @param conn 
     * @param data 
     */
    void toLogin(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 心跳包
     * 
     * @param conn 
     */
    void toHeartBeat(const std::shared_ptr<netlib::TcpConnection> &conn);
    /**
     * @brief 根据u_name或者u_nickname查找好友
     * 
     * @param conn 
     * @param data 
     */
    void toFindFriend(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 加好友，发出好友申请
     * 
     * @param conn 
     * @param data 
     */
    void toAddFriend(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 删除好友或者退群
     * 
     * @param conn 
     * @param data 
     */
    void toDelFriend(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 回复好友申请
     * 
     * @param conn 
     * @param data 
     */
    void toResponseFriendApply(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 和好友f_id聊天
     * 
     * @param conn 
     * @param data 
     */
    void toChat(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 发送消息到群g_id里
     * 
     * @param conn 
     * @param data 
     */
    void toGroupChat(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 创建群
     * 
     * @param conn 
     * @param data 
     */
    void toCreateGroup(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 修改用户在线状态
     * 
     * @param conn 
     * @param data 
     */
    void toChangeUserStatus(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 修改用户的个人信息
     * 
     * @param conn 
     * @param data 
     */
    void toChangeUserInfo(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 修改用户密码
     * 
     * @param conn 
     * @param data 
     */
    void toChangePassword(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 修改好友分组的信息
     * 
     * @param conn 
     * @param data 
     */
    void toChangeTeamInfo(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 将好友移至其它分组
     * 
     * @param conn 
     * @param data 
     */
    void toMoveFriend2OtherTeam(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 修改好友备注
     * 
     * @param conn 
     * @param data 
     */
    void toChangeFriendRemarks(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 加群，在toAddFriend里被调用
     * 
     * @param conn 
     * @param g_id 
     */
    void toAddGroup(const std::shared_ptr<netlib::TcpConnection> &conn, std::int32_t g_id);
    /**
     * @brief 获取好友列表
     * 
     * @param conn 
     * @param data 
     */
    void toGetFriendsList(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
    /**
     * @brief 获取指定群成员的信息
     * 
     * @param conn 
     * @param data 
     */
    void toGetGroupMember(const std::shared_ptr<netlib::TcpConnection> &conn, const std::string &data);
private:
    std::int32_t                        m_sessionId;
    std::int32_t                        m_seq;
    bool                                m_isLogin;
    time_t                              m_lastPakcageTime;
    netlib::TimerId                     m_timerId;
    OnlineUserInfo                      m_onlineUserInfo;

};   // class ChatSession
    
};   /// namespace IMServer

}   /// namesapce YTalk

#endif  /// YTALK_CHAT_SESSION_H