ARGS= -std=c++17 -masm=intel -m32
ARGSDEF= $(ARGS) -O2
ARGSFAST= $(ARGS) -Ofast -msse3
LINKARGS= -m32 -Wl,--exclude-all-symbols -s

build:
	mkdir -p bin
	rm -f bin/*.o
	rm -f bin/vulpes.dll
	#windres version.rc -o bin/version.o
	g++ -c dll_main.cpp $(ARGS) -o bin/dll_main.o
	g++ -c popout_console/guicon.cpp -m32 -o bin/popout_console__guicon.o
	
	g++ -c hooks/hooker.cpp $(ARGSDEF) -o bin/hooks__hooker.o
	
	#g++ -c hooks/incoming_packets.cpp $(ARGS) -o bin/hooks__incoming_packets.o
	
	g++ bin/*.o -shared -static-libgcc -lkernel32 -o "bin/vulpes.dll"
	