#include <stdio.h>
#include <stdlib.h>

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "../lib/include/log.h"
#include "../lib/include/vector.h"

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
    LOG_VERIFY(ast               != nullptr, nullptr);
    LOG_VERIFY(main_func_addr    != nullptr, nullptr);
    LOG_VERIFY(glob_var_quantity != nullptr, nullptr);

    vector *IR  =  IR_translator   (ast, var_quantity, func_quantity, main_func_addr, glob_var_quantity);
    vector *x64 = x64_translator   (IR , main_func_addr);
    buffer *exe = binary_translator(x64, main_func_addr);

    vector_delete(IR);
    vector_delete(x64);

    return exe;
}
