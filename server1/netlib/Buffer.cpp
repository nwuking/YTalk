/**
 * @file Buffer.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Buffer.h"

namespace YTalk
{

namespace netlib
{

const char Buffer::kCRLF[] = "\r\n";

const size_t Buffer::kCheapPrepend;
const size_t Buffer::kInitialSize;

int32_t Buffer::readFd(int fd, int* savedErrno)
{
    // saved an ioctl()/FIONREAD call to tell how much to read
    char extrabuf[65536];
    const size_t writable = writableBytes();
    struct iovec vec[2];

    vec[0].iov_base = begin() + m_writerIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    // when there is enough space in this ByteBuffer, don't read into extrabuf.
    // when extrabuf is used, we read 128k-1 bytes at most.
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = sockets::readv(fd, vec, iovcnt);
    if (n <= 0)
    {
        * savedErrno = errno;
    }
    else if (size_t(n) <= writable)
    {
        m_writerIndex += n;
    }
    else
    {
        //Linux平台把剩下的字节补上去
        m_writerIndex = m_buffer.size();
        append(extrabuf, n - writable);
    }
    
    return n;
}

}   // namespace netlib

}   // namespace YTalk