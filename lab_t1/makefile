GCC = g++
CFLAGS = -std=c++11
SNIF = monitor.cpp counter.cpp printer.cpp
BIN = sniffer.o

default:
	$(GCC) $(SNIF) -o $(BIN) $(CFLAGS)
debug: CFLAGS += -DDEBUG
debug: default
curses: CFLAGS += -DPCUR -lncurses
curses: debug
