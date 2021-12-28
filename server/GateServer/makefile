
CXX=g++
CC=gcc

BRPC_PATH=/root/YTalk/thirdparty
BASE_PATH=/root/YTalk/server
GATE_SERVER_PATH=/root/YTalk/server
###DBPROXY_PATH=/root/YTalk/server
##3MYSQL_PATH=/usr/
###REDIS_PATH=/usr/local/
###LOGIN_SERVER_PATH=/root/YTalk/server

HDRS=$(BRPC_PATH)/brpc/include/
HDRS+=$(BASE_PATH)/
###HDRS+=$(DBPROXY_PATH)/DBProxyServer/
###HDRS+=$(LOGIN_SERVER_PATH)/LoginServer/
HDRS+=$(GATE_SERVER_PATH)/GateServer/

##LIBS=$(MYSQL_PATH)/lib/x86_64-linux-gnu/
LIBS+=$(BRPC_PATH)/brpc/lib/
LIBS+=$(BASE_PATH)/base/
LIBS+=$(GATE_SERVER_PATH)/GateServer/
##LIBS+=$(DBPROXY_PATH)/DBProxyServer/
##LIBS+=$(REDIS_PATH)/lib
##LIBS+=$(LOGIN_SERVER_PATH)/LoginServer/

HDRPATHS = $(addprefix -I, $(HDRS))
LIBPATHS = $(addprefix -L, $(LIBS))

CPPFLAGS=-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google
CXXFLAGS+=$(CPPFLAGS) -std=c++0x -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer

STATIC_LINKINGS= -lgflags -lprotobuf -lbase -lbrpc -lGateServer
DYNAMIC_LINKINGS=-lpthread -lssl -lcrypto -ldl -lz -lrt -lleveldb

LINK_OPTIONS = -Xlinker "-(" $^ -Wl,-Bstatic $(STATIC_LINKINGS) -Wl,-Bdynamic -Xlinker "-)" $(DYNAMIC_LINKINGS)

gate_server : gate_server.o 
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o gate_server

gate_server.o : Main.cpp 
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o gate_server.o

clean:
	rm gate_server.o 