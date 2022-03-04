/**
 * @file ChatService.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ChatService.h"
#include "../base/Logger.h"
#include "../netlib/InetAddress.h"
#include "../base/Singleton.h"
#include "UserManager.h"

using namespace std::placeholders;

namespace YTalk
{

using namespace netlib;
using namespace base;

namespace IMServer
{

ChatService::ChatService() {
    //
}

ChatService::~ChatService() {
    //
}

int ChatService::init(EventLoop *loop, const std::string &ip, std::uint16_t port) {
    InetAddress listeAddr(ip, port);
    m_server.reset(new TcpServer(loop, listeAddr, "YTalk_chat", TcpServer::kReusePort));
    m_server->setConnectionCallBack(std::bind(&ChatService::onConnected, this, std::placeholders::_1));
    m_server->start(4); 
    return 0;
}

void ChatService::stop() {
    m_server.reset(nullptr);
}

void ChatService::onConnected(std::shared_ptr<TcpConnection> conn) {
    if(conn->connected()) {
        fprintf(stderr, "1");
        std::lock_guard<std::mutex> lock(m_sessionMutex);
        ++m_sessionId;
        std::shared_ptr<ChatSession> newSession(new ChatSession(conn, m_sessionId));
        conn->setMessageCallBack(std::bind(&ChatSession::onRead, newSession.get(), _1, _2, _3));
        m_sessions.push_back(newSession);

        printf("1");
    }
    else {
        // 断开
        onDisconnected(conn);
    }
}

void ChatService::onDisconnected(const std::shared_ptr<TcpConnection> &conn) {
    std::lock_guard<std::mutex> lock(m_sessionMutex);
    for(auto it = m_sessions.begin(); it != m_sessions.end(); ++it) {
        if((*it)->getTcpConnectionPtr() == nullptr) {
            LOG_ERROR("TcpConnection is nullptr");
            break;
        }

        if((*it)->getTcpConnectionPtr() == conn) {
            if((*it)->vaild()) {
                //该session有效, 推送下线消息给好友
                std::int32_t u_id = (*it)->getUserId();
                std::vector<std::int32_t> friendId;
                Singleton<UserManager>::getInstance().getFriendIdByUserId(u_id, friendId);
                for(auto f_id : friendId) {
                    for(auto &session : m_sessions) {
                        // 好友在线就推送消息
                        if(f_id == session->getUserId()) {
                            session->sendWhenFriendStatusChange(u_id, 2);
                            break;
                        }
                    }
                }
            }
            else {
                LOG_INFO("ChatSession is invaild, u_id=%d", (*it)->getUserId());
            }

            m_sessions.erase(it);
            LOG_INFO("Client:%s disconnected", conn->peerAddress().toIpPort().c_str());
            break;
        }
    }
}

std::int32_t ChatService::getClientTypeByUserId(std::int32_t u_id) {
    std::int32_t clientType = CLIENT_TYPE_UNKNOW;
    for(const auto &it : m_sessions) {
        if(it->getUserId() == u_id) {
            clientType = it->getClientType();
            break;
        }
    }

    return clientType;
}

std::shared_ptr<ChatSession> ChatService::getSessionByUserIdAndClientType(std::int32_t u_id, std::int32_t clienttype) {
    std::lock_guard<std::mutex> lock(m_sessionMutex);
    for(const auto &session : m_sessions) {
        if(session->getUserId() == u_id && session->getClientType() == clienttype) {
            return session;
        }
    }
    return nullptr;
}

void ChatService::getSessionsByUserId(std::int32_t u_id, std::list<std::shared_ptr<ChatSession>> &sessions) {
    std::lock_guard<std::mutex> lock(m_sessionMutex);
    for(const auto &session : m_sessions) {
        if(session->getUserId() == u_id && session->vaild()) {
            sessions.push_back(session);
        }
    }
}
    
}   // namespace IMServer

}   // namespace YTalk