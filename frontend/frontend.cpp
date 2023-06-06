#include <stdio.h>
#include <stdlib.h>

#define LOG_NTRACE
#define LOG_NLEAK
#include "../lib/logs/log.h"
#include "../lib/algorithm/algorithm.h"
#include "../lib/vector/vector.h"

#include "tokenizer/tokenizer.h"
#include "parser/parser.h"
#include "../ast/ast.h"

#include "frontend.h"

//================================================================================================================================
// FRONTEND
//================================================================================================================================

AST_node *frontend(const char *source_code, size_t *const main_func_id,
                                            size_t *const var_quantity,
                                            size_t *const func_quantity)
{
    log_verify(source_code != nullptr, nullptr);

    buffer *source = buffer_new(source_code);
    if (source == nullptr)
    {
        log_error("can't open \"%s\" with source code\n", source_code);
        return nullptr;
    }

    vector   *token_arr = tokenizer(source); if (token_arr == nullptr) return nullptr;
    AST_node *result    = parser   (token_arr, main_func_id, var_quantity, func_quantity);

    buffer_free(source);
    vector_free(token_arr);

    return result;
}
