/*================================================================================   
 *    Date:
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <string>

namespace YTalk
{

class LogFile
{
public:
    LogFile(const int &flush, const off_t &roll, const std::string &path);
    ~LogFile();

private:

};    // class LogFile

}    // namespace YTalk