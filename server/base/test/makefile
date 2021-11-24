all : time_test conf_test log_test
.PHONY : all


BRPC_PATH=/root/YTalk/thirdparty
BASE_PATH=/root/YTalk/server
CC=gcc
CXX=g++
CPPFLAGS=-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google
STATIC_LINKINGS= -lgflags -lprotobuf -lbase
DYNAMIC_LINKINGS=-lpthread -lssl -lcrypto -ldl -lz -lrt -lleveldb
CXXFLAGS+=$(CPPFLAGS) -std=c++0x -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer

HDRS=/usr/include/
LIBS=/usr/lib/x86_64-linux-gnu
HDRS+=$(BRPC_PATH)/brpc/include
LIBS+=$(BRPC_PATH)/brpc/lib
HDRS+=$(BASE_PATH)
LIBS+=$(BASE_PATH)/base

HDRPATHS=$(addprefix -I, $(HDRS))
LIBPATHS=$(addprefix -L, $(LIBS))

TIME_SOURCES = time.cpp

TIME_OBJS = $(addsuffix .o, $(basename $(TIME_SOURCES)))

STATIC_LINKINGS += -lbrpc

LINK_OPTIONS = -Xlinker "-(" $^ -Wl,-Bstatic $(STATIC_LINKINGS) -Wl,-Bdynamic -Xlinker "-)" $(DYNAMIC_LINKINGS)

time_test:$(TIME_OBJS)
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o time_test

$(TIME_OBJS):time.cpp
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o time.o

##########################
conf_test:config.o   
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o conf_test

config.o:configParse.cpp  
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o config.o

############################################
log_test:log.o   
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o $@

log.o:log.cpp  
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o $@


clean:
	rm *.o