CXXFLAGS = -std=c++11 -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -fstack-protector

DEBUG_FLAGS = -O0 -ggdb3 -D_DEBUG
RELEASE_FLAGS = -O2 -DNDEBUG

CXXFLAGS += $(DEBUG_FLAGS)
##CXXFLAGS = $(RELEASE_FLAGS)

CXX = g++

all: run

comp: ./bin/comp.o
	$(CXX) ./bin/comp.o $(CXXFLAGS) -o compile

./bin/comp.o: ./src/comp.c ./h/comp.h ./h/colors.h
	$(CXX) -c ./src/comp.c $(CXXFLAGS) -o ./bin/comp.o

run: ./bin/proc.o ./bin/stack.o
	$(CXX) ./bin/proc.o ./bin/stack.o $(CXXFLAGS) -o res.exe

./bin/stack.o: ../stack/stack.cpp ../stack/stack.h ../stack/colors.h
	$(CXX) -c ../stack/stack.cpp $(CXXFLAGS) -o ./bin/stack.o

./bin/proc.o: src/proc.c h/proc.h h/colors.h
	$(CXX) -c ./src/proc.c $(CXXFLAGS) -o ./bin/proc.o

clean:
	rm -f main compile ./bin/*.o