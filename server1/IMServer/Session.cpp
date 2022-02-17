/**
 * @file Session.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Session.h"
#include "Protocal.h"
#include "../base/Logger.h"

namespace YTalk
{

using namespace netlib;
using namespace base;

namespace IMServer
{

Session::Session(const std::weak_ptr<TcpConnection> &conn) : m_connection(conn) {
    //
}

Session::~Session() {
    //
}

void Session::send(const std::string &msg) {
    send(msg.c_str(), msg.size());
}

void Session::send(const void *msg, int length) {
    sendPackage(msg, length);
}

void Session::sendPackage(const void *msg, int length) {
    std::string srcMsg(reinterpret_cast<const char*>(msg), length);
    //TODO: 将数据压缩
    std::string compressMsg = srcMsg;
    std::string package;
    PackageHead head;
    head.ph_compress = '0';
    head.ph_compress_size = compressMsg.size();
    head.ph_src_size = length;
    package.append(reinterpret_cast<const char*>(&head), sizeof head);
    package += srcMsg;

    if(m_connection.expired()) {
        LOG_ERROR("Fail to send package, TcpConnection is destroyed");
        return;
    }

    std::shared_ptr<TcpConnection> conn = m_connection.lock();
    if(conn) {
        //LOG_INFO("");
        conn->send(package);
    }
}


}  // namespace IMServer

}   // namespace YTalk