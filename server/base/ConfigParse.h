/*================================================================================   
 *    Date:
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_CONFIGPARSE_H
#define YTALK_CONFIGPARSE_H

#include <string>
#include <unordered_map>
#include <vector>

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

    std::unordered_map<Key, Value> _confMap;

};    // class ConfigParse

}    // namesapce YTalk

#endif