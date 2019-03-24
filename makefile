ARGS= -std=c++17 -masm=intel -m32
ARGSDEF= $(ARGS) -O2
ARGSFAST= $(ARGS) -Ofast -msse3
LINKARGS= $(ARGS) -Wl,--exclude-all-symbols -s

build:
	mkdir -p bin
	
	g++ -c hooks/hooker.cpp $(ARGSDEF) -o bin/hooks__hooker.o
	
	g++ -c hooks/incoming_packets.cpp $(ARGS) -o bin/hooks__incoming_packets.o
	