cmake_minimum_required(VERSION 2.8.10)
project(YTalk_BASE CXX)

# for *.so 
set(DYNAMIC_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)
# for *.a
set(STATIC_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)

set(BASE_SOURCES 
    AsyncLog.cpp 
    LogFile.cpp 
    Logging.cpp 
    Time.cpp)

set(BASE_INCLUDE ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google -std=c++11 -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

# To find the third_party directory
execute_process(
    COMMAND bash -c "find / -name thirdparty | grep YTalk"
    OUTPUT_VARIABLE THIRD_PARTY_INCLUDE
)
#set(BRPC_INCLUDE ${THIRD_PARTY_INCLUDE}/brpc/include)

set(BRPC_INCLUDE "/root/YTalk/thirdparty/brpc/include")

message("${BRPC_INCLUDE}")

#Generate the static library and the dynamic library
add_library(base-static STATIC ${BASE_SOURCES})
##add_library(base-shared SHARED ${BASE_SOURCES})
target_include_directories(base-static PUBLIC ${BASE_INCLUDE} ${BRPC_INCLUDE})
##target_include_directories(base-shared ${PROJECT_SOURCE_DIR})

# rename
set_target_properties(base-static PROPERTIES OUTPUT_NAME base CLEAN_DIRECT_OUTPUT 1) 

#Install the library
install(TARGETS base-static
        ARCHIVE DESTINATION ${STATIC_LIBRARY_OUTPUT_DIRECTORY}   
        )
