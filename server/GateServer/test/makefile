all : testForConLogin
.PHONY : all

BRPC_PATH=/root/YTalk/thirdparty
BASE_PATH=/root/YTalk/server
DBPROXY_PATH=/root/YTalk/server
LOGINSERVER_PATH=/root/YTalk/server
GATESERVER_PATH=/root/YTalk/server
MYSQL_PATH=/usr/
REDIS_PATH=/usr/local
RAPIDJSON_PATH=/root/YTalk/thirdparty

HDRS=$(MYSQL_PATH)/include/
HDRS+=$(BRPC_PATH)/brpc/include/
HDRS+=$(BASE_PATH)/
HDRS+=$(DBPROXY_PATH)/DBProxyServer/
HDRS+=$(REDIS_PATH)/include/
HDRS+=$(LOGINSERVER_PATH)/LoginServer/
HDRS+=$(RAPIDJSON_PATH)/
HDRS+=$(GATESERVER_PATH)/GateServer/

LIBS=$(LOGINSERVER_PATH)/LoginServer/
LIBS+=$(MYSQL_PATH)/lib/x86_64-linux-gnu/
LIBS+=$(BRPC_PATH)/brpc/lib/
LIBS+=$(BASE_PATH)/base/
LIBS+=$(DBPROXY_PATH)/DBProxyServer/
LIBS+=$(REDIS_PATH)/lib
LIBS+=$(GATESERVER_PATH)/GateServer/

HDRPATHS = $(addprefix -I, $(HDRS))
LIBPATHS = $(addprefix -L, $(LIBS))

CPPFLAGS=-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google
CXXFLAGS+=$(CPPFLAGS) -std=c++0x -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer

STATIC_LINKINGS= -lgflags -lprotobuf -lbase -lbrpc -lDBProxy -lGateServer -lLoginServer 
DYNAMIC_LINKINGS=-lpthread -lssl -lcrypto -ldl -lz -lrt -lleveldb

LINK_OPTIONS = -Xlinker "-(" $^ -Wl,-Bstatic $(STATIC_LINKINGS) -Wl,-Bdynamic -Xlinker "-)" $(DYNAMIC_LINKINGS)

testForConLogin:testForConLogin.o
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o testForConLogin

testForConLogin.o:testForConLogin.cpp
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o testForConLogin.o


clean:
	rm *.o