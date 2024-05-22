ROOT_PREFIX    ?= ../../../
ROOT_BUILD_DIR ?= $(ROOT_PREFIX)build/

PREFIX    := $(ROOT_PREFIX)ast/llvm/
BUILD_DIR := $(ROOT_BUILD_DIR)

INCLUDES := -I$(ROOT_PREFIX) -I$(ROOT_PREFIX)ast/llvm/
CFLAGS   += `llvm-config --cxxflags`

include $(ROOT_PREFIX)makefiles/auto_deps.mk
