/**
 * @file ConfigParse.
 * @author nwuking@qq.com
 * @brief 配置类，读取配置文件
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_CONFIG_PARSE_H
#define YTALK_CONFIG_PARSE_H

#include <unordered_map>
#include <string>

#define CONFIG_API

namespace YTalk
{

namespace base
{

class CONFIG_API ConfigParse 
{
public:
    /**
     * @brief object被构造的时候，读取配置文件并解析到Map里
     * 
     * @param fileName 配置文件名
     */
    ConfigParse(const std::string &fileName);
    ~ConfigParse();
    /**
     * @brief Get the Config Value object
     * 
     * @param key 
     * @return std::string 
     */
    std::string getConfigValue(const std::string &key);

private:
    /**
     * @brief 解析读取到的每一行数据
     * 
     * @param buf 一行数据
     */
    void parse(char *buf);
    /**
     * @brief 去掉多余的空格
     * 
     * @param str 返回值，buf去掉空格后的值
     * @param buf 
     */
    void removeSpace(std::string &str, char *buf);

private:

    std::string m_fileName;
    std::unordered_map<std::string, std::string> m_configMap;
};   // class ConfigParse

}   // namespace base

}   // namespace YTalk

#endif     //  YTALK_CONFIG_PARSE_H