/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "MySqlConn.h"
#include "MySqlPool.h"

namespace YTalk
{

MySqlConn::MySqlConn(MySqlPool *mySqlPool) {
    _mySqlPool = mySqlPool;
    _mysql = nullptr;
}

MySqlConn::~MySqlConn() {
    //TODO
}

int MySqlConn::init() {
    //TODO
    return 0;
}

}    // namesapce YTalk