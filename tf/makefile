CC=g++
CPPFLAGS= -std=c++11 -lpthread -lncurses
BASIC = screen.cpp util.cpp printer.cpp common.cpp in_cksum.cpp GameLogic/response.cpp
BACKEND = GameLogic/DungeonMaster.cpp GameLogic/Game.cpp GameLogic/Noun/*.cpp GameLogic/Verb/*.cpp
default: server client
server:
	$(CC) $(BASIC) $(BACKEND) server.cpp -o server.run $(CPPFLAGS)
client:
	$(CC) $(BASIC) client.cpp -o client.run $(CPPFLAGS)
debug: CPPFLAGS+=-DDEBUG
debug: default
alien: alien-srv alien-cli
	echo 'done with alien'
alien-srv:
	$(CC) alien.cpp $(BASIC) -o alien_srv.run $(CPPFLAGS) -DFLG_SERVER
alien-cli:
	$(CC) alien.cpp $(BASIC) -o alien_cli.run $(CPPFLAGS)
curse-cli: CPPFLAGS+=-DPCUR
curse-cli: client
clean:
	rm -rf *.run *.gch
