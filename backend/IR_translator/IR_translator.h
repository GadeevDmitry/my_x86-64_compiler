#ifndef IR_TRANSLATOR_H
#define IR_TRANSLATOR_H

#include "../../ast/ast.h"
#include "../../lib/vector/vector.h"

//================================================================================================================================
// IR_TRANSLATOR
//================================================================================================================================

vector *IR_translator(const AST_node *const subtree, const size_t  var_quantity,
                                                     const size_t func_quantity);

#endif //IR_TRANSLATOR_H