/*================================================================================
 *    Date: 2021-11-21
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "./ConfigParse.h"

#include <stdio.h>
#include <string.h>

namespace YTalk
{

static void skipBlank(char **cur) {
    while(**cur == ' ') {
        ++(*cur);
    }
}

void ConfigParse::parse(const std::string &path) {
    FILE *fd = fopen(path.data(), "r");
    if(!fd) {
        _status = false;
        fprintf(stderr, "Faild to open the config file!");
        return;
    }
    while(!feof(fd)) {
        char buf[1024];
        if(!fgets(buf, sizeof buf, fd)) {
            continue;
        }
        char *cur = buf;
        if(cur[0] == '#' || cur[0] == '\n') {
            continue;
        }
        cur = strchr(buf, '\n');
        if(cur) {
            *cur = '\0';
        }
        cur = buf;
        // key
        skipBlank(&cur);
        char *start = cur;
        while(*cur != '\0' && *cur != ' ' && *cur != '=') {
            ++cur;
        }
        //++cur;
        std::string key(start, cur);
        skipBlank(&cur);
        if(*cur++ != '=') {
            _status = false;
            fprintf(stderr, "The config format is error!\n");
            return;
        }
        // value
        std::vector<std::string> vals;
        while(*cur != '\0') {
            skipBlank(&cur);
            if(*cur == ',') {
                ++cur;
                continue;
            }
            char *s = cur;
            while(*cur != ',' && *cur != ' ' && *cur != '\0') {
                ++cur;
            }
            //++cur;
            std::string value(s, cur);
            vals.push_back(value);
        }
        _flatMap[key] = vals;
    }
    fclose(fd);
}

void ConfigParse::getValue(const std::string &key, std::string &value) {
    if(_flatMap.count(key) && _flatMap[key].size() > 0) {
        value = _flatMap[key][0];
    }
}

void ConfigParse::getValue(const std::string &key, int &value) {
    if(_flatMap.count(key) && _flatMap[key].size() > 0) {
        std::string v = _flatMap[key][0];
        value = std::stoi(v);
    }   
}

void ConfigParse::getValue(const std::string &key, long &value) {
    if(_flatMap.count(key) && _flatMap[key].size() > 0) {
        std::string v = _flatMap[key][0];
        value = std::stol(v);
    }
}

void ConfigParse::getValue(const std::string &key, long long &value) {
    if(_flatMap.count(key) && _flatMap[key].size() > 0) {
        std::string v = _flatMap[key][0];
        value = std::stoll(v);
    }
}

void ConfigParse::getValue(const std::string &key, std::vector<std::string> &value) {
    if(_flatMap.count(key)) {
        value = _flatMap[key];
    }
}

bool ConfigParse::isExist(const std::string &key) {
   if(_flatMap.count(key)) {
       return true;
   }
   return false;
}

}    // namesapce YTalk