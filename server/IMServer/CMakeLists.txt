cmake_minimum_required(VERSION 2.8.10)
project(YTalk_Route_Server)

## for include
set(BASE_INCLUDE ${PROJECT_SOURCE_DIR}/../)
set(DBPROXY_INCLUDE ${PROJECT_SOURCE_DIR}/../)
set(PROTO_INCLUDE ${PROJECT_SOURCE_DIR}/)
set(BRPC_INCLUDE ${PROJECT_SOURCE_DIR}/../../thirdparty/brpc/include/)
set(RAPIDJSON_INCLUDE ${PROJECT_SOURCE_DIR}/../../thirdparty/)
##set(MYSQL_INCLUDE /usr/include/)
##set(REDIS_INCLUDE /usr/local/include/)
###set(LOGINSERVER_INCLUDE ${PROJECT_SOURCE_DIR}/../)
set(ROUTESERVER_INCLUDE ${PROJECT_SOURCE_DIR}/../)
####set(GATESERVER_INCLUDE ${PROJECT_SOURCE_DIR}/../)
set(IMSERVER_INCLUDE ${PROJECT_SOURCE_DIR}/)

set(SOURCES 
    ConRoute_im.cpp 
    IMService.cpp 
    protobuf/im.pb.cc 
    ${ROUTESERVER_INCLUDE}/RouteServer/protobuf/route.pb.cc 
    ${DBPROXY_INCLUDE}/DBProxyServer/protobuf/mysql.pb.cc 
    ${DBPROXY_INCLUDE}/DBProxyServer/protobuf/redis.pb.cc 
    )


## for CMAKE_CXX_FLAGS
set(CMAKE_CXX_FLAGS "-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google -std=c++11 -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

## generate static-lib 
add_library(IMServer-static STATIC ${SOURCES})
target_include_directories(IMServer-static 
                    PUBLIC ${BASE_INCLUDE} 
                           ${BRPC_INCLUDE} 
                           ${PROTO_INCLUDE} 
                           ${RAPIDJSON_INCLUDE}
                           ${ROUTESERVER_INCLUDE}
                           ${IMSERVER_INCLUDE}
                           ${DBPROXY_INCLUDE})
set_target_properties(IMServer-static PROPERTIES OUTPUT_NAME IMServer CLEAN_DIRECT_OUTPUT 1)
