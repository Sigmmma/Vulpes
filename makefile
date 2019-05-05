WARNS= -pedantic -Wunused-value
ARGS= $(WARNS) -std=c++17 -masm=intel -msse2 -m32
ARGSDEF= $(ARGS) -O2
LINKARGS= -m32 -W
#,--exclude-all-symbols -s

build:
	mkdir -p bin
	rm -f bin/*.o
	rm -f bin/vulpes.dll

	windres version.rc -o bin/version.o

	g++ -c dll_main.cpp $(ARGSDEF) -o bin/dll_main.o
	g++ -c vulpes.cpp $(ARGSDEF) -o bin/vulpes.o
	g++ -c popout_console/guicon.cpp -m32 -O2 -o bin/popout_console__guicon.o

	g++ -c command/handler.cpp $(ARGSDEF) -o bin/command__handler.o
	g++ -c command/test.cpp $(ARGSDEF) -o bin/command__test.o

	g++ -c hooks/hooker.cpp $(ARGSDEF) -o bin/hooks__hooker.o
	g++ -c hooks/incoming_packets.cpp $(ARGS) -o bin/hooks__incoming_packets.o
	g++ -c hooks/console.cpp $(ARGS) -o bin/hooks__console.o

	g++ -c halo_functions/object_unit.cpp $(ARGS) -o bin/halo_functions__object_unit.o
	g++ -c halo_functions/console.cpp $(ARGS) -o bin/halo_functions__console.o
	g++ -c halo_functions/devmode.cpp $(ARGS) -o bin/halo_functions__devmode.o

	g++ -c halo_bug_fixes/cpu_usage.cpp $(ARGS) -o bin/halo_bug_fixes__cpu_usage.o
	g++ -c halo_bug_fixes/file_handle_leak.cpp $(ARGSDEF) -o bin/halo_bug_fixes__file_handle_leak.o
	g++ -c halo_bug_fixes/host_refusal.cpp $(ARGSDEF) -o bin/halo_bug_fixes__host_refusal.o
	g++ -c halo_bug_fixes/string_overflows.cpp $(ARGS) -o bin/halo_bug_fixes__string_overflows.o
	g++ -c halo_bug_fixes/shdr_trans_zfighting.cpp $(ARGSDEF) -o bin/halo_bug_fixes__shdr_trans_zfighting.o

	g++ -c memory/types.cpp $(ARGSDEF) -o bin/memory__types.o
	g++ -c memory/object.cpp $(ARGSDEF) -o bin/memory__object.o
	g++ -c memory/object_item.cpp $(ARGSDEF) -o bin/memory__object_item.o
	g++ -c memory/object_device.cpp $(ARGSDEF) -o bin/memory__object_device.o
	g++ -c memory/object_unit.cpp $(ARGSDEF) -o bin/memory__object_unit.o
	g++ -c memory/table.cpp $(ARGSDEF) -o bin/memory__table.o

	g++ -c network/network_globals.cpp $(ARGS) -o bin/network__network_globals.o

	g++ -c network/message_delta/bitstream.cpp $(ARGS) -o bin/network__message_delta__bitstream.o
	g++ -c network/message_delta/message_delta_processor.cpp $(ARGS) -o bin/network__message_delta__message_delta_processor.o
	g++ -c network/message_delta/definition.cpp $(ARGS) -o bin/network__message_delta__definition.o
	g++ -c network/message_delta/vulpes_message.cpp $(ARGS) -o bin/network__message_delta__vulpes_message.o
	g++ -c network/message_delta/message_delta_sender.cpp $(ARGS) -o bin/network__message_delta__message_delta_sender.o

	#ld -r -b binary binary/console_font.bin -o bin/console_font.o

	g++ bin/*.o -shared $(LINKARGS) -static-libgcc -lkernel32 -o "bin/Vulpes.dll"
