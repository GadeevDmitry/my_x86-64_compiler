#ifndef STRUCTS_STATIC_H
#define STRUCTS_STATIC_H

#include <stdio.h>
#include <stdlib.h>

#include "../../../lib/algorithm/algorithm.h"

#include "bin_info.h"

//================================================================================================================================
// BINARY_INFO
//================================================================================================================================

#define $cmds       (binary->cmds)
#define $cmd_addr   (binary->cmd_addr)

#define $pc         (binary->pc)
#define $main_addr  (binary->main_func_addr)

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

static void binary_info_fixup_cmd_addr      (binary_info *const binary, const vector *const x64, const size_t cmd_num);
static void binary_info_fixup_main_func_addr(binary_info *const binary);

#endif //STRUCTS_STATIC_H
