/**
 * @file ConfigParse.
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ConfigParse.h"

#include <cstdio>
#include <cstring>

namespace YTalk
{

namespace base
{

ConfigParse::ConfigParse(const std::string &fileName) {
    m_fileName = fileName;

    FILE *fp = fopen(m_fileName.c_str(), "r");
    if(fp == nullptr) {
        return;
    }

    char buf[256];
    while(1) {
        char *p = fgets(buf, 256, fp);
        if(p == nullptr) {
            break;
        }

        // 去掉'\n'
        std::size_t len = strlen(buf);
        if(buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }

        // 去掉注释
        char *ch = strchr(buf, '#');
        if(ch) {
            *ch = '\0';
        }
        if(strlen(buf) != 0) {
            // 解析每一行
            parse(buf);
        }
    }
}

ConfigParse::~ConfigParse() {
    //TODO
}

void ConfigParse::parse(char *buf) {
    char *ch = strchr(buf, '=');
    if(ch == nullptr) {
        return;
    }

    *ch = '\0';

    std::string key, value;
    removeSpace(key, buf);
    removeSpace(value, ch+1);

    if(!key.empty() && !value.empty()) {
        m_configMap.insert(std::make_pair(key, value));
    }
}

void ConfigParse::removeSpace(std::string &str, char *buf) {
    char *start = buf;
    while((*start == ' ') || (*start == '\t') || (*start == '\r')) {
        ++start;
    }
    if(*start == '\0') {
        return;
    }

    char *end = buf + strlen(buf) - 1;
    while ((*end == ' ') || (*end == '\t') || (*end == '\t')) {
        --end;
    }

    int len = end - start + 1;
    str.append(start, len);
}

std::string ConfigParse::getConfigValue(const std::string &key) {
    std::unordered_map<std::string, std::string>::iterator it = m_configMap.find(key);
    if(it != m_configMap.end()) {
        return it->second;
    }
    return std::string();
}

}   // namespace base

}   // namespace YTalk