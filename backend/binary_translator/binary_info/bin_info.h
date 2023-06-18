#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>

#include "../../../lib/array/array.h"
#include "../../../lib/vector/vector.h"

#include "../../binary/bin.h"


//================================================================================================================================
// BINARY_INFO
//================================================================================================================================

struct binary_info
{
    array  *cmds;           ///< массив binary_node
    array  *cmd_addr;       ///< массив адресов инструкций в байтах

    size_t  pc;             ///< адрес текущей инструкции в байтах
    size_t  main_func_addr; ///< адрес главной функции в cmds
    size_t  offset;         ///< постоянное смещение для адресов
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

binary_info *binary_info_new   (                            const size_t x64_size, const size_t main_func_x64_addr, const size_t offset = 0UL);
bool         binary_info_ctor  (binary_info *const  binary, const size_t x64_size, const size_t main_func_x64_addr, const size_t offset = 0UL);
void         binary_info_delete(void        *const _binary);
void         binary_info_dtor  (void        *const _binary);

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

bool    binary_info_push_cmd       (binary_info *const binary, const size_t cmd_num, const binary_node *const cmd);

bool    binary_info_set_x64_node_pc(binary_info *const binary, const size_t x64_node_num);
int     binary_info_get_x64_node_pc(binary_info *const binary, const size_t x64_node_num);
buffer *binary_info_get_exe_buff   (binary_info *const binary);

bool    binary_info_fixup_addr     (binary_info *const binary, const vector *const x64);

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void binary_info_dump(const void *const _binary, const vector *const x64);

#endif //STRUCTS_H
