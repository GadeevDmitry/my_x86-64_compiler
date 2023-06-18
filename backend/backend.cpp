#include <stdio.h>
#include <stdlib.h>

#define LOG_NTRACE
#define LOG_NLEAK
#include "../lib/logs/log.h"
#include "../lib/vector/vector.h"

#include "IR_translator/IR_translator.h"
#include "x64_translator/x64_translator.h"
#include "binary_translator/bin_translator.h"

#include "backend.h"

//================================================================================================================================
// BACKEND
//================================================================================================================================

buffer *backend(const AST_node *const ast, const size_t  var_quantity,
                                           const size_t func_quantity, size_t *const main_func_addr,
                                                                       size_t *const glob_var_quantity)
{
    log_verify(ast               != nullptr, nullptr);
    log_verify(main_func_addr    != nullptr, nullptr);
    log_verify(glob_var_quantity != nullptr, nullptr);

    vector *IR  =  IR_translator   (ast, var_quantity, func_quantity, main_func_addr, glob_var_quantity);
    vector *x64 = x64_translator   (IR , main_func_addr);
    buffer *exe = binary_translator(x64, main_func_addr);

    vector_free(IR);
    vector_free(x64);

    return exe;
}
