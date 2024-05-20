#ifndef FRONTEND_H
#define FRONTEND_H

#include <stdlib.h>
#include "ast.h"

//================================================================================================================================
// FRONTEND
//================================================================================================================================

AST_node *frontend(const char *source_code, size_t *const  var_quantity,
                                            size_t *const func_quantity,
                                            size_t *const  main_func_id);

#endif //FRONTEND_H
