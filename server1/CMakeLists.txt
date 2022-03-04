cmake_minimum_required(VERSION 2.6)

project (YTalk)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -O0 -Wno-unused-variable -pthread")

link_directories(
    ${PROJECT_SOURCE_DIR}/lib
    /usr/lib/x86_64-linux-gnu/
)

find_library(MYSQL_LIB libmysqlclient.so /usr/lib/x86_64-linux-gnu/)
IF (NOT MYSQL_LIB)
    MESSAGE(FATAL_ERROR "mysqlclient not found. If your mysql is mariadb, please install mariadb-devel otherwise install mysql-devel.")
ENDIF(NOT MYSQL_LIB)

set(net_srcs 
base/ConfigParse.cpp 
base/LogService.cpp 
base/ThreadUtils.cpp 
base/TimeStamp.cpp 
base/BinaryStream.cpp 

netlib/Acceptor.cpp 
netlib/Buffer.cpp 
netlib/Channel.cpp 
netlib/Connector.cpp 
netlib/EPoller.cpp 
netlib/EventLoop.cpp 
netlib/EventLoopThread.cpp 
netlib/EventLoopThreadPool.cpp
netlib/InetAddress.cpp 
netlib/Socket.cpp 
netlib/SocketsOps.cpp 
netlib/TcpClient.cpp 
netlib/TcpConnection.cpp 
netlib/TcpServer.cpp 
netlib/Timer.cpp 
netlib/TimerQueue.cpp 
)

set(mysql_srcs
mysql/MysqlConn.cpp 
mysql/MysqlManager.cpp 
)

set(IMServer_srcs
IMServer/CacheManager.cpp 
IMServer/ChatService.cpp 
IMServer/ChatSession.cpp 
IMServer/Main.cpp 
IMServer/Session.cpp 
IMServer/UserManager.cpp 
)


add_executable(chatserver ${net_srcs} ${IMServer_srcs} ${mysql_srcs})
#光包含库目录是没用的，还必须使用TARGET_LINK_LIBRARIES链接该库
target_include_directories(chatserver PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/)
TARGET_LINK_LIBRARIES(chatserver mysqlclient)