/*================================================================================   
 *    Date: 2022-1-7
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_DIGEST_H
#define YTALK_DIGEST_H

#include <string>

#include "Base64.h"
#include "Time.h"

namespace YTalk
{

class Digest
{
public:
    static std::string generateDigest(const std::string &username) {
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

        std::string digest = header_cip + "." + payload_cip;

        return digest;
    }

    static const std::string header;
};

const std::string Digest::header = "{"
                                                "\"alg\": \"SHA3-512\","
                                                "\"typ\": \"JWT\""
                                           "}";

}

#endif