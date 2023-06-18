CFLAGS  = -D _DEBUG -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -pie -Wlarger-than=8192 -Wstack-usage=8192

#--------------------------------------------------------------------------------------------------------------------------------

TRC_O = lib/logs/trace/trace.o
LOG_O = lib/logs/log.o
ALG_O = lib/algorithm/algorithm.o
STK_O = lib/stack/stack.o
ARR_O = lib/array/array.o

LIB_O = $(TRC_O) $(LOG_O) $(ALG_O) $(STK_O) $(ARR_O)

#--------------------------------------------------------------------------------------------------------------------------------

FRONTEND_O  = frontend/obj/frontend.o
PARSER_O    = frontend/obj/parser.o
TOKENIZER_O = frontend/obj/tokenizer.o
STRUCT_O    = frontend/obj/structs.o

FRONT_O     = $(FRONTEND_O) $(PARSER_O) $(TOKENIZER_O) $(STRUCT_O)

#--------------------------------------------------------------------------------------------------------------------------------

AST_O = ast/obj/ast.o

#--------------------------------------------------------------------------------------------------------------------------------

IR_O            = backend/obj/ir.o
IR_TRANS_O      = backend/obj/ir_translator.o
IR_TRANS_PROG_O = backend/obj/ir_translator_prog.o

X64_O           = backend/obj/x64.o
X64_TRANS_O     = backend/obj/x64_translator.o
X64_INFO_O      = backend/obj/x64_info.o

BIN_O           = backend/obj/bin.o
BIN_TRANS_O     = backend/obj/bin_translator.o
BIN_INFO_O      = backend/obj/bin_info.o

BACKEND_O       = backend/obj/backend.o

BACK_O          = $(IR_O)  $(IR_TRANS_O) $(IR_TRANS_PROG_O) \
                 $(X64_O) $(X64_TRANS_O) $(X64_INFO_O)      \
				 $(BIN_O) $(BIN_TRANS_O) $(BIN_INFO_O)	    \
				 $(BACKEND_O)

#--------------------------------------------------------------------------------------------------------------------------------

JIT_O = JIT/obj/jit.o

#--------------------------------------------------------------------------------------------------------------------------------

jit: JIT/main.cpp
	cd lib      && make all
	cd frontend && make all
	cd ast      && make ast
	cd backend  && make all
	cd JIT      && make JIT

	g++ $(CFLAGS) $< $(LIB_O) $(FRONT_O) $(AST_O) $(BACK_O) $(JIT_O) -o $@

clean:
	rm -f jit

.PHONY: jit clean
