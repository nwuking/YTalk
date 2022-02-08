/*================================================================================   
 *    Date: 2022-2-8
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "Token.h"
#include "base/Logging.h"

namespace YTalk
{

Token::Token() : _e(RSA_3), _bne(nullptr), _rkey(nullptr), _pkey(nullptr) {
    //TODO
}

Token::~Token() {
    //TODO
}

int Token::init() {
    //生成RSA密钥
    _bne = BN_new();
    int ret = BN_set_word(_bne, _e);
    _rkey = RSA_new();
    ret = RSA_generate_key_ex(_rkey, 1024, _bne, nullptr);
    if(ret != 1) {
        return 1;
    }
    _pkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(_pkey, _rkey);
    return 0;
}

bool Token::sign(const std::string &digest, std::string &signature) {
    EVP_MD_CTX *ctx;
    unsigned char *out = nullptr;
    unsigned int out_len;
    ctx = EVP_MD_CTX_new();
    EVP_MD_CTX_init(ctx);
    int ret = EVP_SignInit_ex(ctx, EVP_md5(), nullptr);
    if(ret != 1) {
        LOG(ERROR) << "Fail to EVP_SignInit_ex";
        EVP_MD_CTX_free(ctx);
        return false;
    }
    ret = EVP_SignUpdate(ctx, digest.c_str(), digest.size());
    if(ret != 1) {
        LOG(ERROR) << "Fail to EVP_SignUpdate";
        EVP_MD_CTX_free(ctx);
        return false;
    }
    EVP_SignFinal(ctx, nullptr, &out_len, _pkey);
    if(out_len <= 0) {
        LOG(ERROR) << "EVP_SignFinal return invalid signature length";
        EVP_MD_CTX_free(ctx);
        return false;
    }
    out = static_cast<unsigned char*>(OPENSSL_malloc(out_len));
    if(out == nullptr) {
        LOG(ERROR) << "no memory";
        EVP_MD_CTX_free(ctx);
        return false;
    }
    EVP_SignFinal(ctx, out, &out_len, _pkey);
    signature.append(reinterpret_cast<char*>(out), out_len);
    EVP_MD_CTX_free(ctx);
    return true;
}

bool Token::verify(const std::string &digest, const std::string &signature) {
    EVP_MD_CTX *ctx;
    ctx = EVP_MD_CTX_new();
    EVP_MD_CTX_init(ctx);
    int ret = EVP_VerifyInit_ex(ctx, EVP_md5(), nullptr);
    if(ret != 1) {
        LOG(ERROR) << "Fail to EVP_VerifyInit_ex";
        EVP_MD_CTX_free(ctx);
        return false;
    }
    ret = EVP_VerifyUpdate(ctx, digest.c_str(), digest.size());
    if(ret != 1) {
        LOG(ERROR) << "Fail to EVP_VerifyUpdate";
        EVP_MD_CTX_free(ctx);
        return false;
    }
    ret = EVP_VerifyFinal(ctx, (const unsigned char*)signature.c_str(), signature.size(), _pkey);
    if(ret != 1) {
        LOG(INFO) << "Fail to verify";
        EVP_MD_CTX_free(ctx);
        return false;
    }
    EVP_MD_CTX_free(ctx);
    return true;
}

}   // namespace YTalk