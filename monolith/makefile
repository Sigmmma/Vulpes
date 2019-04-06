build:
	rm -f monolith.o
	gcc -c monolith.c -m32 -fno-stack-protector -o monolith.o
	gcc monolith.o -shared -m32 -nostdlib -static-libgcc -lkernel32 -lshlwapi -luser32 -o strings.dll