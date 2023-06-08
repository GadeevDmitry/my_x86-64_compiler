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
# FRONTEND
#================================================================================================================================

#--------------------------------------------------------------------------------------------------------------------------------
# COMMON
#--------------------------------------------------------------------------------------------------------------------------------

COMMON_H = common.h $(LOG_H) $(ALG_H) $(STK_H) $(VEC_H)

frontend:
	make $(TOKENIZER_O)
	make $(PARSER_O)
	make $(FRONTEND_O)

clean:
	rm -r -f $(OBJ_DIR)/

.PHONY: frontend clean

#--------------------------------------------------------------------------------------------------------------------------------

FRONTEND   = frontend
FRONTEND_O = $(OBJ_DIR)/frontend.o

FRONTEND_H   = $(FRONTEND).h $(AST_H)
FRONTEND_SRC = $(FRONTEND).cpp $(FRONTEND_H) $(LOG_H) $(ALG_H) $(VEC_H) $(TOKENIZER_H) $(PARSER_H) $(AST_H)

$(FRONTEND_O): $(FRONTEND_SRC)
	mkdir -p $(OBJ_DIR)
	g++ -c $(CFLAGS) $< -o $@

#--------------------------------------------------------------------------------------------------------------------------------
# TOKENIZER
#--------------------------------------------------------------------------------------------------------------------------------

TOKENIZER   = tokenizer/tokenizer
TOKENIZER_O = $(OBJ_DIR)/tokenizer.o

TOKENIZER_H      = $(TOKENIZER).h        $(VEC_H)
TOKENIZER_STAT_H = $(TOKENIZER)_static.h $(TOKENIZER_H) $(COMMON_H)
TOKENIZER_SRC    = $(TOKENIZER).cpp      $(TOKENIZER_STAT_H)

$(TOKENIZER_O): $(TOKENIZER_SRC)
	mkdir -p $(OBJ_DIR)
	g++ -c $(CFLAGS) $< -o $@

#--------------------------------------------------------------------------------------------------------------------------------
# PARSER
#--------------------------------------------------------------------------------------------------------------------------------

PARSER   = parser/parser
PARSER_O = $(OBJ_DIR)/parser.o

PARSER_H      = $(PARSER).h        $(AST_H)
PARSER_STAT_H = $(PARSER)_static.h $(COMMON_H) $(PARSER_H) $(TOKENIZER_H)
PARSER_SRC    = $(PARSER).cpp      $(PARSER_STAT_H)

$(PARSER_O): $(PARSER_SRC)
	mkdir -p $(OBJ_DIR)
	g++ -c $(CFLAGS) $< -o $@
