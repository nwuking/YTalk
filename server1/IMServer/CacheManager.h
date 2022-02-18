/**
 * @file CacheManager.h
 * @author nwuking@qq.com
 * @brief 缓存管理，离线消息的缓存，离线通知的缓存
 * @version 0.1
 * @date 2022-02-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_CACHE_MANAGER_H
#define YTALK_CACHE_MANAGER_H

#include <list>
#include <unordered_map>
#include <cstdint>
#include <mutex>

namespace YTalk
{

namespace IMServer
{

typedef struct CacheMsg {
    std::int32_t            m_id;               // 消息id
    time_t                  m_time;             // 群消息的时候使用
    std::string             m_data;             // 聊天消息实体
} CacheMsg;

typedef struct GroupMember {
    std::int32_t            gm_id;              // 组成员id
    time_t                  gm_lastAckTime;     // 最后接收cache的时间   
} GroupCacheMsg;

typedef struct CacheNotify {
    std::int32_t            n_id;               // 通知消息id
    std::string             n_data;             // 通知消息实体
} CacheNotify;

class CacheManager
{
public:
    CacheManager();
    ~CacheManager();
    /**
     * @brief 初始化，从数据库读取缓存消息
     * 
     * @return int 
     */
    int init();
    /**
     * @brief u_id离线，给u_id缓存通知消息
     * 
     * @param u_id 
     * @param n_data 
     * @return int 
     */
    int addCacheNotify(std::int32_t u_id, const std::string &n_data);
    /**
     * @brief 获取u_id的离线通知消息
     * 
     * @param u_id 
     * @param cacheNotifys 
     */
    void getCacheNotify(std::int32_t u_id, std::list<CacheNotify> &cacheNotifys);
    /**
     * @brief 缓存u_id的离线聊天消息
     * 
     * @param u_id 
     * @param m_data 
     * @return int 
     */
    int addCacheMsg(std::int32_t u_id, const std::string &m_data);
    /**
     * @brief 获取u_id的离线聊天消息
     * 
     * @param u_id 
     * @param cacheMsgs 
     */
    void getCacheMsg(std::int32_t u_id, std::list<CacheMsg> &cacheMsgs);

private:
    CacheManager(const CacheManager &obj) = delete;
    CacheManager& operator=(const CacheManager &obj) = delete;

private:
    std::unordered_map<std::int32_t, std::list<CacheMsg> >       m_cacheMsgs;
    std::unordered_map<std::int32_t, std::list<GroupMember>>     m_groupMembers;
    std::unordered_map<std::int32_t, std::list<CacheNotify>>     m_cacheNotifys;
    std::mutex                                                   m_msgMutex;
    std::mutex                                                   m_notifyMutex;

};   // class CacheManager

}   /// namespace IMServer

}   // namespace YTalk

#endif  // YTALK_CACHE_MANAGER_H