
.DEFAULT_GOAL := all

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
# Minisat                                                                     #
###############################################################################
MINISAT_DIR := lib
MINISAT_HDR := $(MINISAT_DIR)/minisat.hxx
MINISAT_PCH := $(OBJ_ROOT)/minisat.hxx.gch
MINISAT_AR  := minisat

###############################################################################
# Compile options                                                             #
###############################################################################
CXX     := g++-10
CXX_STD := -std=c++20
CXX_W   := -Wall

CXX_INCS  := $(addprefix -I, $(dir $(SRCS)) $(MINISAT_DIR))
CXX_LINKS := -L$(MINISAT_DIR) -l$(MINISAT_AR)

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
$(DEP_ROOT)/$(1)/%.d: $(SRC_ROOT)/$(1)/%.cxx $(SRC_ROOT)/$(1)/%.hxx | $(DEP_ROOT)/$(1)
	$$(CXX) $$(CXX_FLAGS) -MM -MT$$@ -MT$(OBJ_ROOT)/$(1)/$$*.o $$< -MF$$@
endef

define ObjFactory
$(OBJ_ROOT)/$(1)/%.o: $(SRC_ROOT)/$(1)/%.cxx $(SRC_ROOT)/$(1)/%.hxx | $(OBJ_ROOT)/$(1)
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
	@echo "trash-ing builds"
	@$(call Silence, trash $(OBJ_ROOT))

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
