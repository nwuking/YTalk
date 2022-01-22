/*================================================================================   
 *    Date: 2022-1-21
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "Signature.h"
#include "Logging.h"
#include "Key.h"


namespace YTalk
{

void Signature::sign(const std::string &digest, std::string &signature) {
    OSSL_LIB_CTX *libctx = nullptr;
    const char *propq = nullptr;
    const char *sig_name = "SHA3-512";
    //std::string sig_name = "SHA3-512";
    std::size_t sig_len;
    unsigned char *sig_value = nullptr;

    EVP_PKEY *pri_key = nullptr;
    EVP_MD_CTX *sig_context = nullptr;

    // Get private key
    pri_key = getKey(libctx, propq, PRIVATE_KEY);
    if(pri_key == nullptr) {
        LOG(ERROR) << "PKEY is nullptr, fail to load key";
    }

    // Make a message signature context to hold temporary state
    // during signature createion
    sig_context = EVP_MD_CTX_new();
    if(sig_context == nullptr) {
        LOG(ERROR) << "EVP_MD_CTX_new failed";
        goto cleanup;
    }

    // 初始化
    if(!EVP_DigestSignInit_ex(sig_context, nullptr, sig_name, libctx, nullptr, pri_key, nullptr)) {
        LOG(ERROR) << "WVP_DigestSignInit failed";
        goto cleanup;
    }

    // 签名
    if (!EVP_DigestSignUpdate(sig_context, digest.c_str(), digest.size())) {
        LOG(ERROR) << "EVP_DigestSignUpdate(digest) failed";
        goto cleanup;
    }

    if (!EVP_DigestSignFinal(sig_context, NULL, &sig_len)) {
        LOG(ERROR) << "EVP_DigestSignFinal failed.";
        goto cleanup;
    }
    if (sig_len <= 0) {
        LOG(ERROR) << "EVP_DigestSignFinal returned invalid signature length.";
        goto cleanup;
    }

    sig_value = static_cast<unsigned char*>(OPENSSL_malloc(sig_len));
    if (sig_value == NULL) {
        LOG(ERROR) << "No memory.";
        goto cleanup;
    }
    if (!EVP_DigestSignFinal(sig_context, sig_value, &sig_len)) {
        LOG(ERROR) << "EVP_DigestSignFinal failed.";
        goto cleanup;
    }

    //std::string new_sig_value(reinterpret_cast<char*>(sig_value), sig_len);
    signature.append(reinterpret_cast<char*>(sig_value), sig_len);
cleanup:
    OPENSSL_free(sig_value);
    EVP_PKEY_free(pri_key);
    EVP_MD_CTX_free(sig_context);
}

bool Signature::verify(const std::string &digest, const std::string &signature) {
    OSSL_LIB_CTX *libctx = nullptr;
    const char *propq = nullptr;
    EVP_MD_CTX *verify_context = nullptr;
    EVP_PKEY *pub_key = nullptr;

    const char *sig_name = "SHA3-512";
    bool result = true;

    /*
    * Make a verify signature context to hold temporary state
    * during signature verification
    */
    verify_context = EVP_MD_CTX_new();
    if (verify_context == NULL) {
        LOG(ERROR) << "EVP_MD_CTX_new failed.";
        result = false;
        goto cleanup;
    }
    /* Get public key */
    pub_key = getKey(libctx, propq, PUBLIC_KEY);
    if (pub_key == NULL) {
        LOG(ERROR) << "Get public key failed.";
        result = false;
        goto cleanup;
    }

    /* Verify */
    if (!EVP_DigestVerifyInit_ex(verify_context, NULL, sig_name,
                                libctx, NULL, pub_key, NULL)) {
        LOG(ERROR) << "EVP_DigestVerifyInit failed.";
        result = false;
        goto cleanup;
    }
    /*
    * EVP_DigestVerifyUpdate() can be called several times on the same context
    * to include additional data.
    */
    if (!EVP_DigestVerifyUpdate(verify_context, digest.c_str(), digest.size())) {
        LOG(ERROR) << "EVP_DigestVerifyUpdate(hamlet_1) failed.";
        result = false;
        goto cleanup;
    }
    //const char *ptr = signature.c_str();
    if (EVP_DigestVerifyFinal(verify_context, (const unsigned char*)signature.c_str(), signature.size()) <= 0) {
        LOG(ERROR) << "EVP_DigestVerifyFinal failed.";
        result = false;
        goto cleanup;
    }
        
cleanup:
    EVP_PKEY_free(pub_key);
    EVP_MD_CTX_free(verify_context);
    return result;
}

EVP_PKEY* Signature::getKey(OSSL_LIB_CTX *libctx, const char *propq, int flag) {
    OSSL_DECODER_CTX *dctx = nullptr;
    EVP_PKEY *pkey = nullptr;
    int selection;
    const unsigned char *data;
    size_t data_len;

    if(flag) {
        selection = EVP_PKEY_KEYPAIR;
        data = priv_key_der;
        data_len = sizeof(priv_key_der);
    }
    else {
        selection = EVP_PKEY_PUBLIC_KEY;
        data = pub_key_der;
        data_len = sizeof(pub_key_der);
    }

    dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, "DER", NULL, "EC",
                                         selection, libctx, propq);
    (void)OSSL_DECODER_from_data(dctx, &data, &data_len);
    OSSL_DECODER_CTX_free(dctx);

    return pkey;
}

}