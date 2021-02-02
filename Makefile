
.DEFAULT_GOAL := all

CXX := g++-10

CXXFLAGS := -std=c++20 -I.
HXXFLAGS := -std=c++20 -I.

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
	@-trash *.o || :
	@-trash *.gch || :
	@-trash *.dSYM || :

%.o: %.cxx %.hxx
	$(CXX) $(CXXFLAGS) -c $< $(LIBS) -o $@

minisat.hxx.gch: minisat.hxx
	$(CXX) $(HXXFLAGS) -c minisat.hxx
