ROOT_PREFIX    ?= ../../
ROOT_BUILD_DIR ?= $(ROOT_PREFIX)build/

PREFIX    := $(ROOT_PREFIX)JIT/city/
BUILD_DIR := $(ROOT_BUILD_DIR)

INCLUDES := -I$(ROOT_PREFIX) -I$(ROOT_PREFIX)JIT/city/ -I$(ROOT_PREFIX)ast/city/ -I$(ROOT_PREFIX)frontend/city/ -I$(ROOT_PREFIX)backend/city/
CFLAGS   += -mstackrealign

include $(ROOT_PREFIX)makefiles/auto_deps.mk
