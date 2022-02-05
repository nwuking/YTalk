/*================================================================================   
 *    Date: 2022-1-21
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_SIGNATURE_H
#define YTALK_SIGNATURE_H

#include "openssl/evp.h"
//#include "openssl/core_names.h"
//#include "openssl/decoder.h"

#include <string>

namespace YTalk
{

enum KEY_FLAG {
    PUBLIC_KEY = 0,
    PRIVATE_KEY
};

class Signature
{
public:
    static void sign(const std::string &plaintext, std::string &signature) {
        signature = "todo";
    }

    static bool verify(const std::string &digest, const std::string &signature) {
        //TODO
    }


    /*
    For sign, load private key from priv_key_der[]
    For cerify, load public key from pub_key_der[]
    */
    //static EVP_PKEY* getKey(OSSL_LIB_CTX *libctx, const char *propq, int flag);

}; // class Signature

}  /// namespace YTalk

#endif