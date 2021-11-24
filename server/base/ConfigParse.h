/*================================================================================   
 *    Date: 2021-11-21
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_CONFIGPARSE_H
#define YTALK_CONFIGPARSE_H

#include <string>
#include <vector>
#include <unordered_map>
//#include <butil/containers/flat_map.h>    // brpc

namespace YTalk
{

class ConfigParse
{
public:
    ConfigParse() : _status(true) {}
    ~ConfigParse() = default;

    void parse(const std::string &path);
    void getValue(const std::string &key, std::string &value);
    void getValue(const std::string &key, int &value);
    void getValue(const std::string &key, long &value);
    void getValue(const std::string &key, long long &value);
    void getValue(const std::string &key, std::vector<std::string> &value);
    bool isExist(const std::string &key);
    bool status() const {
        return _status;
    }

    // for test
    int size() const {
        return _flatMap.size();
    }
    //TODO

private:
    typedef std::string Key;
    typedef std::vector<std::string> Value;

    //butil::FlatMap<Key, Value> _flatMap;
    std::unordered_map<Key, Value> _flatMap;
    bool _status;

};    // class ConfigParse

}    // namesapce YTalk

#endif