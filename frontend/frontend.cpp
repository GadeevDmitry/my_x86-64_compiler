#include <stdio.h>
#include <stdlib.h>

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "../lib/include/log.h"
#include "../lib/include/algorithm.h"
#include "../lib/include/vector.h"

#include "tokenizer/tokenizer.h"
#include "parser/parser.h"

#include "frontend.h"

//================================================================================================================================
// FRONTEND
//================================================================================================================================

AST_node *frontend(const char *source_code, size_t *const  var_quantity,
                                            size_t *const func_quantity,
                                            size_t *const  main_func_id)
{
    LOG_VERIFY(source_code   != nullptr, nullptr);
    LOG_VERIFY( var_quantity != nullptr, nullptr);
    LOG_VERIFY(func_quantity != nullptr, nullptr);
    LOG_VERIFY(main_func_id  != nullptr, nullptr);

    buffer *source = buffer_new(source_code);
    if (source == nullptr)
    {
        fprintf(stderr, BASH_COLOR_RED "ERROR: " BASH_COLOR_WHITE "can't open \"%s\" with source code\n", source_code);
        return nullptr;
    }

    vector   *token_arr = tokenizer(source); if (token_arr == nullptr) return nullptr;
    AST_node *result    = parser   (token_arr, main_func_id, var_quantity, func_quantity);

    buffer_delete(source);
    vector_delete(token_arr);

    return result;
}
