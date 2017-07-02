CC=g++
CPPFLAGS= -std=c++11 -lpthread -lncurses
BASIC = screen.cpp util.cpp GameLogic/response.cpp
BACKEND = GameLogic/DungeonMaster.cpp GameLogic/Game.cpp GameLogic/Noun/Character.cpp GameLogic/Noun/Player.cpp GameLogic/Verb/*.cpp
default: server client
server:
	$(CC) $(BASIC) $(BACKEND) server.cpp -o server.run $(CPPFLAGS)
client:
	$(CC) $(BASIC) client.cpp -o client.run $(CPPFLAGS)
debug: CPPFLAGS+=-DDEBUG
debug: default
curse-cli: CPPFLAGS+=-DPCUR
curse-cli: client
clean:
	rm -rf *.run *.gch
