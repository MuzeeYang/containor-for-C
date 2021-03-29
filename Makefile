.PHONY: all clean

SRC_DIR := src
INC_DIR := inc
LIB_DIR := lib
OUT_DIR := out

COMPILE_PREFIX := 
CC = $(COMPILE_PREFIX)gcc
AR = $(COMPILE_PREFIX)ar
LD = $(COMPILE_PREFIX)ld

SRC_DIRS = $(shell find $(SRC_DIR) -type d)
OBJ_FILES := $(foreach d, $(SRC_DIRS), $(patsubst %.c, %.o, $(wildcard $(d)/*.c)))
vpath %.c $(SRC_DIRS)
vpath %.h $(INC_DIR) $(SRC_DIRS)


LIB_NAME := ctnr4c
TARGETS := lib$(LIB_NAME).a lib$(LIB_NAME).so

CFLAGS += -Wall -I$(INC_DIR) -g
SO_FLAGS := -shared -fPIC
LD_FLAGS := -L$(LIB_DIR) -l$(LIB_NAME) -Wl,-rpath=$(LIB_DIR)

all: $(TARGETS)
	-mv $(LIB_DIR)/* $(OUT_DIR)
	@echo "done."

test: test.o lib$(LIB_NAME).so
	$(CC) $(CFLAGS) -o $@ $< $(LD_FLAGS)

lib$(LIB_NAME).a: $(OBJ_FILES)
	$(AR) crv $(LIB_DIR)/$@ $^

lib$(LIB_NAME).so: $(OBJ_FILES)
	$(CC) $(SO_FLAGS) -o $(LIB_DIR)/$@ $^

clean:
	@-rm -f $(OBJ_FILES) $(LIB_DIR)/lib* $(OUT_DIR)/lib* ./test ./test.o

