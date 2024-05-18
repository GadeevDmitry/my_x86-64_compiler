CC := clang++
export CC

#--------------------------------------------------------------------------------------------------------------------------------

CFLAGS_DEBUG    := -D _DEBUG -std=c++20 -ggdb3 -Wall -O0 -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -pie -Wstack-usage=8192
CFLAGS_RELEASE  := -D NDEBUG -D NVERIFY -std=c++20 -O2
CFLAGS_SANITIZE := -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

ifeq ($(debug), 1)
CFLAGS := $(CFLAGS_DEBUG)
else
CFLAGS := $(CFLAGS_RELEASE)
endif

ifeq ($(sanitize), 1)
CFLAGS += $(CFLAGS_SANITIZE)
endif

export CFLAGS

#--------------------------------------------------------------------------------------------------------------------------------

ROOT_PREFIX    := ./
ROOT_BUILD_DIR := build/

export ROOT_PREFIX
export ROOT_BUILD_DIR

#================================================================================================================================

.PHONY: all
all:;

#--------------------------------------------------------------------------------------------------------------------------------

AST_BUILD_DIR := $(ROOT_BUILD_DIR)ast/

.PHONY: ast_obj
ast_obj:
	$(MAKE) -f makefiles/ast/Makefile

AST_OBJS := $(shell find $(AST_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_ast_obj
clean_ast_obj:
	rm -rf $(AST_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

BACKEND_BUILD_DIR := $(ROOT_BUILD_DIR)backend/

.PHONY: backend_obj
backend_obj:
	$(MAKE) -f makefiles/backend/Makefile

BACKEND_OBJS := $(shell find $(BACKEND_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_backend_obj
clean_backend_obj:
	rm -rf $(BACKEND_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

FRONTEND_BUILD_DIR := $(ROOT_BUILD_DIR)frontend/

.PHONY: frontend_obj
frontend_obj:
	$(MAKE) -f makefiles/frontend/Makefile

FRONTEND_OBJS := $(shell find $(FRONTEND_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_frontend_obj
clean_frontend_obj:
	rm -rf $(FRONTEND_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

JIT_BUILD_DIR := $(ROOT_BUILD_DIR)JIT/

.PHONY: JIT_obj
JIT_obj:
	$(MAKE) -f makefiles/JIT/Makefile

JIT_OBJS := $(shell find $(JIT_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_JIT_obj
clean_JIT_obj:
	rm -rf $(JIT_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

LIB_BUILD_DIR := $(ROOT_BUILD_DIR)lib/

.PHONY: lib_obj
lib_obj:
ifeq ($(debug), 1)
	$(MAKE) BUILD_DIR:='$(ROOT_BUILD_DIR)' PREFIX:='$(ROOT_PREFIX)lib/' -f lib/Makefile mode=verify
else
	$(MAKE) BUILD_DIR:='$(ROOT_BUILD_DIR)' PREFIX:='$(ROOT_PREFIX)lib/' -f lib/Makefile mode=release
endif

LIB_OBJS := $(shell find $(LIB_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_lib_obj
clean_lib_obj:
	rm -rf $(LIB_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

.PHONY: all_obj
all_obj: ast_obj backend_obj frontend_obj JIT_obj lib_obj;

.PHONY: clean_obj
clean_all_obj: clean_ast_obj clean_backend_obj clean_frontend_obj clean_JIT_obj clean_lib_obj;

#--------------------------------------------------------------------------------------------------------------------------------

.PHONY: CityJIT_exe_objs
CityJIT_exe_objs: ast_obj backend_obj frontend_obj JIT_obj lib_obj;

CITY_JIT_EXE_OBJS := $(AST_OBJS) $(BACKEND_OBJS) $(FRONTEND_OBJS) $(JIT_OBJS) $(LIB_OBJS)

.PHONY: CityJIT_exe_linkage
CityJIT_exe_linkage:
	$(CC) $(CFLAGS) $(CITY_JIT_EXE_OBJS) -o $(ROOT_BUILD_DIR)CityJIT

.PHONY: CityJIT_exe
CityJIT_exe:
	$(MAKE) CityJIT_exe_objs
	$(MAKE) CityJIT_exe_linkage
all: CITY_JIT_exe

#--------------------------------------------------------------------------------------------------------------------------------

.PHONY: clean
clean:
	rm -rf $(ROOT_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

.PHONY: compilation_database
compilation_database:
	$(MAKE) clean_all_obj
	bear -- $(MAKE) all_obj -k
