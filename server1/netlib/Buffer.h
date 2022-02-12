/**
 * @file Buffer.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_BUFFER_H
#define YTALK_BUFFER_H

#include "Types.h"
#include "Endian.h"
#include "SocketsOps.h"

#include <algorithm>
#include <vector>
#include <string>
#include <string.h>

namespace YTalk
{

namespace netlib
{

class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : m_buffer(kCheapPrepend + initialSize),
        m_readerIndex(kCheapPrepend),
        m_writerIndex(kCheapPrepend)
    {
    }

    void swap(Buffer& rhs)
    {
        m_buffer.swap(rhs.m_buffer);
        std::swap(m_readerIndex, rhs.m_readerIndex);
        std::swap(m_writerIndex, rhs.m_writerIndex);
    }

    size_t readableBytes() const
    {
        return m_writerIndex - m_readerIndex;
    }

    size_t writableBytes() const
    {
        return m_buffer.size() - m_writerIndex;
    }

    size_t prependableBytes() const
    {
        return m_readerIndex;
    }

    const char* peek() const
    {
        return begin() + m_readerIndex;
    }

    const char* findString(const char* targetStr) const
    {
        const char* found = std::search(peek(), beginWrite(), targetStr, targetStr + strlen(targetStr));
        return found == beginWrite() ? nullptr : found;
    }

    const char* findCRLF() const
    {
        // FIXME: replace with memmem()?
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? nullptr : crlf;
    }

    const char* findCRLF(const char* start) const
    {
        if (peek() > start)
            return nullptr;

        if (start > beginWrite())
            return nullptr;

        // FIXME: replace with memmem()?
        const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? nullptr : crlf;
    }

    const char* findEOL() const
    {
        const void* eol = memchr(peek(), '\n', readableBytes());
        return static_cast<const char*>(eol);
    }

    const char* findEOL(const char* start) const
    {
        if (peek() > start)
            return nullptr;

        if (start > beginWrite())
            return nullptr;

        const void* eol = memchr(start, '\n', beginWrite() - start);
        return static_cast<const char*>(eol);
    }

        
    bool retrieve(size_t len)
    {
        if (len > readableBytes())
             return false;

        if (len < readableBytes())
        {
            m_readerIndex += len;
        }
        else
        {
            retrieveAll();
        }

        return true;
    }

    bool retrieveUntil(const char* end)
    {
        if (peek() > end)
            return false;

        if (end > beginWrite())
            return false;

        retrieve(end - peek());

        return true;
    }

    void retrieveInt64()
    {
        retrieve(sizeof(int64_t));
    }

    void retrieveInt32()
    {
        retrieve(sizeof(int32_t));
    }

    void retrieveInt16()
    {
        retrieve(sizeof(int16_t));
    }

    void retrieveInt8()
    {
        retrieve(sizeof(int8_t));
    }

    void retrieveAll()
    {
        m_readerIndex = kCheapPrepend;
        m_writerIndex = kCheapPrepend;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());;
    }

    std::string retrieveAsString(size_t len)
    {
        if (len > readableBytes())
            return "";

        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    std::string toStringPiece() const
    {
        return std::string(peek(), static_cast<int>(readableBytes()));
    }

    void append(const std::string& str)
    {
        append(str.c_str(), str.size());
    }

    void append(const char* /*restrict*/ data, size_t len)
    {
            //其实相当于把已有数据往前挪动
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }

    void append(const void* /*restrict*/ data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    bool ensureWritableBytes(size_t len)
    {
        //剩下的可写空间如果小于需要的空间len，则增加len长度个空间
        if (writableBytes() < len)
        {
            makeSpace(len);
        }

        return true;
    }

    char* beginWrite()
    {
        return begin() + m_writerIndex;
    }

    const char* beginWrite() const
    {
        return begin() + m_writerIndex;
    }

    bool hasWritten(size_t len)
    {
        if (len > writableBytes())
            return false;
        m_writerIndex += len;
        return true;
    }

    bool unwrite(size_t len)
    {
        if (len > readableBytes())
            return false;

        m_writerIndex -= len;
        return true;
    }

        
    void appendInt64(int64_t x)
    {
        int64_t be64 = sockets::hostToNetwork64(x);
        append(&be64, sizeof be64);
    }

        
    void appendInt32(int32_t x)
    {
        int32_t be32 = sockets::hostToNetwork32(x);
        append(&be32, sizeof be32);
    }

    void appendInt16(int16_t x)
    {
        int16_t be16 = sockets::hostToNetwork16(x);
        append(&be16, sizeof be16);
    }

    void appendInt8(int8_t x)
    {
        append(&x, sizeof x);
    }

        
    int64_t readInt64()
    {
        int64_t result = peekInt64();
        retrieveInt64();
        return result;
    }

        
    int32_t readInt32()
    {
        int32_t result = peekInt32();
        retrieveInt32();
        return result;
    }

    int16_t readInt16()
    {
        int16_t result = peekInt16();
        retrieveInt16();
        return result;
    }

    int8_t readInt8()
    {
        int8_t result = peekInt8();
        retrieveInt8();
        return result;
    }

        
    int64_t peekInt64() const
    {
        if (readableBytes() < sizeof(int64_t))
            return -1;
        int64_t be64 = 0;
        ::memcpy(&be64, peek(), sizeof be64);
        return sockets::networkToHost64(be64);
    }

    
    int32_t peekInt32() const
    {
        if (readableBytes() < sizeof(int32_t))
            return -1;

        int32_t be32 = 0;
        ::memcpy(&be32, peek(), sizeof be32);
        return sockets::networkToHost32(be32);
    }

    int16_t peekInt16() const
    {
        if (readableBytes() < sizeof(int16_t))
            return -1;

        int16_t be16 = 0;
        ::memcpy(&be16, peek(), sizeof be16);
        return sockets::networkToHost16(be16);
    }

    int8_t peekInt8() const
    {
        if (readableBytes() < sizeof(int8_t))
            return -1;

        int8_t x = *peek();
        return x;
    }

        
    void prependInt64(int64_t x)
    {
        int64_t be64 = sockets::hostToNetwork64(x);
        prepend(&be64, sizeof be64);
    }

        
    void prependInt32(int32_t x)
    {
        int32_t be32 = sockets::hostToNetwork32(x);
        prepend(&be32, sizeof be32);
    }

    void prependInt16(int16_t x)
    {
        int16_t be16 = sockets::hostToNetwork16(x);
        prepend(&be16, sizeof be16);
    }

    void prependInt8(int8_t x)
    {
        prepend(&x, sizeof x);
    }

    bool prepend(const void* /*restrict*/ data, size_t len)
    {
        if (len > prependableBytes())
            return false;

        m_readerIndex -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d + len, begin() + m_readerIndex);
        return true;
    }

    void shrink(size_t reserve)
    {
            // FIXME: use vector::shrink_to_fit() in C++ 11 if possible.
        Buffer other;
        other.ensureWritableBytes(readableBytes() + reserve);
        other.append(toStringPiece());
        swap(other);
    }

    size_t internalCapacity() const
    {
        return m_buffer.capacity();
    }

        
    int32_t readFd(int fd, int* savedErrno);

private:
    char* begin()
    {
        return &*m_buffer.begin();
    }

    const char* begin() const
    {
        return &*m_buffer.begin();
    }

    void makeSpace(size_t len)
    {
        //kCheapPrepend为保留的空间
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            // FIXME: move readable data
            m_buffer.resize(m_writerIndex + len);
        }
        else
        {
            // move readable data to the front, make space inside buffer
            //assert(kCheapPrepend < readerIndex_);
            if (kCheapPrepend >= m_readerIndex)
                return;
            size_t readable = readableBytes();
            std::copy(begin() + m_readerIndex,
                begin() + m_writerIndex,
                begin() + kCheapPrepend);
            m_readerIndex = kCheapPrepend;
            m_writerIndex = m_readerIndex + readable;
        }
    }

private:
    std::vector<char>           m_buffer;
    size_t                      m_readerIndex;
    size_t                      m_writerIndex;

    static const char           kCRLF[];
};

}   // namespace netlib

}   // namespace YTalk


#endif   // YTALK_BUFFER_H