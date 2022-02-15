/**
 * @file Buffer.h
 * @author nwuking@qq.com
 * @brief Buffer，用于数据的缓存
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

public:
    /**
     * @brief 从文件描述符中读取数据
     * 
     * @param fd 描述符
     * @param savedErrno 保存读取数据时的错误码
     * @return int32_t 
     */
    int32_t readFd(int fd, int* savedErrno);

public:
    /**
     * @brief Construct a new Buffer object
     * 
     * @param initialSize 
     */
    explicit Buffer(size_t initialSize = kInitialSize) : 
        m_buffer(kCheapPrepend + initialSize),
        m_readerIndex(kCheapPrepend),
        m_writerIndex(kCheapPrepend)
    {
    }
    /**
     * @brief 
     * 
     * @param rhs 
     */
    void swap(Buffer& rhs) {
        m_buffer.swap(rhs.m_buffer);
        std::swap(m_readerIndex, rhs.m_readerIndex);
        std::swap(m_writerIndex, rhs.m_writerIndex);
    }
    /**
     * @brief Buffer中可读的数据大小
     * 
     * @return size_t 
     */
    size_t readableBytes() const {
        return m_writerIndex - m_readerIndex;
    }
    /**
     * @brief Buffer中可写空间的大小
     * 
     * @return size_t 
     */
    size_t writableBytes() const {
        return m_buffer.size() - m_writerIndex;
    }
    /**
     * @brief Buffer中开头部分的空余空间
     * 
     * @return size_t 
     */
    size_t prependableBytes() const
    {
        return m_readerIndex;
    }
    /**
     * @brief 寻找可读数据的开始地址
     * 
     * @return const char* 
     */
    const char* peek() const
    {
        return begin() + m_readerIndex;
    }
    /**
     * @brief 查找Buffer中是否有相应的字符
     * 
     * @param targetStr 
     * @return const char* 
     */
    const char* findString(const char* targetStr) const
    {
        const char* found = std::search(peek(), beginWrite(), targetStr, targetStr + strlen(targetStr));
        return found == beginWrite() ? nullptr : found;
    }
    /**
     * @brief 
     * 
     * @return const char* 
     */
    const char* findCRLF() const {
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? nullptr : crlf;
    }
    /**
     * @brief 
     * 
     * @param start 
     * @return const char* 
     */
    const char* findCRLF(const char* start) const {
        if (peek() > start)
            return nullptr;

        if (start > beginWrite())
            return nullptr;

        const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? nullptr : crlf;
    }
    /**
     * @brief 
     * 
     * @return const char* 
     */
    const char* findEOL() const {
        const void* eol = memchr(peek(), '\n', readableBytes());
        return static_cast<const char*>(eol);
    }
    /**
     * @brief 
     * 
     * @param start 
     * @return const char* 
     */
    const char* findEOL(const char* start) const {
        if (peek() > start)
            return nullptr;

        if (start > beginWrite())
            return nullptr;

        const void* eol = memchr(start, '\n', beginWrite() - start);
        return static_cast<const char*>(eol);
    }
    /**
     * @brief 取回使用的空间
     * 
     * @param len 大小
     * @return true 
     * @return false 
     */
    bool retrieve(size_t len) {
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
    /**
     * @brief 
     * 
     * @param end 
     * @return true 
     * @return false 
     */
    bool retrieveUntil(const char* end) {
        if (peek() > end)
            return false;

        if (end > beginWrite())
            return false;

        retrieve(end - peek());

        return true;
    }

    void retrieveInt64() {
        retrieve(sizeof(int64_t));
    }

    void retrieveInt32() {
        retrieve(sizeof(int32_t));
    }

    void retrieveInt16() {
        retrieve(sizeof(int16_t));
    }

    void retrieveInt8() {
        retrieve(sizeof(int8_t));
    }
    /**
     * @brief 取回所有的空间
     * 
     */
    void retrieveAll() {
        m_readerIndex = kCheapPrepend;
        m_writerIndex = kCheapPrepend;
    }
    /**
     * @brief 将可读数据以字符串返回，并取回空间
     * 
     * @return std::string 
     */
    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());;
    }
    /**
     * @brief 
     * 
     * @param len 
     * @return std::string 
     */
    std::string retrieveAsString(size_t len) {
        if (len > readableBytes())
            return "";

        std::string result(peek(), len);
        retrieve(len);
        return result;
    }
    /**
     * @brief 将可读1数据以字符串返回
     * 
     * @return std::string 
     */
    std::string toStringPiece() const {
        return std::string(peek(), static_cast<int>(readableBytes()));
    }
    /**
     * @brief 在Buffer后添加数据
     * 
     * @param str 要添加的字符
     */
    void append(const std::string& str) {
        append(str.c_str(), str.size());
    }
    /**
     * @brief 添加数据
     * 
     */
    void append(const char* /*restrict*/ data, size_t len) {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }
    /**
     * @brief 
     * 
     */
    void append(const void* /*restrict*/ data, size_t len) {
        append(static_cast<const char*>(data), len);
    }
    /**
     * @brief 确保有充足的空间写数据，空间不足会扩展空间
     * 
     * @param len 写数据的长度
     * @return true 
     * @return false 
     */
    bool ensureWritableBytes(size_t len) {
        //剩下的可写空间如果小于需要的空间len，则增加len长度个空间
        if (writableBytes() < len)
        {
            makeSpace(len);
        }

        return true;
    }
    /**
     * @brief Buffer中可写空间的开始地址
     * 
     * @return char* 
     */
    char* beginWrite() {
        return begin() + m_writerIndex;
    }
    /**
     * @brief 
     * 
     * @return const char* 
     */
    const char* beginWrite() const {
        return begin() + m_writerIndex;
    }
    /**
     * @brief 更新写到哪里的位置
     * 
     * @param len 
     * @return true 
     * @return false 
     */
    bool hasWritten(size_t len) {
        if (len > writableBytes())
            return false;
        m_writerIndex += len;
        return true;
    }
    /**
     * @brief 将写的位置后退len各字节
     * 
     * @param len 
     * @return true 
     * @return false 
     */
    bool unwrite(size_t len) {
        if (len > readableBytes())
            return false;

        m_writerIndex -= len;
        return true;
    }
    /**
     * @brief 添加一个int64类型整数
     * 
     * @param x 
     */
    void appendInt64(int64_t x)
    {
        int64_t be64 = sockets::hostToNetwork64(x);
        append(&be64, sizeof be64);
    } 
    /**
     * @brief 
     * 
     * @param x 
     */
    void appendInt32(int32_t x) {
        int32_t be32 = sockets::hostToNetwork32(x);
        append(&be32, sizeof be32);
    }
    /**
     * @brief 
     * 
     * @param x 
     */
    void appendInt16(int16_t x) {
        int16_t be16 = sockets::hostToNetwork16(x);
        append(&be16, sizeof be16);
    }
    /**
     * @brief 
     * 
     * @param x 
     */
    void appendInt8(int8_t x) {
        append(&x, sizeof x);
    }
    /**
     * @brief 读一个整型数据
     * 
     * @return int64_t 
     */
    int64_t readInt64() {
        int64_t result = peekInt64();
        retrieveInt64();
        return result;
    }
    /**
     * @brief 
     * 
     * @return int32_t 
     */
    int32_t readInt32() {
        int32_t result = peekInt32();
        retrieveInt32();
        return result;
    }
    /**
     * @brief 
     * 
     * @return int16_t 
     */
    int16_t readInt16() {
        int16_t result = peekInt16();
        retrieveInt16();
        return result;
    }
    /**
     * @brief 
     * 
     * @return int8_t 
     */
    int8_t readInt8() {
        int8_t result = peekInt8();
        retrieveInt8();
        return result;
    }
    /**
     * @brief 
     * 
     * @return int64_t 
     */
    int64_t peekInt64() const {
        if (readableBytes() < sizeof(int64_t))
            return -1;
        int64_t be64 = 0;
        ::memcpy(&be64, peek(), sizeof be64);
        return sockets::networkToHost64(be64);
    }
    /**
     * @brief 
     * 
     * @return int32_t 
     */
    int32_t peekInt32() const {
        if (readableBytes() < sizeof(int32_t))
            return -1;

        int32_t be32 = 0;
        ::memcpy(&be32, peek(), sizeof be32);
        return sockets::networkToHost32(be32);
    }
    /**
     * @brief 
     * 
     * @return int16_t 
     */
    int16_t peekInt16() const {
        if (readableBytes() < sizeof(int16_t))
            return -1;

        int16_t be16 = 0;
        ::memcpy(&be16, peek(), sizeof be16);
        return sockets::networkToHost16(be16);
    }
    /**
     * @brief 
     * 
     * @return int8_t 
     */
    int8_t peekInt8() const {
        if (readableBytes() < sizeof(int8_t))
            return -1;

        int8_t x = *peek();
        return x;
    }
    /**
     * @brief 在Buffer前面添加一个整型的标记
     * 
     * @param x 
     */
    void prependInt64(int64_t x) {
        int64_t be64 = sockets::hostToNetwork64(x);
        prepend(&be64, sizeof be64);
    }
    /**
     * @brief 
     * 
     * @param x 
     */
    void prependInt32(int32_t x) {
        int32_t be32 = sockets::hostToNetwork32(x);
        prepend(&be32, sizeof be32);
    }
    /**
     * @brief 
     * 
     * @param x 
     */
    void prependInt16(int16_t x) {
        int16_t be16 = sockets::hostToNetwork16(x);
        prepend(&be16, sizeof be16);
    }
    /**
     * @brief 
     * 
     * @param x 
     */
    void prependInt8(int8_t x) {
        prepend(&x, sizeof x);
    }
    /**
     * @brief 在Buffer前面添加data
     * 
     */
    bool prepend(const void* /*restrict*/ data, size_t len) {
        if (len > prependableBytes())
            return false;

        m_readerIndex -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d + len, begin() + m_readerIndex);
        return true;
    }
    /**
     * @brief 扩展空间
     * 
     * @param reserve 
     */
    void shrink(size_t reserve) {
        Buffer other;
        other.ensureWritableBytes(readableBytes() + reserve);
        other.append(toStringPiece());
        swap(other);
    }
    /**
     * @brief Buffer的容量
     * 
     * @return size_t 
     */
    size_t internalCapacity() const {
        return m_buffer.capacity();
    }

private:
    /**
     * @brief Buffer的起始地址
     * 
     * @return char* 
     */
    char* begin() {
        return &*m_buffer.begin();
    }
    /**
     * @brief 
     * 
     * @return const char* 
     */
    const char* begin() const {
        return &*m_buffer.begin();
    }
    /**
     * @brief 构造len字节大小的空间
     * 
     * @param len 
     */
    void makeSpace(size_t len) {
        //kCheapPrepend为保留的空间
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            m_buffer.resize(m_writerIndex + len);
        }
        else {
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
};   /// class Buffer

}   // namespace netlib

}   // namespace YTalk


#endif   // YTALK_BUFFER_H