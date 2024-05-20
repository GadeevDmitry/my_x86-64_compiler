#ifndef BACKEND_H
#define BACKEND_H

#include "lib/include/buffer.h"
#include "ast/ast.h"

//================================================================================================================================
// BACKEND
//================================================================================================================================

buffer *backend(const AST_node *const ast, const size_t  var_quantity,
                                           const size_t func_quantity, size_t *const main_func_addr,
                                                                       size_t *const glob_var_quantity);

#endif //BACKEND_H
