all : mysql_test redis_test
.PHONY : all

CXX=g++
CC=gcc

BRPC_PATH=/root/YTalk/thirdparty
BASE_PATH=/root/YTalk/server
DBPROXY_PATH=/root/YTalk/server
MYSQL_PATH=/usr/
REDIS_PATH=/usr/local/

HDRS=$(MYSQL_PATH)/include/
HDRS+=$(BRPC_PATH)/brpc/include/
HDRS+=$(BASE_PATH)/
HDRS+=$(DBPROXY_PATH)/DBProxyServer/
HDRS+=$(REDIS_PATH)/include

LIBS=$(MYSQL_PATH)/lib/x86_64-linux-gnu/
LIBS+=$(BRPC_PATH)/brpc/lib/
LIBS+=$(BASE_PATH)/base/
LIBS+=$(DBPROXY_PATH)/DBProxyServer/
LIBS+=$(REDIS_PATH)/lib

HDRPATHS = $(addprefix -I, $(HDRS))
LIBPATHS = $(addprefix -L, $(LIBS))

CPPFLAGS=-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google
CXXFLAGS+=$(CPPFLAGS) -std=c++0x -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer

STATIC_LINKINGS= -lgflags -lprotobuf -lbase -lbrpc -lmysqlclient -lDBProxy -lhiredis
DYNAMIC_LINKINGS=-lpthread -lssl -lcrypto -ldl -lz -lrt -lleveldb

LINK_OPTIONS = -Xlinker "-(" $^ -Wl,-Bstatic $(STATIC_LINKINGS) -Wl,-Bdynamic -Xlinker "-)" $(DYNAMIC_LINKINGS)


mysql_test : mysql_test.o 
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o mysql_test

mysql_test.o : mysql_test.cpp 
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o mysql_test.o 


redis_test : redis_test.o 
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o redis_test

redis_test.o : redis_test.cpp 
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o redis_test.o


clean:
	rm *.o 