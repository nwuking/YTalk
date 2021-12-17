cmake_minimum_required(VERSION 2.8.10)
project(YTalk_Login_Server)

set(SOURCES 
    AccessMySql.cpp 
    )

## for include
set(BASE_INCLUDE ${PROJECT_SOURCE_DIR}/../)
set(DBPROXY_INCLUDE ${PROJECT_SOURCE_DIR}/../)
set(PROTO_INCLUDE ${PROJECT_SOURCE_DIR}/)
set(BRPC_INCLUDE ${PROJECT_SOURCE_DIR}/../../thirdparty/brpc/include/)
set(MYSQL_INCLUDE /usr/include/)
set(REDIS_INCLUDE /usr/local/include/)
set(RAPIDJSON_INCLUDE ${PROJECT_SOURCE_DIR}/../../thirdparty/)
set(LOGINSERVER_INCLUDE ${PROJECT_SOURCE_DIR}/)


## for CMAKE_CXX_FLAGS
set(CMAKE_CXX_FLAGS "-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google -std=c++11 -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

## generate static-lib 
add_library(LoginServer-static STATIC ${SOURCES})
target_include_directories(LoginServer-static 
                    PUBLIC ${BASE_INCLUDE} 
                           ${DBPROXY_INCLUDE} 
                           ${MYSQL_INCLUDE} 
                           ${BRPC_INCLUDE} 
                           ${PROTO_INCLUDE} 
                           ${REDIS_INCLUDE} 
                           ${RAPIDJSON_INCLUDE}
                           ${LOGINSERVER_INCLUDE})
set_target_properties(LoginServer-static PROPERTIES OUTPUT_NAME LoginServer CLEAN_DIRECT_OUTPUT 1)
