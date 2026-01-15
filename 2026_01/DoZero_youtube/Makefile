DEFS =
ifeq ($(PAUSE),1)
    DEFS = -DENABLE_PAUSE
endif

APPS    := apps
SRC     := src
INCLUDE := include
OBJ     := obj
BIN     := bin
LIB     := lib

CC      := gcc
CFLAGS  := -O3 -Wall -Wextra -I$(INCLUDE) -Wno-unused-parameter
MATHFLAGS := -lm
AR      := ar
ARFLAGS := rcs

LIBNAME := libed.a
LIBPATH := $(LIB)/$(LIBNAME)

SRCS := $(wildcard $(SRC)/*.c)
OBJS := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))

APPS_SRC := $(wildcard $(APPS)/*.c)
APPS_BIN := $(patsubst $(APPS)/%.c,$(BIN)/%,$(APPS_SRC))

.PHONY: all clean run dirs exec

all: clean dirs $(LIBPATH) $(APPS_BIN)

$(LIBPATH): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ)/%.o: $(SRC)/%.c $(INCLUDE)/%.h | dirs
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@

$(BIN)/%: $(APPS)/%.c $(LIBPATH) | dirs
	$(CC) $(CFLAGS) $(DEFS) $< -L$(LIB) -led $(MATHFLAGS) -o $@

dirs:
	mkdir -p $(OBJ) $(BIN) $(LIB)

run:
	$(BIN)/$(notdir $(firstword $(APPS_BIN)))

clean:
	rm -rf $(OBJ) $(BIN) $(LIB)

exec: clean all run
