ROOT_PREFIX    ?= ../../
ROOT_BUILD_DIR ?= $(ROOT_PREFIX)build/

PREFIX    := $(ROOT_PREFIX)JIT/llvm/
BUILD_DIR := $(ROOT_BUILD_DIR)

INCLUDES := -I$(ROOT_PREFIX) -I$(ROOT_PREFIX)JIT/llvm/ -I$(ROOT_PREFIX)ast/llvm/ -I$(ROOT_PREFIX)frontend/llvm/
CFLAGS   += -mstackrealign

include $(ROOT_PREFIX)makefiles/auto_deps.mk
