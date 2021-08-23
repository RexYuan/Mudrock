
.DEFAULT_GOAL := all

###############################################################################
# Environment                                                                 #
###############################################################################
OS := $(shell uname -s)

###############################################################################
# Utilities                                                                   #
###############################################################################
ifeq ($(OS),Darwin)
 RM  := trash
 CXX := g++-10
 CPP := cpp-10 -E
else
 ifeq ($(OS),Linux)
  RM  := rm -rf
  CXX := g++
  CPP := cpp -E
 else
  $(error Darwin/Linux only!)
 endif
endif

###############################################################################
# Filenames                                                                   #
###############################################################################
SRC_ROOT := src
DEP_ROOT := depend
OBJ_ROOT := build

SUB_DIRS := $(patsubst $(SRC_ROOT)/%/, %, $(wildcard $(SRC_ROOT)/*/))
SRC_DIRS := $(addprefix $(SRC_ROOT)/, $(SUB_DIRS))
DEP_DIRS := $(addprefix $(DEP_ROOT)/, $(SUB_DIRS))
OBJ_DIRS := $(addprefix $(OBJ_ROOT)/, $(SUB_DIRS))

SRCS := $(foreach d, $(SRC_DIRS), $(wildcard $(d)/*.cxx))
HDRS := $(foreach d, $(SRC_DIRS), $(wildcard $(d)/*.hxx))
DEPS := $(patsubst $(SRC_ROOT)/%.cxx, $(DEP_ROOT)/%.d, $(SRCS))
OBJS := $(patsubst $(SRC_ROOT)/%.cxx, $(OBJ_ROOT)/%.o, $(SRCS))

###############################################################################
# Source search paths                                                         #
###############################################################################
null  :=
space := $(null) #
colon := :
COLON_SEP_SRC_DIRS := $(subst $(space),$(colon),$(strip $(SRC_DIRS)))
vpath %.cxx $(COLON_SEP_SRC_DIRS)
vpath %.hxx $(COLON_SEP_SRC_DIRS)

###############################################################################
# Submodule paths                                                             #
###############################################################################
LIB_ROOT := lib
MINISAT_ROOT := $(LIB_ROOT)/minisat22-fixed

MINISAT_LIB_DIR  := $(MINISAT_ROOT)/build/release/lib
MINISAT_LIB_NAME := minisat
MINISAT_LIB_AR   := $(MINISAT_LIB_DIR)/lib$(MINISAT_LIB_NAME).a

MINISAT_HDR     := $(LIB_ROOT)/minisat.hxx
MINISAT_PCH     := $(OBJ_ROOT)/minisat.hxx.gch

MINISAT := $(MINISAT_LIB_AR) $(MINISAT_PCH)

###############################################################################
# Compile options                                                             #
###############################################################################
CXX_STD   := -std=c++20
CXX_W     := -Wall
CXX_O     := -O0
CXX_DEBUG :=

CPP_DEF   :=
CPP_PATH  := -iprefix$(SRC_ROOT)/ $(addprefix -iwithprefixbefore, $(SUB_DIRS))
CPP_PATH  += $(addprefix -I, $(LIB_ROOT) $(MINISAT_ROOT))

override CPP_FLAGS = $(CPP_DEF) $(CPP_PATH)
override CXX_FLAGS = $(CXX_STD) $(CXX_W) $(CXX_O) $(CXX_DEBUG) $(CPP_FLAGS)

LD_FLAGS  := -L$(MINISAT_LIB_DIR)
LD_LIBS   := -l$(MINISAT_LIB_NAME)
CXX_LINKS := $(LD_FLAGS) $(LD_LIBS)

MAIN_SRC := main.cxx
MAIN_OBJ := $(MAIN_SRC:.cxx=.o)

OUT := $(MAIN_OBJ)

###############################################################################
# Submodules recipes                                                          #
###############################################################################
$(MINISAT_LIB_AR):
	$(MAKE) -C $(MINISAT_ROOT)

$(MINISAT_PCH): $(MINISAT_HDR) | $(OBJ_ROOT)
	$(CXX) $(CXX_STD) -w -I$(MINISAT_ROOT) -c $< -o $@

###############################################################################
# Canned                                                                      #
###############################################################################
define Silence
-$(1) 2>/dev/null || :
endef

define DepFactory
$(DEP_ROOT)/$(1)/%.d: %.cxx %.hxx | $(DEP_ROOT)/$(1)
	$$(CPP) $$(CXX_FLAGS) -MM -MT$$@ -MT$(OBJ_ROOT)/$(1)/$$*.o $$< -MF$$@
endef

define ObjFactory
$(OBJ_ROOT)/$(1)/%.o: %.cxx %.hxx | $(OBJ_ROOT)/$(1)
	$$(CXX) $$(CXX_FLAGS) -c $$< $$(CXX_LINKS) -o $$@
endef

###############################################################################
# Recipes                                                                     #
###############################################################################
.PHONY: all
all: debug

.PHONY: release
release: CXX_O     := -Ofast
release: CXX_DEBUG := -g0
release: CPP_DEF   := -D NDEBUG
release: $(OUT)

.PHONY: debug
debug: CXX_O     := -Og
debug: CXX_DEBUG := -g3
debug: CPP_DEF   := -D LOGGING -D PROFILING -D COUNTING
debug: $(OUT)

.PHONY: clean cleanall
clean:
	@echo "deleting builds"
	@$(call Silence, $(RM) $(OBJ_ROOT) $(MAIN_OBJ))
cleanall:
	@echo "deleting builds"
	@$(call Silence, $(RM) $(OBJ_ROOT) $(MAIN_OBJ))
	@echo "deleting depends"
	@$(call Silence, $(RM) $(DEP_ROOT))
	@echo "deleting minisat builds"
	@$(call Silence, $(MAKE) -C $(MINISAT_ROOT) -s clean VERB=)

$(OBJ_DIRS): | $(OBJ_ROOT)
$(DEP_DIRS): | $(DEP_ROOT)
$(OBJ_ROOT) $(OBJ_DIRS) $(DEP_ROOT) $(DEP_DIRS):
	mkdir $@

$(MAIN_OBJ): $(MAIN_SRC) $(HDRS) $(OBJS) $(MINISAT) | $(OBJ_ROOT)
	$(CXX) $(CXX_FLAGS) $(OBJS) $< $(CXX_LINKS) -o $@

$(foreach d, $(SUB_DIRS), $(eval $(call DepFactory,$(d))))
$(foreach d, $(SUB_DIRS), $(eval $(call ObjFactory,$(d))))

-include $(DEPS)
