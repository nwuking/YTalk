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
    _rContent = redisConnectWithTimeout(_redisPool->getRedisHost(), _redisPool->getRedisPort(), timeout);
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

    redisReply *reply = static_cast<redisReply*>(redisCommand(_rContent, "SELECT %d", _redisPool->getRedisDbNum()));
    if(reply && (reply->type == REDIS_REPLY_STATUS) && (strncmp(reply->str, "OK", 2) == 0)) {
        LOG(INFO) << "select cache db:" << _redisPool->getRedisDbNum();
        freeReplyObject(reply);
        return 0;
    }
    else {
        LOG(ERROR) << "select cache db:" << _redisPool->getRedisDbNum() << " failed";
        return 2;
    }
}

/// string type
std::string RedisConn::get(const char *key) {
    std::string value;
    if(init()) {
        return value;
    }

    redisReply *reply = static_cast<redisReply*>(redisCommand(_rContent, "GET %s", key));
    if(!reply) {
        LOG(ERROR) << "GET redisCommand failed: " << _rContent->errstr;
        redisFree(_rContent);
        _rContent = nullptr;
        return value;
    }

    if(reply->type == REDIS_REPLY_STRING) {
        value.append(reply->str, reply->len);
    }

    freeReplyObject(reply);
    return value;
}

std::string RedisConn::set(const char *key, const char *value) {
    std::string retValue;
    if(init()) {
        return retValue;
    }

    redisReply *reply = static_cast<redisReply*>(redisCommand(_rContent, "SET %s %s", key, value));

    if(!reply) {
        LOG(ERROR) << "SET redisCommand failed: " << _rContent->errstr;
        redisFree(_rContent);
        _rContent = nullptr;
        return retValue;
    }

    retValue.append(reply->str, reply->len);
    freeReplyObject(reply);
    return retValue;
}

std::string RedisConn::setex(const char *key, const char *value, int timeout) {
    std::string retValue;
    if(init()) {
        return retValue;
    }

    redisReply *reply = static_cast<redisReply*>(redisCommand(_rContent, "SETEX %s %d %s", key, timeout, value));

    if(!reply) {
        LOG(ERROR) << "SETEX redisCommand failed: " << _rContent->errstr;
        redisFree(_rContent);
        _rContent = nullptr;
        return retValue;
    }

    retValue.append(reply->str, reply->len);
    freeReplyObject(reply);
    return retValue;
}

bool RedisConn::mget(const std::vector<std::string> &keys, std::unordered_map<std::string, std::string> &retValue) {
    if(init()) {
        return false;
    }

    if(keys.empty()) {
        return false;
    }

    std::string key;
    std::vector<std::string>::const_iterator it = keys.begin();
    key = *it;
    ++it;
    for(; it != keys.end(); ++it) {
        key = key + " " + *it;
    }

    if(key.empty()) {
        return false;
    }

    redisReply *reply = static_cast<redisReply*>(redisCommand(_rContent, "MGET %s", key.c_str()));
    if(!reply) {
        LOG(ERROR) << "MGET redisCommand failed: " << _rContent->errstr;
        redisFree(_rContent);
        _rContent = nullptr;
        return false;
    }

    if(reply->type == REDIS_REPLY_ARRAY) {
        for(size_t i = 0; i < reply->elements; ++i) {
            redisReply *rep = reply->element[i];
            if(rep->type == REDIS_REPLY_STRING) {
                retValue.insert(std::make_pair(keys[i], rep->str));
            }
        }
    }

    freeReplyObject(reply);
    return true;
}

bool RedisConn::isExits(const char *key) {
    if(init()) {
        return false;
    }

    redisReply *reply = static_cast<redisReply*>(redisCommand(_rContent, "EXISTS %s", key));
    if(!reply) {
        LOG(ERROR) << "EXISTS redisCommand failed: " << _rContent->errstr;
        redisFree(_rContent);
        _rContent = nullptr;
        return false;
    }

    long retValue = reply->integer;
    freeReplyObject(reply);

    if(0 == retValue) {
        return false;
    }
    return true;
}

/// hash type


}    // namespace YTalk