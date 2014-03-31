GEN_SRC := simpleTests.cpp test_constants.cpp test_types.cpp
GEN_OBJ := $(patsubst %.cpp,%.o, $(GEN_SRC))

THRIFT_DIR := /usr/local/include/thrift
BOOST_DIR := /usr/local/include

INC := -I$(THRIFT_DIR) -I$(BOOST_DIR)

.PHONY: all clean

all: server_NoCaching server_RandomCaching server_FIFOCaching server_MAXSCaching client

%.o: %.cpp
	$(CXX) -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H $(INC) -c $< -o $@

server_NoCaching: simpleTests_server.o $(GEN_OBJ)
	$(CXX) $^ -o $@ -L/usr/local/lib -lthrift -lcurl

server_RandomCaching: simpleTests_server_Random.o $(GEN_OBJ)
	$(CXX) $^ -o $@ -g -L/usr/local/lib -lthrift -lcurl

server_FIFOCaching: simpleTests_server_FIFO.o $(GEN_OBJ)
	$(CXX) $^ -o $@ -g -L/usr/local/lib -lthrift -lcurl

server_MAXSCaching: simpleTests_server_MAXS.o $(GEN_OBJ)
	$(CXX) $^ -o $@ -g -L/usr/local/lib -lthrift -lcurl

client: simpleTests_client.o $(GEN_OBJ)
	$(CXX) $^ -o $@ -L/usr/local/lib -lthrift 

clean:
	$(RM) *.o server_NoCaching server_RandomCaching server_FIFOCaching server_MAXSCaching client
