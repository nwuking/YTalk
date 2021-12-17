cmake_minimum_required(VERSION 2.8.10)
project(YTalk_BASE CXX)

# for *.a
set(STATIC_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)

set(BASE_SOURCES 
    AsyncLog.cpp 
    LogFile.cpp 
    Logging.cpp 
    Time.cpp 
    ConfigParse.cpp )

## for include
set(BASE_INCLUDE ${PROJECT_SOURCE_DIR}/)
set(BRPC_INCLUDE ${PROJECT_SOURCE_DIR}/../../thirdparty/brpc/include/)

set(CMAKE_CXX_FLAGS "-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google -std=c++11 -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

#Generate the static library and the dynamic library
add_library(base-static STATIC ${BASE_SOURCES})
target_include_directories(base-static PUBLIC ${BASE_INCLUDE} ${BRPC_INCLUDE})

# rename
set_target_properties(base-static PROPERTIES OUTPUT_NAME base CLEAN_DIRECT_OUTPUT 1) 