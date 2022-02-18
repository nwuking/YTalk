/**
 * @file CacheManager.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "CacheManager.h"
#include "../base/Logger.h"

#include <memory>

namespace YTalk
{

using namespace base;

namespace IMServer
{

CacheManager::CacheManager() {
    //
}

CacheManager::~CacheManager() {
    ///
}

int CacheManager::init() {
    //TODO
    return 0;
}

int CacheManager::addCacheNotify(std::int32_t u_id, const std::string &n_data) {
    std::lock_guard<std::mutex> lock(m_notifyMutex);
    CacheNotify cn;
    cn.n_data = n_data;
    if(m_cacheNotifys.find(u_id) == m_cacheNotifys.end()) {
        // 缓存里没有该用户，new
        std::list<CacheNotify> lcn;
        lcn.push_back(cn);
        m_cacheNotifys.insert(std::make_pair(u_id, lcn));
    }
    else {
        m_cacheNotifys[u_id].push_back(cn);
    }

    LOG_INFO("Add cache about notify to user:%d", u_id);
    return 0;
}

void CacheManager::getCacheNotify(std::int32_t u_id, std::list<CacheNotify> &cacheNotifys) {
    std::lock_guard<std::mutex> lock(m_notifyMutex);
    if(m_cacheNotifys.find(u_id) != m_cacheNotifys.end()) {
        cacheNotifys = m_cacheNotifys[u_id];
        m_cacheNotifys.erase(u_id);

        LOG_INFO("Get cache about notify for user:%d", u_id);
    }
}

int CacheManager::addCacheMsg(std::int32_t u_id, const std::string &m_data) {
    std::lock_guard<std::mutex> lock(m_msgMutex);
    CacheMsg cm;
    cm.m_data = m_data;
    if(m_cacheMsgs.find(u_id) == m_cacheMsgs.end()) {
        std::list<CacheMsg> lcm;
        lcm.push_back(cm);
        m_cacheMsgs.insert(std::make_pair(u_id, lcm));
    }
    else {
        m_cacheMsgs[u_id].push_back(cm);
    }

    LOG_INFO("Add cache about msg to user:%d", u_id);
    return 0;
}

void CacheManager::getCacheMsg(std::int32_t u_id, std::list<CacheMsg> &cacheMsgs) {
    std::lock_guard<std::mutex> lock(m_msgMutex);
    if(m_cacheMsgs.find(u_id) != m_cacheMsgs.end()) {
        cacheMsgs = m_cacheMsgs[u_id];
        m_cacheMsgs.erase(u_id);

        LOG_INFO("Get cache about msg for user:%d", u_id);
    }
}

}   /// namespace IMServer

}   // namespace YTalk