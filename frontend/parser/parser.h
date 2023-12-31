#ifndef PARSER_H
#define PARSER_H

#include "../../ast/ast.h"

//================================================================================================================================
// PARSER
//================================================================================================================================

AST_node *parser(vector *token_arr, size_t *const main_func_id,
                                    size_t *const var_quantity,
                                    size_t *const func_quantity);

#endif //PARSER_H
