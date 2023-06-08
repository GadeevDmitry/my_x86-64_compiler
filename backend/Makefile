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

AST_H = ../ast/ast.h

#================================================================================================================================
# BACKEND
#================================================================================================================================

#--------------------------------------------------------------------------------------------------------------------------------
# COMMON
#--------------------------------------------------------------------------------------------------------------------------------

backend:
	make $(IR_O)
	make $(IR_TRANSLATOR_O)
	make $(IR_TRANSLATOR_PROG_O)

clean:
	rm -r -f $(OBJ_DIR)/

.PHONY: backend clean

#--------------------------------------------------------------------------------------------------------------------------------
# IR
#--------------------------------------------------------------------------------------------------------------------------------

IR   = IR/IR
IR_O = $(OBJ_DIR)/ir.o

IR_H      = $(IR).h
IR_STAT_H = $(IR)_static.h $(IR_H) $(LOG_H) $(ALG_H)
IR_SRC    = $(IR).cpp $(IR_STAT_H)

$(IR_O): $(IR_SRC)
	mkdir -p $(OBJ_DIR)
	g++ -c $(CFLAGS) $< -o $@

#--------------------------------------------------------------------------------------------------------------------------------
# IR_translator
#--------------------------------------------------------------------------------------------------------------------------------

IR_TRANSLATOR   = IR_translator/IR_translator
IR_TRANSLATOR_O = $(OBJ_DIR)/ir_translator.o

IR_TRANSLATOR_H        = $(IR_TRANSLATOR).h $(AST_H) $(VEC_H)
IR_TRANSLATOR_STATIC_H = $(IR_TRANSLATOR)_static.h $(LOG_H) $(AST_H) $(IR_H) $(IR_TRANSLATOR_H) $(IR_TRANSLATOR_PROG_H)
IR_TRANSLATOR_SRC      = $(IR_TRANSLATOR).cpp $(IR_TRANSLATOR_STATIC_H)

$(IR_TRANSLATOR_O): $(IR_TRANSLATOR_SRC)
	mkdir -p $(OBJ_DIR)
	g++ -c $(CFLAGS) $< -o $@

#--------------------------------------------------------------------------------------------------------------------------------

IR_TRANSLATOR_PROG   = IR_translator/prog_info/prog_info
IR_TRANSLATOR_PROG_O = $(OBJ_DIR)/ir_translator_prog.o

IR_TRANSLATOR_PROG_H      = $(IR_TRANSLATOR_PROG).h $(VEC_H) $(ARR_H)
IR_TRANSLATOR_PROG_STAT_H = $(IR_TRANSLATOR_PROG)_static.h $(LOG_H) $(ALG_H) $(IR_H) $(IR_TRANSLATOR_PROG_H)
IR_TRANSLATOR_PROG_SRC    = $(IR_TRANSLATOR_PROG).cpp $(IR_TRANSLATOR_PROG_STAT_H)

$(IR_TRANSLATOR_PROG_O): $(IR_TRANSLATOR_PROG_SRC)
	mkdir -p $(OBJ_DIR)
	g++ -c $(CFLAGS) $< -o $@
