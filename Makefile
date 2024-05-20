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

AST_CITY_BUILD_DIR := $(ROOT_BUILD_DIR)ast/city/

.PHONY: ast_city_obj
ast_city_obj:
	$(MAKE) -f makefiles/ast/city/Makefile

AST_CITY_OBJS := $(shell find $(AST_CITY_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_ast_city_obj
clean_ast_city_obj:
	rm -rf $(AST_CITY_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

AST_LLVM_BUILD_DIR := $(ROOT_BUILD_DIR)ast/llvm/

.PHONY: ast_llvm_obj
ast_llvm_obj:
	$(MAKE) -f makefiles/ast/llvm/Makefile

AST_LLVM_OBJS := $(shell find $(AST_LLVM_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_ast_llvm_obj
clean_ast_llvm_obj:
	rm -rf $(AST_LLVM_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

BACKEND_CITY_BUILD_DIR := $(ROOT_BUILD_DIR)backend/city/

.PHONY: backend_city_obj
backend_city_obj:
	$(MAKE) -f makefiles/backend/city/Makefile

BACKEND_CITY_OBJS := $(shell find $(BACKEND_CITY_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_backend_city_obj
clean_backend_city_obj:
	rm -rf $(BACKEND_CITY_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

TOKENIZER_BUILD_DIR := $(ROOT_BUILD_DIR)frontend/tokenizer/

.PHONY: tokenizer_obj
tokenizer_obj:
	$(MAKE) -f makefiles/frontend/tokenizer/Makefile

TOKENIZER_OBJS := $(shell find $(TOKENIZER_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_tokenizer_obj
clean_tokenizer_obj:
	rm -rf $(TOKENIZER_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

FRONTEND_CITY_BUILD_DIR := $(ROOT_BUILD_DIR)frontend/city/

.PHONY: frontend_city_obj
frontend_city_obj:
	$(MAKE) -f makefiles/frontend/city/Makefile

FRONTEND_CITY_OBJS := $(shell find $(FRONTEND_CITY_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_frontend_city_obj
clean_frontend_city_obj:
	rm -rf $(FRONTEND_CITY_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

FRONTEND_LLVM_BUILD_DIR := $(ROOT_BUILD_DIR)frontend/llvm/

.PHONY: frontend_llvm_obj
frontend_llvm_obj:
	$(MAKE) -f makefiles/frontend/llvm/Makefile

FRONTEND_LLVM_OBJS := $(shell find $(FRONTEND_LLVM_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_frontend_llvm_obj
clean_frontend_llvm_obj:
	rm -rf $(FRONTEND_LLVM_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

JIT_CITY_BUILD_DIR := $(ROOT_BUILD_DIR)JIT/city/

.PHONY: JIT_city_obj
JIT_city_obj:
	$(MAKE) -f makefiles/JIT/city/Makefile

JIT_CITY_OBJS := $(shell find $(JIT_CITY_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_JIT_city_obj
clean_JIT_city_obj:
	rm -rf $(JIT_CITY_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

JIT_LLVM_BUILD_DIR := $(ROOT_BUILD_DIR)JIT/llvm/

.PHONY: JIT_llvm_obj
JIT_llvm_obj:
	$(MAKE) -f makefiles/JIT/llvm/Makefile

JIT_LLVM_OBJS := $(shell find $(JIT_LLVM_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_JIT_llvm_obj
clean_JIT_llvm_obj:
	rm -rf $(JIT_LLVM_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

LIB_BUILD_DIR := $(ROOT_BUILD_DIR)lib/

.PHONY: lib_obj
lib_obj:
ifeq ($(debug), 1)
	$(MAKE) BUILD_DIR:='$(ROOT_BUILD_DIR)' PREFIX:='$(ROOT_PREFIX)lib/' -f lib/Makefile debug=0 verify=1
else
	$(MAKE) BUILD_DIR:='$(ROOT_BUILD_DIR)' PREFIX:='$(ROOT_PREFIX)lib/' -f lib/Makefile debug=0 verify=0 release=1
endif

LIB_OBJS := $(shell find $(LIB_BUILD_DIR) -name "*.o" 2>/dev/null)

.PHONY: clean_lib_obj
clean_lib_obj:
	rm -rf $(LIB_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

.PHONY: all_obj
all_obj: ast_city_obj ast_llvm_obj backend_city_obj tokenizer_obj frontend_city_obj frontend_llvm_obj JIT_city_obj JIT_llvm_obj lib_obj;

.PHONY: clean_all_obj
clean_all_obj: clean_ast_city_obj clean_ast_llvm_obj clean_backend_city_obj clean_tokenizer_obj clean_frontend_city_obj clean_frontend_llvm_obj clean_JIT_city_obj clean_JIT_llvm_obj clean_lib_obj;

#--------------------------------------------------------------------------------------------------------------------------------

.PHONY: CityJIT_exe_objs
CityJIT_exe_objs: ast_city_obj backend_city_obj tokenizer_obj frontend_city_obj JIT_city_obj lib_obj;

CITY_JIT_EXE_OBJS := $(AST_CITY_OBJS) $(BACKEND_CITY_OBJS) $(TOKENIZER_OBJS) $(FRONTEND_CITY_OBJS) $(JIT_CITY_OBJS) $(LIB_OBJS)

.PHONY: CityJIT_exe_linkage
CityJIT_exe_linkage:
	$(CC) $(CFLAGS) $(CITY_JIT_EXE_OBJS) -o $(ROOT_BUILD_DIR)CityJIT

.PHONY: CityJIT_exe
CityJIT_exe:
	$(MAKE) CityJIT_exe_objs
	$(MAKE) CityJIT_exe_linkage
all: CityJIT_exe

#--------------------------------------------------------------------------------------------------------------------------------

.PHONY: LlvmJIT_exe_objs
LlvmJIT_exe_objs: ast_llvm_obj tokenizer_obj frontend_llvm_obj JIT_llvm_obj lib_obj;

LLVM_JIT_EXE_OBJS := $(AST_LLVM_OBJS) $(TOKENIZER_OBJS) $(FRONTEND_LLVM_OBJS) $(JIT_LLVM_OBJS) $(LIB_OBJS)

.PHONY: LlvmJIT_exe_linkage
LlvmJIT_exe_linkage:
	$(CC) $(CFLAGS) $(LLVM_JIT_EXE_OBJS) -o $(ROOT_BUILD_DIR)LlvmJIT

.PHONY: LlvmJIT_exe
LlvmJIT_exe:
	$(MAKE) LlvmJIT_exe_objs
	$(MAKE) LlvmJIT_exe_linkage
all: LlvmJIT_exe


#--------------------------------------------------------------------------------------------------------------------------------

.PHONY: clean
clean:
	rm -rf $(ROOT_BUILD_DIR)

#--------------------------------------------------------------------------------------------------------------------------------

.PHONY: compilation_database
compilation_database:
	$(MAKE) clean_all_obj
	bear -- $(MAKE) all_obj -k
