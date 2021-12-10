/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "RedisConn.h"
#include "RedisPool.h"
#include "base/Logging.h"

#include <sys/time.h>

namespace YTalk
{

RedisConn::RedisConn(RedisPool *pool) {
    _redisPool = pool;
    _rContent = nullptr;
    _last_connect_time = 0;
}

RedisConn::~RedisConn() {
    if(!_rContent) {
        redisFree(_rContent);
        _rContent = nullptr;
    }
}

int RedisConn::init() {
    if(_rContent) {
        return 0;
    }

    uint64_t now = (uint64_t)time(nullptr);
    if(now < _last_connect_time + 4) {
        return 1;
    }
    _last_connect_time = now;

    // 1s超时
    struct timeval timeout = {1, 0};
    _rContent = redisConnectWithTimeout(_redisPool->getCacheHost(), _redisPool->getCachePort(), timeout);
    if(!_rContent || _rContent->err) {
        if(_rContent) {
            LOG(ERROR) << "redis connect failed: " << _rContent->errstr;
            redisFree(_rContent);
            _rContent = nullptr;
        }
        else {
            LOG(ERROR) << "redis connect failed";
        }
        return 1;
    }

    redisReply *reply = static_cast<redisReply*>(redisCommand(_rContent, "SELECT %d", _redisPool->getCacheDbNum()));
    if(reply && (reply->type == REDIS_REPLY_STATUS) && (strncmp(reply->str, "OK", 2) == 0)) {
        LOG(INFO) << "select cache db:" << _redisPool->getCacheDbNum();
        freeReplyObject(reply);
        return 0;
    }
    else {
        LOG(ERROR) << "select cache db:" << _redisPool->getCacheDbNum() << " failed";
        return 2;
    }
}

/// string type
std::string RedisConn::get(const char *key) {
    //TODO
}

std::string RedisConn::set(const char *key, const char *value) {
    //TODO
}

std::string RedisConn::setex(const char *key, const char *value, int timeout) {
    //TODO
}

bool RedisConn::mget(const std::vector<std::string> &keys, std::unordered_map<std::string, std::string> &retValue) {
    //TODO
}

bool RedisConn::isExits(const char *key) {
    //TODO
}

/// hash type


}    // namespace YTalk