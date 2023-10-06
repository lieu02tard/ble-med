#----------Implicit constant------------
# Shell program
SHELL	:=/usr/bin/bash
CC		:=gcc
CCX		:=g++
# C flags
CFLAGS	:=-Wall -Wextra -fstack-protector -pipe -I/usr/include/gtk-4.0 -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/fribidi -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/x86_64-linux-gnu -I/usr/include/graphene-1.0 -I/usr/lib/x86_64-linux-gnu/graphene-1.0/include -mfpmath=sse -msse -msse2 -pthread
# CPP flags
CXXFLAGS:=-Wall -Wextra -fstack-protector -pipe
# Output flags
OFLAGS	:=-o
# Assemble flag (but not link)
CCFLAGS	:=-c
# C preprocessor flag
CPPFLAGS:=
# Linker flag
LDFLAGS	:=-lgtk-4 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -lgdk_pixbuf-2.0 -lcairo-gobject -lcairo -lgraphene-1.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
#------------------------------------------

#----------Directories-------------------
# Build directory
BUILD	:=./build
# Output program
TARGET	:=main
# Output directory
APP_DIR	:=.

# Debug directory
DEB_DIR	:=$(BUILD)/debug
# Object files directory
OBJ_DIR	:=$(BUILD)/obj

# Include flags
INC		:=-I .
# Soruce files directory
SRC_DIR	:= .
SRCS	:=$(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.c')

VALGRIND_LOG:=./valgrind_log

# Object and external object file
OBJ		:=$(SRCS:%=$(OBJ_DIR)/%.o)
EX 		:=

#-----------Content----------------------

all: build $(APP_DIR)/$(TARGET)			#all: target  './main'

#Build from Cpp source
$(OBJ_DIR)/%.cpp.o: %.cpp
		@mkdir -p $(@D)
		$(CCX) $(CXXFLAGS) $(INC) $(CCFLAGS) $< $(OFLAGS) $@ 	
		#g++ -I . -c hello.cpp -o hello.cpp.o

#Build from C source
$(OBJ_DIR)/%.c.o: %.c
		@mkdir -p $(@D)
		$(CC) $(CFLAGS) $(INC) $(CCFLAGS) $< $(OFLAGS) $@

#The final build step
$(APP_DIR)/$(TARGET): $(OBJ)
		$(CC) $(CFLAGS) $(OFLAGS) $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

.PHONY: all build debug execute clean releaase
build:
		@mkdir -p $(APP_DIR)
		@mkdir -p $(OBJ_DIR)

release: CFLAGS+=-O2
release: CXXFLAGS+=-O2
release: all

debug: CFLAGS+=-g -D DEBUG
debug: CXXFLAGS+=-g -D DEBUG
debug: all

execute : all
execute	:
		setsid $(APP_DIR)/$(TARGET)

test	: all
test	:
		valgrind -s --track-origin=yes --leak-check=full --show-leak-kinds=all $(APP_DIR)/$(TARGET) | tee $(VALGRIND_LOG)

clean:
		-@rm -rf $(BUILD)
		-@rm -rf $(APP_DIR)/$(TARGET)
