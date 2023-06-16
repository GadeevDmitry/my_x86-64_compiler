#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>

#include "../../../lib/array/array.h"
#include "../../../lib/vector/vector.h"

#include "../../x64/x64.h"

//================================================================================================================================
// STRUCT
//================================================================================================================================

struct x64_info
{
    vector *cmds;           ///< массив x64_node
    array  *IR_node_addr;   ///< массив адресов IR_node в cmds
    size_t  main_func_addr; ///< адрес главной функции в cmds
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

x64_info *x64_info_new           (                      const size_t IR_size, const size_t main_func_ir_addr);
bool      x64_info_ctor          (x64_info *const  x64, const size_t IR_size, const size_t main_func_ir_addr);
void      x64_info_delete        (void     *const _x64);
void      x64_info_dtor          (void     *const _x64);
vector   *x64_info_delete_no_cmds(void     *const _x64);
vector   *x64_info_dtor_no_cmds  (void     *const _x64);

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

bool x64_info_push_cmd        (x64_info *const x64, const x64_node *const cmd);

bool x64_info_set_ir_node_addr(x64_info *const x64, const size_t IR_node_num);
int  x64_info_get_ir_node_addr(x64_info *const x64, const size_t IR_node_num);

bool x64_info_fixup_addr      (x64_info *const x64);

#endif //STRUCTS_H
