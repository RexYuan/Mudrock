
.DEFAULT_GOAL := all

###############################################################################
# Filenames                                                                   #
###############################################################################
SRC_ROOT := src
OBJ_ROOT := build
DEP_ROOT := dep

SUB_DIRS := $(patsubst $(SRC_ROOT)/%/, %, $(wildcard $(SRC_ROOT)/*/))
SRC_DIRS := $(addprefix $(SRC_ROOT)/, $(SUB_DIRS))
OBJ_DIRS := $(addprefix $(OBJ_ROOT)/, $(SUB_DIRS))
DEP_DIRS := $(addprefix $(DEP_ROOT)/, $(SUB_DIRS))

SRCS := $(foreach d, $(SRC_DIRS), $(wildcard $(d)/*.cxx))
OBJS := $(patsubst $(SRC_ROOT)/%.cxx, $(OBJ_ROOT)/%.o, $(SRCS))
DEPS := $(patsubst $(SRC_ROOT)/%.cxx, $(DEP_ROOT)/%.d, $(SRCS))

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

MAIN     := main.cxx
MAIN_DEP := $(MAIN) $(OBJS) $(MINISAT_PCH)
OUT      := $(OBJ_ROOT)/main.o

###############################################################################
# Canned                                                                      #
###############################################################################
define Silence
-$(1) 2>/dev/null || :
endef

define ObjFactory
$(OBJ_ROOT)/$(1)/%.o: $(SRC_ROOT)/$(1)/%.cxx $(SRC_ROOT)/$(1)/%.hxx | $(OBJ_ROOT)/$(1)
	$$(CXX) $$(CXX_FLAGS) -c $$< $$(CXX_LINKS) -o $$@
endef

define DepFactory
$(DEP_ROOT)/$(1)/%.d: $(SRC_ROOT)/$(1)/%.cxx $(SRC_ROOT)/$(1)/%.hxx | $(DEP_ROOT)/$(1)
	$$(CXX) $$(CXX_FLAGS) -MM -MT$$@ -MT$$(<:.cxx=.o) $$< -MF$$@
endef

###############################################################################
# Recipes                                                                     #
###############################################################################
.PHONY: all run
all: depend $(OUT) run
run:
	./$(OUT)

.PHONY: debug
debug: CXX_FLAGS += -g -O0
debug: $(OUT)
debug:
	lldb $(OUT)

.PHONY: depend
depend: $(DEPS) | $(DEP_ROOT)

.PHONY: clean
clean:
	@echo "trash-ing builds"
	@$(call Silence, trash $(OBJ_ROOT))
	@$(call Silence, trash $(DEP_ROOT))

$(OBJ_DIRS): | $(OBJ_ROOT)
$(DEP_DIRS): | $(DEP_ROOT)
$(OBJ_ROOT) $(OBJ_DIRS) $(DEP_ROOT) $(DEP_DIRS):
	mkdir $@

$(MINISAT_PCH): $(MINISAT_HDR) | $(OBJ_ROOT)
	$(CXX) $(CXX_STD) -w -I$(MINISAT_DIR) -c $< -o $@

$(OUT): $(MAIN_DEP) | $(OBJ_ROOT)
	$(CXX) $(CXX_FLAGS) $(OBJS) $(MAIN) $(CXX_LINKS) -o $@

$(foreach d, $(SUB_DIRS), $(eval $(call DepFactory,$(d))))
$(foreach d, $(SUB_DIRS), $(eval $(call ObjFactory,$(d))))

-include $(DEPS)
