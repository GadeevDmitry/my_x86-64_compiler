#ifndef STRUCTS_STATIC_H
#define STRUCTS_STATIC_H

#include <stdio.h>
#include <stdlib.h>

#define LOG_NTRACE
#include "../../../lib/logs/log.h"

#include "x64_info.h"

//================================================================================================================================
// STRUCT
//================================================================================================================================

#define $cmds           (x64->cmds)
#define $IR_node_addr   (x64->IR_node_addr)
#define $main_addr      (x64->main_func_addr)

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

static void x64_info_fixup_cmd_addr      (x64_info *const x64, x64_node *const node);
static void x64_info_fixup_main_func_addr(x64_info *const x64);

#endif //STRUCTS_STATIC_H
