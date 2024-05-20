#ifndef BIN_TRANSLATOR_H
#define BIN_TRANSLATOR_H

#include "lib/include/buffer.h"
#include "lib/include/vector.h"

//================================================================================================================================
// BINARY_TRANSLATOR
//================================================================================================================================

buffer *binary_translator(const vector *const x64, size_t *const main_func_x64_addr);

#endif //BIN_TRANSLATOR_H
