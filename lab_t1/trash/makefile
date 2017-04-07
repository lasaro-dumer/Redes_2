GCC = g++
STD = -std=c++11
LD_FLAGS = -lncurses
SNIF = meter.cpp sniffer.cpp
BIN = sniffer.o

default:
	$(GCC) $(SNIF) -o $(BIN) $(LD_FLAGS) $(STD)
old:
	$(GCC) monitor.cpp -o monitor.o
