#ifndef BIN_TRANSLATOR_H
#define BIN_TRANSLATOR_H

#include "../../lib/array/array.h"
#include "../../lib/vector/vector.h"

//================================================================================================================================
// BINARY_TRANSLATOR
//================================================================================================================================

array *binary_translator(const vector *const x64, size_t *const main_func_x64_addr);

#endif //BIN_TRANSLATOR_H
