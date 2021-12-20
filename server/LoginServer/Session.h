/*================================================================================   
 *    Date: 2021-12-20
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_SESSION_H
#define YTALK_SESSION_H

#include <unordered_map>
#include <string>
#include <arpa/inet.h>

//#include "base/Condition.h"
#include "base/Mutex.h"

namespace YTalk
{

struct GateServerMsg
{
    GateServerMsg(const std::string &ip, const std::string &port) {
        _ip = ip;
        _port = port;
    }

    std::string _ip;
    std::string _port;
};

class Session
{
public:
    static Session* getInstance() {
        if(!_session) {
            _session = new Session();
        }

        return _session;
    }
    static void freeSession() {
        if(_session) {
            delete _session;
        }
        _session = nullptr;
    }

    void record(const std::string &server_name, const std::string &server_port, struct in_addr &server_ip);

    struct GateServerMsg* getGateServerMsg();

private:
    Session();
    ~Session();

    std::unordered_map<std::string, struct GateServerMsg*> _gate_server_map;
    std::unordered_map<std::string, int> _clientsPerServer;

    static Session *_session;
    Mutex _mutex;
    //Cond _cond;
};   // class Session

}     // namespace YTalk

#endif