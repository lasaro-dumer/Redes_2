GCC = g++
CFLAGS = -std=c++11
SNIF = monitor.cpp
BIN = sniffer.o

default:
	$(GCC) $(SNIF) -o $(BIN) $(CFLAGS)
debug: CFLAGS += -DDEBUG
debug: default
