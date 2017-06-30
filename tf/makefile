CC=g++
CPPFLAGS= -std=c++11 -lpthread -lncurses
BASIC = screen.cpp
BACKEND = GameLogic/DungeonMaster.cpp GameLogic/Game.cpp GameLogic/Noun/Character.cpp GameLogic/Noun/Player.cpp GameLogic/Verb/*.cpp
default: server client
server:
	$(CC) $(BASIC) $(BACKEND) server.cpp -o server.run $(CPPFLAGS)
client:
	$(CC) $(BASIC) client.cpp -o client.run $(CPPFLAGS) -DPCUR
debug: CPPFLAGS+=-DDEBUG
debug: default
clean:
	rm -rf *.run *.gch
