# I'm noob in writing makefiles, will improve this later on.

release32:
	mkdir -p ./obj/
	mkdir -p ./bin/
	mkdir -p ./bin/Release/
	g++ -m32 -pthread -std=c++11 -c -fPIC -I./inc/ -I/usr/local/include ./src/*.cpp
	mv *.o obj/
	g++ -m32 -pthread -std=c++11 -shared -o ./bin/Release/RCON32.so ./obj/*.o -L/usr/local/lib
	@rm -Rf obj/*
	@echo done.

release64:
	mkdir -p ./obj/
	mkdir -p ./bin/
	mkdir -p ./bin/Release/
	g++ -m64 -pthread -std=c++11 -c -fPIC -I./inc/ -I/usr/local/include ./src/*.cpp
	mv *.o obj/
	g++ -m64 -pthread -std=c++11 -shared -o ./bin/Release/RCON64.so ./obj/*.o -L/usr/local/lib
	@rm -Rf obj/*
	@echo done.

debug32:
	mkdir -p ./obj/
	mkdir -p ./bin/
	mkdir -p ./bin/Debug/
	g++ -m32 -pthread -std=c++11 -c -g -fPIC -I./inc/ -I/usr/local/include -D_DEBUG ./src/*.cpp
	mv *.o obj/
	g++ -m32 -pthread -std=c++11 -shared -o ./bin/Debug/RCON32.so ./obj/*.o -L/usr/local/lib
	@rm -Rf obj/*
	@echo done.

debug64:
	mkdir -p ./obj/
	mkdir -p ./bin/
	mkdir -p ./bin/Debug/
	g++ -m64 -pthread -std=c++11 -c -g -fPIC -I./inc/ -I/usr/local/include -D_DEBUG ./src/*.cpp
	mv *.o obj/
	g++ -m64 -pthread -std=c++11 -shared -o ./bin/Debug/RCON64.so ./obj/*.o -L/usr/local/lib
	@rm -Rf obj/*
	@echo done.

all: clean release32 release64

clean:
	@rm -Rf bin/*
	@rm -Rf obj/*
