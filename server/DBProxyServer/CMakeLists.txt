cmake_minimum_required(VERSION 2.8.10)
project(YTalk_DBProxy_Server)

set(SOURCES 
    MySqlConn.cpp 
    MySqlPool.cpp 
    MySqlService.cpp 
    protobuf/mysql.pb.cc )

## for *.a 
set(STATIC_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib/)

## for include
set(BASE_INCLUDE ${PROJECT_SOURCE_DIR}/../)
set(DBPROXY_INCLUDE ${PROJECT_SOURCE_DIR}/)
set(PROTO_INCLUDE ${PROJECT_SOURCE_DIR}/)
set(BRPC_INCLUDE ${PROJECT_SOURCE_DIR}/../../thirdparty/brpc/include/)
set(MYSQL_INCLUDE /usr/include/)

## for lib
set(BASE_LIB ${PROJECT_SOURCE_DIR}/../base/output/lib/)
set(BRPC_LIB ${PROJECT_SOURCE_DIR}/../../thirdparty/brpc/lib/)
set(DBPROXY_LIB ${PROJECT_SOURCE_DIR}/output/lib/)
set(MYSQL_LIB /usr/lib/x86_64-linux-gnu/)

## for CMAKE_CXX_FLAGS
set(CMAKE_CXX_FLAGS "-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google -std=c++11 -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

## generate static-lib 
add_library(DBProxy-static STATIC ${SOURCES})
target_include_directories(DBProxy-static 
                    PUBLIC ${BASE_INCLUDE} 
                           ${DBPROXY_INCLUDE} 
                           ${MYSQL_INCLUDE} 
                           ${BRPC_INCLUDE} 
                           ${PROTO_INCLUDE})
set_target_properties(DBProxy-static PROPERTIES OUTPUT_NAME DBProxy CLEAN_DIRECT_OUTPUT 1)

## install static-lib
install(TARGETS DBProxy-static 
        ARCHIVE DESTINATION ${STATIC_LIBRARY_OUTPUT_DIRECTORY})

##TODO

