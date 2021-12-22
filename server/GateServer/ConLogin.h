/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_CONLOGIN_H
#define YTALK_CONLOGIN_H

#include "brpc/channel.h"

namespace YTalk
{

class ConfigParse;

class ConLogin
{
public:
    ConLogin();
    virtual ~ConLogin();

    int init(ConfigParse *cParse);

private:
    brpc::Channel _channel;

    //static ConLogin *_conLogin;
};    /// class ConLogin

}   //// namesapce YTalk

#endif