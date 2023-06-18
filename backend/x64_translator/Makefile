#================================================================================================================================
# COMMON
#================================================================================================================================

FLAGS_DEBUG  = -D _DEBUG -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -pie -Wlarger-than=8192 -Wstack-usage=8192
FLAGS_NDEBUG = -g -std=c++20 -Wall

CFLAGS  = $(FLAGS_NDEBUG)

OBJ_DIR = ../obj

#================================================================================================================================
# LIB
#================================================================================================================================

TRC	  = ../../lib/logs/trace/trace
LOG   = ../../lib/logs/log
ALG   = ../../lib/algorithm/algorithm
STK   = ../../lib/stack/stack
VEC   = ../../lib/vector/vector
ARR   = ../../lib/array/array

TRC_H = $(TRC).h
LOG_H = $(LOG).h $(LOG)_settings.h $(LOG)_def.h
ALG_H = $(ALG).h $(LOG_H)
STK_H = $(STK).h $(LOG_H) $(ALG_H)
VEC_H = $(VEC).h $(STK_H)
ARR_H = $(ARR).h $(LOG_H) $(ALG_H)

#--------------------------------------------------------------------------------------------------------------------------------

IR_H  = ../IR/IR.h
X64_H = ../x64/x64.h

#================================================================================================================================
# X64_TRANSLATOR
#================================================================================================================================

X64_translator:
	make $(X64_INFO_O)
	make $(X64_TRANSLATOR_O)

clean:
	clean -r -f $(OBJ_DIR)/

.PHONY: X64_translator clean

#--------------------------------------------------------------------------------------------------------------------------------

X64_TRANSLATOR   = x64_translator
X64_TRANSLATOR_O = $(OBJ_DIR)/x64_translator.o

X64_TRANSLATOR_H      = $(X64_TRANSLATOR).h $(VEC_H)
X64_TRANSLATOR_STAT_H = $(X64_TRANSLATOR)_static.h $(X64_TRANSLATOR_H) $(X64_INFO_H) $(IR_H) $(X64_H) $(LOG_H)
X64_TRANSLATOR_SRC    = $(X64_TRANSLATOR).cpp $(X64_TRANSLATOR_STAT_H)

$(X64_TRANSLATOR_O): $(X64_TRANSLATOR_SRC)
	mkdir -p $(OBJ_DIR)
	g++ -c $(CFLAGS) $< -o $@

#--------------------------------------------------------------------------------------------------------------------------------
# X64_INFO
#--------------------------------------------------------------------------------------------------------------------------------

X64_INFO   = x64_info/x64_info
X64_INFO_O = $(OBJ_DIR)/x64_info.o

X64_INFO_H      = $(X64_INFO).h $(X64_H) $(ARR_H) $(VEC_H)
X64_INFO_STAT_H = $(X64_INFO)_static.h $(X64_INFO_H) $(LOG_H)
X64_INFO_SRC    = $(X64_INFO).cpp $(X64_INFO_STAT_H)

$(X64_INFO_O): $(X64_INFO_SRC)
	mkdir -p $(OBJ_DIR)
	g++ -c $(CFLAGS) $< -o $@
