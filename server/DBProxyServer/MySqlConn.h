/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <mysql/mysql.h>

namespace YTalk
{

class MySqlPool;

class MySqlConn
{
public:
    MySqlConn(MySqlPool *mysqlPool);
    virtual ~MySqlConn();

    int init();
    //TODO

private:
    MySqlPool *_mySqlPool;
    MYSQL *_mysql;
};    // class MySqlConn

}    // namespace YTalk