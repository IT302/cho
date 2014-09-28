AOCL_COMPILE_CONFIG=$(shell aocl compile-config)
AOCL_LINK_CONFIG=$(shell aocl link-config)

VPATH = ../util

host : util.o aocl_util.o host.o  
	g++ -o host host.o util.o aocl_util.o $(AOCL_LINK_CONFIG)

host.o : host.cpp
	g++ -c -I../util -std=c++0x host.cpp $(AOCL_COMPILE_CONFIG)
util.o :Util.cpp
	g++ -c  -o $@ ../util/Util.cpp -std=c++0x $(AOCL_COMPILE_CONFIG)
aocl_util.o : AOCL_Utils.cpp
	g++ -c -o $@ ../util/AOCL_Utils.cpp $(AOCL_COMPILE_CONFIG)


clean :
	rm -rf *.o host

