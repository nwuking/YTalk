/*================================================================================   
 *    Date: 2022-2-8
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_TOKEN_H
#define YTALK_TOKEN_H

#include "openssl/evp.h"
#include "openssl/rsa.h"

#include <string>

namespace YTalk
{

class Token
{
public:
    Token();
    virtual ~Token();

    int init();

    bool sign(const std::string &digest, std::string &signature);

    bool verify(const std::string &digest, const std::string &signature);

private:
    unsigned long _e;
    BIGNUM *_bne;
    RSA *_rkey;
    EVP_PKEY *_pkey;

};  // class Token

}  // namespace YTalk

#endif