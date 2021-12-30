
CXX=g++
CC=gcc

BRPC_PATH=/root/YTalk/thirdparty
BASE_PATH=/root/YTalk/server
IM_SERVER_PATH=/root/YTalk/server
###ROUTE_SERVER_PATH=/root/YTalk/server
###DBPROXY_PATH=/root/YTalk/server
##3MYSQL_PATH=/usr/
###REDIS_PATH=/usr/local/
###LOGIN_SERVER_PATH=/root/YTalk/server

HDRS=$(BRPC_PATH)/brpc/include/
HDRS+=$(BASE_PATH)/
###HDRS+=$(DBPROXY_PATH)/DBProxyServer/
###HDRS+=$(LOGIN_SERVER_PATH)/LoginServer/
HDRS+=$(IM_SERVER_PATH)/IMServer/

##LIBS=$(MYSQL_PATH)/lib/x86_64-linux-gnu/
LIBS+=$(BRPC_PATH)/brpc/lib/
LIBS+=$(BASE_PATH)/base/
LIBS+=$(IM_SERVER_PATH)/IMServer/
##LIBS+=$(DBPROXY_PATH)/DBProxyServer/
##LIBS+=$(REDIS_PATH)/lib
##LIBS+=$(LOGIN_SERVER_PATH)/LoginServer/

HDRPATHS = $(addprefix -I, $(HDRS))
LIBPATHS = $(addprefix -L, $(LIBS))

CPPFLAGS=-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google
CXXFLAGS+=$(CPPFLAGS) -std=c++0x -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer

STATIC_LINKINGS= -lgflags -lprotobuf -lbase -lbrpc -lIMServer
DYNAMIC_LINKINGS=-lpthread -lssl -lcrypto -ldl -lz -lrt -lleveldb

LINK_OPTIONS = -Xlinker "-(" $^ -Wl,-Bstatic $(STATIC_LINKINGS) -Wl,-Bdynamic -Xlinker "-)" $(DYNAMIC_LINKINGS)

im_server : im_server.o 
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o im_server

im_server.o : Main.cpp 
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o im_server.o

clean:
	rm im_server.o 