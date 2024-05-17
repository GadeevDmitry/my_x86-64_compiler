#ifndef X64_TRANSLATOR_H
#define X64_TRANSLATOR_H

#include <stdlib.h>
#include "../../lib/include/vector.h"

//================================================================================================================================
// X64_TRANSLATOR
//================================================================================================================================

vector *x64_translator(const vector *const IR, size_t *const main_func_ir_addr);

#endif //X64_TRANSLATOR_H
