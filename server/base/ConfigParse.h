/*================================================================================   
 *    Date:
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_CONFIGPARSE_H
#define YTALK_CONFIGPARSE_H

#include <string>
#include <vector>

#include <butil/containers/flat_map.h>    // brpc

namespace YTalk
{

class ConfigParse
{
public:
    ConfigParse() = default;
    ~ConfigParse() = default;

    void parse(const std::string &path);
    void getValue(const std::string &key, std::string &value);
    void getValue(const std::string &key, std::vector<std::string> &value);

private:
    typedef std::string Key;
    typedef std::vector<std::string> Value;

    butil::FlatMap<Key, Value> _flatMap;

};    // class ConfigParse

}    // namesapce YTalk

#endif