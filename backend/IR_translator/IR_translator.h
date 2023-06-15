#ifndef IR_TRANSLATOR_H
#define IR_TRANSLATOR_H

#include "../../ast/ast.h"
#include "../../lib/vector/vector.h"

//================================================================================================================================
// IR_TRANSLATOR
//================================================================================================================================

vector *IR_translator(const AST_node *const tree, const size_t  main_func_id,
                                                  const size_t  var_quantity,
                                                  const size_t func_quantity, size_t *const main_func_addr);

#endif //IR_TRANSLATOR_H
