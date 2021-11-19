/*================================================================================   
 *    Date: 2021-11-19
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
        if((vaild() > len)) {
            ::memcpy(_cur, msg, len);
            _cur += len;
        }
    }

    void bzero() {
        ::bzero(_data, sizeof _data);
    }

    int vaild() const {
        return static_cast<int>(end() - _cur);
    }

    int size() const {
        return static_cast<int>(_cur - _data);
    }

    const char* data() const {
        return _data;
    }

    void reset() {
        _cur = _data;
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