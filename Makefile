.PHONY: all head

all: head

head: src/cchead.cpp
	g++ src/cchead.cpp -o build/cchead