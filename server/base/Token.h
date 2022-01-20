/*================================================================================   
 *    Date: 2022-1-7
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_TOKEN_H
#define YTALK_TOKEN_H

#include <string>

#include "Base64.h"
#include "Time.h"

namespace YTalk
{

class TokenGenerator
{
public:
    static std::string generateToken(const std::string &username) {
        std::string header_cip;
        Base64::encode(header, header_cip);

        Time now = Time::now();
        std::string nbf = now.timeToStringLocal();

        std::string payload = "{"
                                    "\"iss\": \"YTalk\","
                                    "\"aud\": \"" + username + "\","
                                    "\"nbf\": " + nbf + "" 
                              "}";
        std::string payload_cip;
        Base64::encode(payload, payload_cip);

        std::string plaintext = header_cip + "." + payload_cip;

        std::string signature;
        //TODO
        return (plaintext + "." + signature);
    }

    static const std::string header;
};

const std::string TokenGenerator::header = "{"
                                                "\"alg\": \"HS256\","
                                                "\"typ\": \"JWT\""
                                           "}";


}

#endif