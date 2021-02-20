
.DEFAULT_GOAL := all

CXX := g++-10

CXXFLAGS := -std=c++20 -I. -Wall
HXXFLAGS := -std=c++20 -I. -w

DATAS := bv.o bf.o face.o
SOLVERS := mana.o ora.o
LEARNERS :=

LIBS := -L. -lminisat
OBJS := $(DATAS) $(SOLVERS) $(LEARNERS)
HDRS := minisat.hxx.gch
MAIN := main.cxx
DEP := $(HDRS) $(OBJS) $(MAIN)

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
