#include "../Base64.h"
#include <iostream>

int main()
{
    std::string plaintext1 = "Man";
    std::string plaintext2 = "M";

    std::string ciphertext1, ciphertext2;
    YTalk::Base64::encode(plaintext1, ciphertext1);
    YTalk::Base64::encode(plaintext2, ciphertext2);
std::string plaintext3, plaintext4;
    YTalk::Base64::decode(ciphertext1, plaintext3);
    YTalk::Base64::decode(ciphertext2, plaintext4);

    std::cout << ciphertext1 << "\n" << ciphertext2 << "\n";
    std::cout << plaintext3 << "\n" << plaintext4 << "\n";
    return 0;
}