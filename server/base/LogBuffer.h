/*================================================================================   
 *    Date:
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <string>

namespace YTalk
{

const int ksmallBuffer = 4000;
const int klargeBuffer = 4000 * 1000;

template<int SIZE>
class LogBuffer
{
public:
    LogBuffer() : _cur(_data) {
        //
    }
    ~LogBuffer() {}

    void append(const char *msg, int len) {
        //TODO
    }

    void bzero() {
        ::bzero(_data, sizeof _data);
    }

    int vaild() const {
        return static_cast<int>(end() - _cur);
    }
    //TODO

private:
    const char* end() const {
        return _data + sizeof _data;
    }

    char *_cur;
    char _data[SIZE];                   // 

};    // class LogBuffer

}    // namespace YTalk