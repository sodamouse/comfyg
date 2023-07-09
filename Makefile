CC = c++
CFLAGS = -Wall -Wextra -Wpedantic
INPUT = test.cpp
INSTALL_SRCS = comfyg.hpp
OUTPUT = test

.PHONY: all

all:
	$(CC) $(CLFAGS) -o $(OUTPUT) $(INPUT)

clean:
	rm $(OUTPUT) -v

install:
	mkdir /usr/local/include/comfyg
	cp -v $(INSTALL_SRCS) /usr/local/include/comfyg/

uninstall:
	rm -rfv /usr/local/include/comfyg
