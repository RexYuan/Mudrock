
.DEFAULT_GOAL := all

CXX := g++-10

CXXFLAGS := -std=c++20 -I. -O3
HXXFLAGS := -std=c++20 -I. -O3

LIBS := -L. -lminisat
OBJS := bv.o
HDRS := minisat.hxx.gch bv.hxx
MAIN := main.cxx
DEP := $(MINISAT) $(OBJS) $(HDRS) $(MAIN)

OUT := mudk.o

.PHONY: all
all: build run

.PHONY: debug
debug: CXXFLAGS += -g -O0
debug: build

.PHONY: build
build: $(DEP)
	$(CXX) $(CXXFLAGS) $(OBJS) $(MAIN) $(LIBS) -o $(OUT)

.PHONY: run
run:
	./$(OUT)

.PHONY: clean
clean:
	-trash *.o
	-trash *.gch

bv.o: bv.cxx bv.hxx
	$(CXX) $(CXXFLAGS) -c bv.cxx $(LIBS) -o bv.o

minisat.hxx.gch: minisat.hxx
	$(CXX) $(HXXFLAGS) -c minisat.hxx
