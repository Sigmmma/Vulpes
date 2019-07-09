LINKARGS= -m32 -W -s

build:
	mkdir -p bin
	rm -f bin/*.o
	rm -f bin/VulpesLoader.dll

	windres version.rc -o bin/version.o
	gcc -c dll_main.c -O3 -m32 -o bin/dll_main.o

	gcc bin/*.o -shared $(LINKARGS) -static-libgcc -lkernel32 -o "bin/VulpesLoader.dll"
