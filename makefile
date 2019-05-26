WARNS= -Wunused-value -Wold-style-cast
ARCH= -masm=intel -msse2 -m32 -mfpmath=sse
ARGS= $(WARNS) $(ARCH) -std=c++17
ARGSC= $(ARCH)
ARGSDEF= $(ARGS) -O2
ARGSFAST= $(ARGS) -O3
ARGSFASTC= $(ARGSC) -O3
ARGSFASTER= $(ARGS) -Ofast
LINKARGS= -m32 -W -s

build:
	mkdir -p bin
	rm -f bin/*.o
	rm -f bin/vulpes.dll

	windres version.rc -o bin/version.o
	g++ -c dll_main.cpp $(ARGSDEF) -o bin/dll_main.o
	g++ -c vulpes.cpp $(ARGSDEF) -o bin/vulpes.o

	g++ -c command/debug.cpp $(ARGSFAST) -o bin/command__debug.o
	g++ -c command/handler.cpp $(ARGSFAST) -o bin/command__handler.o

	g++ -c hooker/code_cave.cpp $(ARGSFAST) -o bin/hooks__hooker_code_cave.o
	g++ -c hooker/hooker.cpp $(ARGSFAST) -o bin/hooks__hooker.o

	g++ -c includes/guicon.cpp $(ARCH) -O3 -o bin/includes__guicon.o
	gcc -c includes/string_raw_data_encoder.c $(ARGSFASTC) -o bin/includes__string_raw_data_encoder.o
	gcc -c includes/crc32.c $(ARGSFASTC) -O3 -o bin/includes__crc32.o

	g++ -c halo/fixes/animation.cpp $(ARGSDEF) -o bin/fixes__animation.o
	g++ -c halo/fixes/cpu_usage.cpp $(ARGSDEF) -o bin/fixes__cpu_usage.o
	g++ -c halo/fixes/file_handle_leak.cpp $(ARGSDEF) -o bin/fixes__file_handle_leak.o
	g++ -c halo/fixes/framerate_dependent_timers.cpp $(ARGSDEF) -o bin/fixes__framerate_dependent_timers.o
	g++ -c halo/fixes/host_refusal.cpp $(ARGSDEF) -o bin/fixes__host_refusal.o
	g++ -c halo/fixes/shdr_trans_zfighting.cpp $(ARGSDEF) -o bin/fixes__shdr_trans_zfighting.o
	g++ -c halo/fixes/string_overflows.cpp $(ARGSDEF) -o bin/fixes__string_overflows.o

	g++ -c halo/functions/console.cpp $(ARGS) -o bin/functions__console.o
	g++ -c halo/functions/devmode.cpp $(ARGSDEF) -o bin/functions__devmode.o
	g++ -c halo/functions/message_delta.cpp $(ARGS) -o bin/functions__message_delta.o
	g++ -c halo/functions/messaging.cpp $(ARGSFAST) -o bin/functions__messaging.o
	g++ -c halo/functions/object_unit.cpp $(ARGS) -o bin/functions__object_unit.o
	g++ -c halo/functions/table.cpp $(ARGS) -o bin/functions__table.o

	g++ -c halo/hooks/console.cpp $(ARGSDEF) -o bin/hooks__console.o
	g++ -c halo/hooks/incoming_packets.cpp $(ARGSDEF) -o bin/hooks__incoming_packets.o
	g++ -c halo/hooks/map.cpp $(ARGSDEF) -o bin/hooks__map.o
	g++ -c halo/hooks/tick.cpp $(ARGSDEF) -o bin/hooks__tick.o

	g++ -c halo/memory/gamestate/object/object.cpp $(ARGSDEF) -o bin/memory__gamestate__object.o
	g++ -c halo/memory/gamestate/object/object_device.cpp $(ARGSDEF) -o bin/memory__gamestate__object_device.o
	g++ -c halo/memory/gamestate/object/object_item.cpp $(ARGSDEF) -o bin/memory__gamestate__object_item.o
	g++ -c halo/memory/gamestate/object/object_unit.cpp $(ARGSDEF) -o bin/memory__gamestate__object_unit.o
	g++ -c halo/memory/gamestate/console.cpp $(ARGSDEF) -o bin/memory__gamestate__terminal.o
	g++ -c halo/memory/gamestate/effect.cpp $(ARGSDEF) -o bin/memory__gamestate__effect.o
	g++ -c halo/memory/gamestate/player.cpp $(ARGSDEF) -o bin/memory__gamestate__player.o
	g++ -c halo/memory/gamestate/table.cpp $(ARGSDEF) -o bin/memory__gamestate__table.o

	g++ -c halo/memory/types.cpp $(ARGSFASTER) -o bin/memory__types.o

	g++ -c halo/network/network_globals.cpp $(ARGSDEF) -o bin/network__network_globals.o
	g++ -c halo/network/foxnet/vulpes_message.cpp $(ARGSDEF) -o bin/network__foxnet__vulpes_message.o

	g++ -c halo/tweaks/loading_screen.cpp $(ARGSDEF) -o bin/tweaks__loading_screen.o
	g++ -c halo/tweaks/tweaks.cpp $(ARGSDEF) -o bin/tweaks__tweaks.o

	g++ -c halo/upgrades/map.cpp $(ARGSDEF) -o bin/upgrades__map.o

	#ld -r -b binary binary/console_font.bin -o bin/console_font.o

	g++ bin/*.o -shared $(LINKARGS) -static-libgcc -static-libstdc++ -lkernel32 -o "bin/Vulpes.dll"
