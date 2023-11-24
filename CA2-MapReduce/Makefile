DIR_SRC   := src
DIR_BUILD := build
DIR_BIN   := $(DIR_BUILD)/bin
DIR_OBJ   := $(DIR_BUILD)/obj
DIR_DEP   := $(DIR_OBJ)/__dep__

CC        = g++
CPPFLAGS += -std=c++11

MKDIR  = mkdir -p
RM     = rm -f
RMDIR  = rm -rf
COPY   = cp
MOVE   = mv -f
NULL_DEVICE = /dev/null


OUT_EXE_MAIN := main.out
# OUT_EXE_CUSTOMER := customer.out
# OUT_EXE_SUPPLIER := supplier.out

#----------------------------------------

VDIR = $(DIR_SRC)

FILES   = $(patsubst src/%, %, $(shell find $(DIR_SRC) -name "*.cpp" -type f))
FOLDERS = $(patsubst src/%, %, $(shell find $(DIR_SRC) -mindepth 1 -type d))

FILES_NOMAIN = $(filter-out main.cpp, $(FILES))

FILES_DEP = $(patsubst %, $(DIR_DEP)/%.d, $(basename $(FILES)))
FILES_OBJ = $(patsubst %, $(DIR_OBJ)/%.o, $(basename $(FILES_NOMAIN)))

#----------------------------------------

all: $(OUT_EXE_MAIN)

$(OUT_EXE_MAIN): $(DIR_OBJ)/main.o $(FILES_OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# $(OUT_EXE_CUSTOMER): $(DIR_OBJ)/customer.o $(FILES_OBJ)
# 	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# $(OUT_EXE_SUPPLIER): $(DIR_OBJ)/supplier.o $(FILES_OBJ)
# 	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

DEPFLAGS    = -MT $@ -MMD -MP -MF $(DIR_DEP)/$*.dTMP
POSTCOMPILE = @$(MOVE) $(DIR_DEP)/$*.dTMP $(DIR_DEP)/$*.d > $(NULL_DEVICE) && touch $@

$(DIR_OBJ)/%.o: %.c
$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp $(DIR_DEP)/%.d | directories
	$(CC) $(CPPFLAGS) -c $(DEPFLAGS) $< -o $@
	$(POSTCOMPILE)

.PRECIOUS: $(FILES_DEP)
$(FILES_DEP): ;
-include $(FILES_DEP)

#----------------------------------------

directories: $(DIR_BUILD) $(DIR_BIN) $(DIR_OBJ) $(DIR_DEP) nested-folders
nested-folders: $(addprefix $(DIR_OBJ)/, $(FOLDERS)) $(addprefix $(DIR_DEP)/, $(FOLDERS))

$(DIR_BUILD): ; $(MKDIR) $@
$(DIR_BIN): ; $(MKDIR) $@
$(DIR_OBJ): ; $(MKDIR) $@
$(DIR_DEP): ; $(MKDIR) $@

$(addprefix $(DIR_OBJ)/, $(FOLDERS)): ; @$(MKDIR) $@
$(addprefix $(DIR_DEP)/, $(FOLDERS)): ; @$(MKDIR) $@

#----------------------------------------

.PHONY: all directories nested-folders \
		clean clean-obj clean-dep clean-exe delete-build \
		run-byr run-slr help

clean: clean-obj clean-dep clean-exe 
clean-obj: ; $(RMDIR) $(DIR_OBJ)/*
clean-dep: ; $(RMDIR) $(DIR_DEP)/*
clean-exe: ; $(RMDIR) ./*.out
delete-build: ; $(RMDIR) $(DIR_BUILD)

help:
	@echo Targets: all clean clean-obj clean-dep clean-exe delete-build run-byr run-slr
	@echo '(make run ARGS="arg1 arg2...")'