AOCL_COMPILE_CONFIG=$(shell aocl compile-config)
AOCL_LINK_CONFIG=$(shell aocl link-config)

VPATH = ../util

host_fpga : util.o aocl_util.o host_fpga.o  
	g++ -o host_fpga host_fpga.o util.o aocl_util.o $(AOCL_LINK_CONFIG)

host_fpga.o : host_fpga.cpp
	g++ -c -I../util -std=c++0x host_fpga.cpp $(AOCL_COMPILE_CONFIG)
util.o :Util.cpp
	g++ -c  -o $@ ../util/Util.cpp -std=c++0x $(AOCL_COMPILE_CONFIG)
aocl_util.o : AOCL_Utils.cpp
	g++ -c -o $@ ../util/AOCL_Utils.cpp $(AOCL_COMPILE_CONFIG)


clean :
	rm -rf *.o host_fpga

