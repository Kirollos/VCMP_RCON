# I'm noob in writing makefiles, will improve this later on.

debug32:
	mkdir -p squirrelsrc/lib/
	mkdir -p RCON/lib/
	cd squirrelsrc/squirrel && make sq32
	cp squirrelsrc/lib/libsquirrel.a RCON/lib/libsquirrel.a
	cd RCON && make debug32

debug64:
	mkdir -p squirrelsrc/lib/
	mkdir -p RCON/lib/
	cd squirrelsrc/squirrel && make sq64
	cp squirrelsrc/lib/libsquirrel.a RCON/lib/libsquirrel.a
	cd RCON && make debug64

release32:
	mkdir -p squirrelsrc/lib/
	mkdir -p RCON/lib/
	cd squirrelsrc/squirrel && make sq32
	cp squirrelsrc/lib/libsquirrel.a RCON/lib/libsquirrel.a
	cd RCON && make release32

release64:
	mkdir -p squirrelsrc/lib/
	mkdir -p RCON/lib/
	cd squirrelsrc/squirrel && make sq64
	cp squirrelsrc/lib/libsquirrel.a RCON/lib/libsquirrel.a
	cd RCON && make release64

all: release32 release64
