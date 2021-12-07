/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <string>
#include <list>

namespace YTalk
{

class MySqlConn;

class MySqlPool
{
public:
    MySqlPool(const std::string &pool_name, const std::string &db_host, uint16_t db_port,
        const std::string &db_username, const std::string &db_password, const std::string &db_name, int maxconncnt);
    virtual ~MySqlPool();

    int init();
    //TODO
private:
    std::string _pool_name;
    std::string _db_host;
    uint16_t _db_port;
    std::string _db_username;
    std::string _db_password;
    std::string _db_name;
    int _db_maxconncnt;
    int _db_curconncnt;
    std::list<MySqlConn*> _free_list;
};    // class MysqlPool

}    // namespace YTalk