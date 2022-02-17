/**
 * @file Session.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_SESSION_H
#define YTALK_SESSION_H

#include <memory>

#include "../netlib/TcpConnection.h"

namespace YTalk
{

namespace IMServer
{

class Session
{
public:
    Session(const std::weak_ptr<netlib::TcpConnection> &conn);
    virtual ~Session();

    void send(const std::string &msg);
    void send(const void *msg, int length);

    std::shared_ptr<netlib::TcpConnection> getTcpConnectionPtr() {
        if(m_connection.expired()) {
            return nullptr;
        }
        return m_connection.lock();
    }

private:
    Session(const Session &obj) = delete;
    Session& operator=(const Session &obj) = delete;

    void sendPackage(const void *msg, int length);

private:
    std::weak_ptr<netlib::TcpConnection>        m_connection;

};     // class Session

}  // namespace IMServer

}   // namespace YTalk

#endif