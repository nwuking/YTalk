/**
 * @file SocketsOps.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_SOCKETS_OPS_H
#define YTALK_SOCKETS_OPS_H

#include <arpa/inet.h>
#include <cstdint>

#include "Types.h"  

namespace YTalk
{

namespace netlib
{

namespace sockets
{
        SOCKET createOrDie();
        SOCKET createNonblockingOrDie();

        void setNonBlockAndCloseOnExec(SOCKET sockfd);

        void setReuseAddr(SOCKET sockfd, bool on);
        void setReusePort(SOCKET sockfd, bool on);

        SOCKET connect(SOCKET sockfd, const struct sockaddr_in& addr);
        void bindOrDie(SOCKET sockfd, const struct sockaddr_in& addr);
        void listenOrDie(SOCKET sockfd);
        SOCKET accept(SOCKET sockfd, struct sockaddr_in* addr);
        int32_t read(SOCKET sockfd, void* buf, int32_t count);
        ssize_t readv(SOCKET sockfd, const struct iovec* iov, int iovcnt);
        int32_t write(SOCKET sockfd, const void* buf, int32_t count);
        void close(SOCKET sockfd);
        void shutdownWrite(SOCKET sockfd);

        void toIpPort(char* buf, size_t size, const struct sockaddr_in& addr);
        void toIp(char* buf, size_t size, const struct sockaddr_in& addr);
        void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);

        int getSocketError(SOCKET sockfd);

        const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
        struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);
        const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
        struct sockaddr_in* sockaddr_in_cast(struct sockaddr* addr);

        struct sockaddr_in getLocalAddr(SOCKET sockfd);
        struct sockaddr_in getPeerAddr(SOCKET sockfd);
        bool isSelfConnect(SOCKET sockfd);
}  // namespace sockets

}   /// namesapce netlib

}   // namesapce YTalk

#endif  // YTALK_SOCKETS_OPS_H