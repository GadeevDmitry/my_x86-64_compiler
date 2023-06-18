#================================================================================================================================
# COMMON
#================================================================================================================================

CFLAGS  = -D _DEBUG -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -pie -Wlarger-than=8192 -Wstack-usage=8192
OBJ_DIR = obj

#================================================================================================================================
# LIB
#================================================================================================================================

TRC	  = ../lib/logs/trace/trace
LOG   = ../lib/logs/log
ALG   = ../lib/algorithm/algorithm
STK   = ../lib/stack/stack
VEC   = ../lib/vector/vector
ARR   = ../lib/array/array

TRC_H = $(TRC).h
LOG_H = $(LOG).h $(LOG)_settings.h $(LOG)_def.h
ALG_H = $(ALG).h $(LOG_H)
STK_H = $(STK).h $(LOG_H) $(ALG_H)
VEC_H = $(VEC).h $(STK_H)
ARR_H = $(ARR).h $(LOG_H) $(ALG_H)

#--------------------------------------------------------------------------------------------------------------------------------

AST_H      = ../ast/ast.h
FRONTEND_H = ../frontend/frontend.h
BACKEND_H  = ../backend/backend.h

#================================================================================================================================
# JIT
#================================================================================================================================

JIT:
	make $(JIT_O)

clean:
	clean -r -f $(OBJ_DIR)/

.PHONY: JIT clean

#--------------------------------------------------------------------------------------------------------------------------------

JIT   = JIT
JIT_O = $(OBJ_DIR)/jit.o

JIT_H      = $(JIT).h
JIT_STAT_H = $(JIT)_static.h $(JIT_H) $(AST_H) $(FRONTEND_H) $(BACKEND_H) $(LOG_H) $(ALG_H)
JIT_SRC    = $(JIT).cpp $(JIT_STAT_H)

$(JIT_O): $(JIT_SRC)
	mkdir -p $(OBJ_DIR)
	g++ -c $(CFLAGS) $< -o $@
