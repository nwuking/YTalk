
CXX=g++
CC=gcc

BRPC_PATH=/root/YTalk/thirdparty
OPENSSL_PATH=/root/YTalk/thirdparty
BASE_PATH=/root/YTalk/server
DBPROXY_PATH=/root/YTalk/server
LOGIN_SERVER_PATH=/root/YTalk/server
OPENSSL_PATH=/root/YTalk/thirdparty

HDRS=$(BRPC_PATH)/brpc/include/
HDRS+=$(BASE_PATH)/
HDRS+=$(DBPROXY_PATH)/DBProxyServer/
HDRS+=$(LOGIN_SERVER_PATH)/LoginServer/
HDRS+=$(OPENSSL_PATH)/openssl/include/

LIBS+=$(BRPC_PATH)/brpc/lib/
LIBS+=$(BASE_PATH)/base/
LIBS+=$(DBPROXY_PATH)/DBProxyServer/
LIBS+=$(LOGIN_SERVER_PATH)/LoginServer/
LIBS+=$(OPENSSL_PATH)/openssl/lib/

HDRPATHS = $(addprefix -I, $(HDRS))
LIBPATHS = $(addprefix -L, $(LIBS))

CPPFLAGS=-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google
CXXFLAGS+=$(CPPFLAGS) -std=c++0x -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer

STATIC_LINKINGS= -lgflags -lprotobuf -lbase -lbrpc -lDBProxy -lLoginServer -lcrypto
DYNAMIC_LINKINGS=-lpthread -lssl -lcrypto -ldl -lz -lrt -lleveldb

LINK_OPTIONS = -Xlinker "-(" $^ -Wl,-Bstatic $(STATIC_LINKINGS) -Wl,-Bdynamic -Xlinker "-)" $(DYNAMIC_LINKINGS)

login_server : login_server.o 
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o login_server

login_server.o : Main.cpp 
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o login_server.o



clean:
	rm login_server.o 