BRPC_PATH=../../thirdparty
CC=gcc
CXX=g++
CPPFLAGS=-DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google
STATIC_LINKINGS= -lgflags -lprotobuf
DYNAMIC_LINKINGS=-lpthread -lssl -lcrypto -ldl -lz -lrt -lleveldb
CXXFLAGS+=$(CPPFLAGS) -std=c++0x -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer

HDRS=/usr/include/
LIBS=/usr/lib/x86_64-linux-gnu
HDRS+=$(BRPC_PATH)/brpc/include
LIBS+=$(BRPC_PATH)/brpc/lib

HDRPATHS=$(addprefix -I, $(HDRS))
LIBPATHS=$(addprefix -L, $(LIBS))

MAIN_SOURCES = main.cpp

MAIN_OBJS = $(addsuffix .o, $(basename $(MAIN_SOURCES)))

STATIC_LINKINGS += -lbrpc

LINK_OPTIONS = -Xlinker "-(" $^ -Wl,-Bstatic $(STATIC_LINKINGS) -Wl,-Bdynamic -Xlinker "-)" $(DYNAMIC_LINKINGS)

test:$(MAIN_OBJS)
	$(CXX) $(LIBPATHS) $(LINK_OPTIONS) -o $@

main.o:main.cpp
	$(CXX) -c $(HDRPATHS) $(CXXFLAGS) $< -o $@


clean:
	rm *.o