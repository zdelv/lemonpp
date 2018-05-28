ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell uname -s)
endif

ifeq ($(detected_OS),Windows)
	STDFLAG = -std=c++11
endif
ifeq ($(detected_OS),Darwin)  # Mac OS X
    STDFLAG = -std=c++1y  
endif
ifeq ($(detected_OS),Linux)
    STDFLAG = -std=c++11
endif

CXX = clang++
SRC = src/
# CPU = src/gba/cpu/
# LDR = src/gba/loader/
OARGS = -pthread -std=c++17 -g
PROGS = item.o event.o userItems.o bar.o
DIRPROGS = bin/item.o bin/event.o bin/userItems.o bin/bar.o 
VPATH = bin

%.o : %.cpp | bin 
		$(CXX) $(OARGS) -c -o bin/$@ $<

lpp: $(PROGS) main.cpp | bin/
		$(CXX) $(OARGS) -o bin/lpp $(DIRPROGS) main.cpp

clean: 
		rm -rf *.o *.obj *.exe $(PROGS) bin/

bin:
		mkdir bin
