cmake_minimum_required(VERSION 2.8.10)
project(YTalk_Login_Server)

## for include
set(BASE_INCLUDE ${PROJECT_SOURCE_DIR}/../)
set(DBPROXY_INCLUDE ${PROJECT_SOURCE_DIR}/../)
set(PROTO_INCLUDE ${PROJECT_SOURCE_DIR}/)
set(BRPC_INCLUDE ${PROJECT_SOURCE_DIR}/../../thirdparty/brpc/include/)
set(OPENSSL_INCLUDE ${PROJECT_SOURCE_DIR}/../../thirdparty/openssl/include/)
##set(MYSQL_INCLUDE /usr/include/)
###set(REDIS_INCLUDE /usr/local/include/)
set(RAPIDJSON_INCLUDE ${PROJECT_SOURCE_DIR}/../../thirdparty/)
set(LOGINSERVER_INCLUDE ${PROJECT_SOURCE_DIR}/)

set(SOURCES 
    AccessMySql.cpp 
    AccessRedis.cpp 
    LoginService.cpp  
    AuthService.cpp 
    Session.cpp 
    Channel.cpp 
    Token.cpp 
    protobuf/login.pb.cc  
    protobuf/auth.pb.cc  
    ${DBPROXY_INCLUDE}/DBProxyServer/protobuf/mysql.pb.cc 
    ${DBPROXY_INCLUDE}/DBProxyServer/protobuf/redis.pb.cc 
    )


## for CMAKE_CXX_FLAGS
set(CMAKE_CXX_FLAGS "-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google -std=c++11 -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

## generate static-lib 
add_library(LoginServer-static STATIC ${SOURCES})
target_include_directories(LoginServer-static 
                    PUBLIC ${BASE_INCLUDE} 
                           ${DBPROXY_INCLUDE} 
                           ${BRPC_INCLUDE} 
                           ${OPENSSL_INCLUDE} 
                           ${PROTO_INCLUDE} 
                           ${RAPIDJSON_INCLUDE}
                           ${LOGINSERVER_INCLUDE}
                           )
set_target_properties(LoginServer-static PROPERTIES OUTPUT_NAME LoginServer CLEAN_DIRECT_OUTPUT 1)
