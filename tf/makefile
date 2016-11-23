CC=g++
CPPFLAGS=
default:
	$(CC) mittle.cpp -o mittle $(CPPFLAGS)
debugp: CPPFLAGS+=-DDEBUGP
debugp: debug
debug: CPPFLAGS+=-DDEBUG
debug: default
