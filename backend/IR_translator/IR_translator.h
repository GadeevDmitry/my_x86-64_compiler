#ifndef IR_TRANSLATOR_H
#define IR_TRANSLATOR_H

#include "../../ast/ast.h"
#include "../../lib/include/vector.h"

//================================================================================================================================
// IR_TRANSLATOR
//================================================================================================================================

vector *IR_translator(const AST_node *const tree, const size_t  var_quantity,
                                                  const size_t func_quantity, size_t *const main_func_id,
                                                                              size_t *const glob_var_quantity);
#endif //IR_TRANSLATOR_H
