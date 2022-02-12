/**
 * @file Types.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_TYPES_H
#define YTALK_TYPES_H

#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>

#include <unistd.h>
#include <stdint.h>
#include <endian.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <inttypes.h>
#include <errno.h>
#include <dirent.h>


#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/eventfd.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/epoll.h>
#include <sys/syscall.h>

#include <cstring>

namespace YTalk
{

namespace netlib
{

#define SOCKET int

#define INVALID_SOCKET -1

#define SOCKET_ERROR -1

}   /// namespace netlib

}  // namespace YTalk

#endif  // YTALK_TYPES_H