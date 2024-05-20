#include <stdio.h>

#include "frontend.hpp"
#include "parser/parser.hpp"
#include "frontend/tokenizer/tokenizer.h"

#include "lib/include/log.h"
#include "lib/include/buffer.h"
#include "lib/include/vector.h"

//================================================================================================================================

void frontend(const char *source_code)
{
    LOG_VERIFY(source_code != nullptr, (void) 0);

    buffer source = {};
    if (!buffer_ctor(&source, source_code))
    {
        fprintf(stderr, BASH_COLOR_RED "ERROR: " BASH_COLOR_WHITE "can't open \"%s\" with source code\n", source_code);
        return;
    }

    vector *token_arr = tokenizer(&source);
    if (token_arr == nullptr)
    {
        buffer_dtor(&source);
        return;
    }

    std::unique_ptr<LLVM_AST_module_node> result = module_parser(source_code, token_arr);
    if (result) result->dump();

    buffer_dtor  (&source);
    vector_delete(token_arr);
}
