all : access_mysql_test testForLogin testForFirstSend testForNotify testForServer testForRegister
.PHONY : all

BRPC_PATH=/root/YTalk/thirdparty
BASE_PATH=/root/YTalk/server
DBPROXY_PATH=/root/YTalk/server
LOGINSERVER_PATH=/root/YTalk/server
MYSQL_PATH=/usr/
REDIS_PATH=/usr/local/
RAPIDJSON_PATH=/root/YTalk/thirdparty

HDRS=$(MYSQL_PATH)/include/
HDRS+=$(BRPC_PATH)/brpc/include/
HDRS+=$(BASE_PATH)/
HDRS+=$(DBPROXY_PATH)/DBProxyServer/
HDRS+=$(REDIS_PATH)/include
HDRS+=$(LOGINSERVER_PATH)/LoginServer/
HDRS+=$(RAPIDJSON_PATH)/

LIBS=$(MYSQL_PATH)/lib/x86_64-linux-gnu/
LIBS+=$(BRPC_PATH)/brpc/lib/
LIBS+=$(BASE_PATH)/base/
LIBS+=$(DBPROXY_PATH)/DBProxyServer/
LIBS+=$(REDIS_PATH)/lib
LIBS+=$(LOGINSERVER_PATH)/LoginServer/

HDRPATHS = $(addprefix -I, $(HDRS))
LIBPATHS = $(addprefix -L, $(LIBS))

CPPFLAGS=-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google
CXXFLAGS+=$(CPPFLAGS) -std=c++0x -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer

STATIC_LINKINGS= -lgflags -lprotobuf -lbase -lbrpc -lmysqlclient -lDBProxy -lhiredis -lLoginServer
DYNAMIC_LINKINGS=-lpthread -lssl -lcrypto -ldl -lz -lrt -lleveldb

LINK_OPTIONS = -Xlinker "-(" $^ -Wl,-Bstatic $(STATIC_LINKINGS) -Wl,-Bdynamic -Xlinker "-)" $(DYNAMIC_LINKINGS)

access_mysql_test:access_mysql_test.o
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o access_mysql_test

access_mysql_test.o:access_mysql_test.cpp
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o access_mysql_test.o

testForLogin:testForLogin.o  
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o testForLogin

testForLogin.o:testForLogin.cpp 
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o testForLogin.o

testForFirstSend:testForFirstSend.o  
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o testForFirstSend

testForFirstSend.o:testForFirstSend.cpp 
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o testForFirstSend.o

testForNotify:testForNotify.o  
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o testForNotify

testForNotify.o:testForNotify.cpp 
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o testForNotify.o

testForServer:testForServer.o  
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o testForServer

testForServer.o:testForServer.cpp 
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o testForServer.o

testForRegister:testForRegister.o  
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o testForRegister

testForRegister.o:testForRegister.cpp 
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o testForRegister.o


clean:
	rm *.o