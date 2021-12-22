/*================================================================================   
 *    Date: 2021-12-20
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "Session.h"

namespace YTalk
{

Session* Session::_session = nullptr;

Session::Session() : _mutex()/*, _cond(_mutex)*/ {
    //
}

Session::~Session() {
    for(auto &it : _gate_server_map) {
        delete it.second;
        it.second = nullptr;
    }
}

void Session::record(const std::string &server_name, int server_port, struct in_addr &server_ip) {
    std::string ip_str = inet_ntoa(server_ip);
    GateServerMsg *gateServer = new GateServerMsg(ip_str, server_port);

    MutexLock lock(_mutex);
    _clientsPerServer.insert(std::make_pair(server_name, 0));
    _gate_server_map.insert(std::make_pair(server_name, gateServer));
}

void Session::record(const std::string &server_name, int counts) {
    MutexLock lock(_mutex);
    std::unordered_map<std::string, int>::iterator it = _clientsPerServer.find(server_name);
    if(it != _clientsPerServer.end()) {
        it->second = counts;
    }
}

void Session::record(const std::string &server_name) {
    MutexLock lock(_mutex);
    _clientsPerServer.erase(server_name);
    std::unordered_map<std::string, GateServerMsg*>::iterator it = _gate_server_map.find(server_name);
    if(it != _gate_server_map.end()) {
        delete it->second;
        it->second = nullptr;
        _gate_server_map.erase(server_name);
    }
}

struct GateServerMsg* Session::getGateServerMsg() {
    MutexLock lock(_mutex);
    if(_clientsPerServer.empty()) {
        return nullptr;
    }

    std::unordered_map<std::string, int>::iterator it = _clientsPerServer.begin();
    int count = it->second;
    std::string server_name = it->first;
    ++it;
    for(; it != _clientsPerServer.end(); ++it) {
        if(it->second < count) {
            count = it->second;
            server_name = it->first;
        }
    }

    return _gate_server_map[server_name];
}

}    // namespace YTalk