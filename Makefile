
.DEFAULT_GOAL := all

###############################################################################
# Utilities                                                                   #
###############################################################################
RM  := trash
CXX := g++-10
CPP := cpp-10 -E

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
# Minisat                                                                     #
###############################################################################
MINISAT_DIR := lib
MINISAT_HDR := $(MINISAT_DIR)/minisat.hxx
MINISAT_PCH := $(OBJ_ROOT)/minisat.hxx.gch
MINISAT_AR  := minisat

###############################################################################
# Compile options                                                             #
###############################################################################
CXX_STD := -std=c++20
CXX_W   := -Wall

CPP_FLAGS := $(addprefix -I, $(dir $(SRCS)) $(MINISAT_DIR))
CXX_INCS  := $(CPP_FLAGS)

LD_FLAGS  := -L$(MINISAT_DIR)
LD_LIBS   := -l$(MINISAT_AR)
CXX_LINKS := $(LD_FLAGS) $(LD_LIBS)

CXX_FLAGS := $(CXX_STD) $(CXX_W) $(CXX_INCS)

MAIN_SRC := main.cxx
MAIN_OBJ := $(OBJ_ROOT)/$(MAIN_SRC:.cxx=.o)

OUT := $(MAIN_OBJ)

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
.PHONY: all run
all: $(OUT) run
all: CXX_FLAGS += -g -O0
run:
	./$(OUT)

.PHONY: debug
debug: CXX_FLAGS += -g -O0 -DPRINT
debug: $(OUT)
debug:
	lldb $(OUT)

.PHONY: clean
clean:
	@echo "deleting builds"
	@$(call Silence, $(RM) $(OBJ_ROOT))

$(OBJ_DIRS): | $(OBJ_ROOT)
$(DEP_DIRS): | $(DEP_ROOT)
$(OBJ_ROOT) $(OBJ_DIRS) $(DEP_ROOT) $(DEP_DIRS):
	mkdir $@

$(MINISAT_PCH): $(MINISAT_HDR) | $(OBJ_ROOT)
	$(CXX) $(CXX_STD) -w -I$(MINISAT_DIR) -c $< -o $@

$(MAIN_OBJ): $(MAIN_SRC) $(OBJS) $(MINISAT_PCH) | $(OBJ_ROOT)
	$(CXX) $(CXX_FLAGS) $(OBJS) $< $(CXX_LINKS) -o $@

$(foreach d, $(SUB_DIRS), $(eval $(call DepFactory,$(d))))
$(foreach d, $(SUB_DIRS), $(eval $(call ObjFactory,$(d))))

-include $(DEPS)
