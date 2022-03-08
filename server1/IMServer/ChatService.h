/**
 * @file ChatService.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_CHAT_SERVICE_H
#define YTALK_CHAT_SERVICE_H

#include "../netlib/EventLoop.h"
#include "../netlib/TcpServer.h"
#include "../netlib/TcpConnection.h"
#include "ChatSession.h"

#include <string>
#include <cstdint>
#include <mutex>
#include <memory>
#include <list>
#include <atomic>

namespace YTalk
{

namespace IMServer
{

class ChatService
{
public:
    ChatService();
    ~ChatService();

    int init(netlib::EventLoop *loop, const std::string &ip, std::uint16_t port);

    void stop();
    /**
     * @brief 通过用户id，查找用户的客户端类型
     * 
     * @param u_id 
     * @return std::int32_t 
     */
    std::int32_t getClientTypeByUserId(std::int32_t u_id);
    /**
     * @brief 通过用户id和客户端类型获取在线用户的session
     * 
     * @param u_id 
     * @param clienttype 
     * @return std::shared_ptr<ChatSession> 返回在线用户的session
     */
    std::shared_ptr<ChatSession> getSessionByUserIdAndClientType(std::int32_t u_id, std::int32_t clienttype);
    /**
     * @brief 因为支持不同的端同时在线，u_id可能会有多个session,get its
     * 
     * @param u_id 
     * @param sessions 
     */
    void getSessionsByUserId(std::int32_t u_id, std::list<std::shared_ptr<ChatSession>> &sessions);
    /**
     * @brief 通过用户id，获取用户的在线状态
     * 
     * @param u_id 
     * @return std::int32_t 
     */
    std::int32_t getStatusByUserId(std::int32_t u_id);
private:
    /**
     * @brief 新连接到来或者断开连接的时候调用
     * 
     * @param conn 
     */
    void onConnected(std::shared_ptr<netlib::TcpConnection> conn);
    /**
     * @brief 断开连接，会在onConnected里被调用
     * 
     * @param conn 
     */
    void onDisconnected(const std::shared_ptr<netlib::TcpConnection> &conn);

private:
    std::unique_ptr<netlib::TcpServer>                              m_server;
    std::atomic<int>                                                m_sessionId;
    std::list<std::shared_ptr<ChatSession>>                         m_sessions;
    std::mutex                                                      m_sessionMutex;

};   // class ChatService
    
}   // namespace IMServer

}   // namespace YTalk

#endif   // YTALK_CHAT_SERVICE_H