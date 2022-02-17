/**
 * @file Base64.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_BASE_64_H
#define YTALK_BASE_64_H

#include <string>
#include <map>
#include <cstring>

namespace YTalk
{

namespace base
{


class Base64
{
private:
    Base64() = delete;
    ~Base64() = delete;

public:
    static void decode(const std::string &ciphertext, std::string &plaintext) {
        std::size_t counts = ciphertext.size() / 4 - 1;
        const char *str = ciphertext.c_str();
        char c_groups[3];
        char ui_groups[4];
        while(counts) {
            bcopy(str, ui_groups, 4);
            bzero(c_groups, 3);

            bcopy(&base64Map[ui_groups[0]], c_groups, 1);
            c_groups[0] = c_groups[0] << 2;
            //c_groups[0] = c_groups[0] | (ui_groups[1] >> 4);
            c_groups[0] = c_groups[0] | (base64Map[ui_groups[1]] >> 4);

            //4+2
            //bcopy(ui_groups+1, c_groups+1, 1);
            bcopy(&base64Map[ui_groups[1]], c_groups+1, 1);
            c_groups[1] = c_groups[1] << 4;
            //c_groups[1] = c_groups[1] | (ui_groups[2] >> 2);
            c_groups[1] = c_groups[1] | (base64Map[ui_groups[2]] >> 2);

            //4+2
            //bcopy(ui_groups+2, c_groups+2, 1);
            bcopy(&base64Map[ui_groups[2]], c_groups+2, 1);
            c_groups[2] = c_groups[2] << 6;
            //c_groups[2] = c_groups[2] | ui_groups[3];
            c_groups[2] = c_groups[2] | base64Map[ui_groups[3]];

            for(auto k : c_groups) {
                plaintext.push_back(k);
            }
            
            --counts;
            str += 4;
        }

        if(*(str+2) == '=' && *(str+3) == '=') {
            //bcopy(str, c_groups, 1);
            bcopy(&base64Map[*str], c_groups, 1);
            c_groups[0] = c_groups[0] << 2;
            //c_groups[0] = c_groups[0] | ((*(str+1)) >> 4);
            c_groups[0] = c_groups[0] | base64Map[*(str+1)] >> 4;
            plaintext.push_back(c_groups[0]);
        }
        else if(*(str+3) == '=') {
            //bcopy(str, c_groups, 1);
            bcopy(&base64Map[*str], c_groups, 1);
            c_groups[0] = c_groups[0] << 2;
            //c_groups[0] = c_groups[0] | ((*(str+1)) >> 4);
            c_groups[0] = c_groups[0] | base64Map[*(str+1)] >> 4;

            bcopy(str+1, c_groups+1, 1);
            c_groups[1] = c_groups[1] << 4;
            //c_groups[1] = c_groups[1] | ((*(str+2)) >> 2);
            c_groups[1] = c_groups[1] | base64Map[*(str+2)] >> 2;

            plaintext.push_back(c_groups[0]);
            plaintext.push_back(c_groups[1]);
        }
        else {
            bcopy(str, ui_groups, 4);
            bzero(c_groups, 3);

            bcopy(&base64Map[ui_groups[0]], c_groups, 1);
            c_groups[0] = c_groups[0] << 2;
            //c_groups[0] = c_groups[0] | (ui_groups[1] >> 4);
            c_groups[0] = c_groups[0] | (base64Map[ui_groups[1]] >> 4);

            //4+2
            //bcopy(ui_groups+1, c_groups+1, 1);
            bcopy(&base64Map[ui_groups[1]], c_groups+1, 1);
            c_groups[1] = c_groups[1] << 4;
            //c_groups[1] = c_groups[1] | (ui_groups[2] >> 2);
            c_groups[1] = c_groups[1] | (base64Map[ui_groups[2]] >> 2);

            //4+2
            //bcopy(ui_groups+2, c_groups+2, 1);
            bcopy(&base64Map[ui_groups[2]], c_groups+2, 1);
            c_groups[2] = c_groups[2] << 6;
            //c_groups[2] = c_groups[2] | ui_groups[3];
            c_groups[2] = c_groups[2] | base64Map[ui_groups[3]];

            for(auto k : c_groups) {
                plaintext.push_back(k);
            }
        }
    }

    static void encode(const std::string &plaintext, std::string &ciphertext) {
        const char *str = plaintext.c_str();
        std::size_t counts = plaintext.size() / 3;
        std::size_t n = plaintext.size() % 3;

        char c_groups[3];
        char ui_groups[4];
        while(counts) {
            bcopy(str, c_groups, 3);
            bzero(ui_groups, 4);

            bcopy(c_groups, ui_groups, 1);
            ui_groups[0] = ui_groups[0] >> 2;

            //2+4
            bcopy(c_groups+1, ui_groups+1, 1);
            ui_groups[1] = ui_groups[1] >> 4;
            c_groups[0] = c_groups[0] << 6;
            ui_groups[1] = ui_groups[1] | ((c_groups[0]) >> 2);

            //4+2
            bcopy(c_groups+1, ui_groups+2, 1);
            ui_groups[2] = ui_groups[2] << 4;
            ui_groups[2] = (ui_groups[2]) >> 2;
            ui_groups[2] = ui_groups[2] | (c_groups[2] >> 6);

            bcopy(c_groups+2, ui_groups+3, 1);
            ui_groups[3] = ui_groups[3] << 2;
            ui_groups[3] = (ui_groups[3]) >> 2;

            str += 3;
            --counts;

            for(auto k : ui_groups) {
                ciphertext.push_back(base64[k]);
            }
        }

        bzero(ui_groups, 4);
        if(n == 1) {
            bcopy(str, ui_groups, 1);
            int k = (ui_groups[0] >> 2);
            ciphertext.push_back(base64[k]);

            ui_groups[0] = (ui_groups[0] << 6);
            k = (ui_groups[0] >> 2);
            ciphertext.push_back(base64[k]);

            ciphertext += "==";
        }
        else if(n == 2) {
            bcopy(str, ui_groups, 1);
            std::uint8_t k = (ui_groups[0] >> 2);
            ciphertext.push_back(base64[k]);

            bcopy(str+1, ui_groups+1, 1);
            ui_groups[0] = ui_groups[0] << 6;
            k = ((ui_groups[0]) >> 2) | (ui_groups[1] >> 4);
            ciphertext.push_back(base64[k]);

            ui_groups[1] = ui_groups[1] << 4;
            k = ((ui_groups[1]) >> 2);
            ciphertext.push_back(base64[k]);
            
            ciphertext.push_back('=');
        }
    }

private:
    static const char base64[64];

    static std::map<char, std::uint8_t> base64Map;

    /*static constexpr char base64[64] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
    };*/

};     //// class Base64

const char Base64::base64[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

std::map<char, std::uint8_t> Base64::base64Map = {
    {'A', 0}, {'B', 1}, {'C', 2}, {'D', 3}, {'E', 4}, {'F', 5},
    {'G', 6}, {'H', 7}, {'I', 8}, {'J', 9}, {'K', 10}, {'L', 11},
    {'M', 12}, {'N', 13}, {'O', 14}, {'P', 15}, {'Q', 16}, {'R', 17},
    {'S', 18}, {'T', 19}, {'U', 20}, {'V', 21}, {'W', 22}, {'X', 23},
    {'Y', 24}, {'Z', 25},
    {'a', 26}, {'b', 27}, {'c', 28}, {'d', 29}, {'e', 30}, {'f', 31},
    {'g', 32}, {'h', 33}, {'i', 34}, {'j', 35}, {'k', 36}, {'l', 37},
    {'m', 38}, {'n', 39}, {'o', 40}, {'p', 41}, {'q', 42}, {'r', 43},
    {'s', 44}, {'t', 45}, {'u', 46}, {'v', 47}, {'w', 48}, {'x', 49},
    {'y', 50}, {'z', 51}, 
    {'0', 52}, {'1', 53}, {'2', 54}, {'3', 55}, {'4', 56}, {'5', 57}, 
    {'6', 58}, {'7', 59}, {'8', 60}, {'9', 61}, 
    {'+', 62}, {'/', 63}
};

}  // namespace base

}   // namespace YTalk

#endif /// YTALK_BASE_64_H