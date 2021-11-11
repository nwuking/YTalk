/*================================================================================
 *    Date:
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "./DB.h"

#include <butil/logging.h>

namespace YTalk 
{

DBConn::DBConn() {
    _mysql = nullptr;
}
DBConn::~DBConn() {
    //
}

int DBConn::init() {
    //TODO
}

DBResultSet* DBConn::executeQuery(const char *query) {
    //TODO
}

}    // namespace YTalk