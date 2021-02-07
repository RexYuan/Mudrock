
.DEFAULT_GOAL := all

CXX := g++-10

CXXFLAGS := -std=c++20 -I. -Wall
HXXFLAGS := -std=c++20 -I. -w

LIBS := -L. -lminisat
OBJS := bv.o mana.o
HDRS := minisat.hxx.gch
MAIN := main.cxx
DEP := $(OBJS) $(HDRS) $(MAIN)

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
	@echo "trash-ing builds"
	-trash *.o *.gch *.dSYM 2>/dev/null || :

%.o: %.cxx %.hxx
	$(CXX) $(CXXFLAGS) -c $< $(LIBS) -o $@

minisat.hxx.gch: minisat.hxx
	$(CXX) $(HXXFLAGS) -c minisat.hxx
