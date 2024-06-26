#ifndef PROG_INFO_H
#define PROG_INFO_H

#include <stdlib.h>

#include "lib/include/array.h"
#include "lib/include/vector.h"

#include "IR/IR.h"

//================================================================================================================================
// STRUCT
//================================================================================================================================

struct prog_info
{
    vector *IR;             ///< массив IR_node

    array  * func_addr;     ///< массив адресов функций (адрес функции - номер IR_node ее первой инструкции)
    array  * glob_addr;     ///< массив адресов глобальных переменных
    array  *  loc_addr;     ///< массив стеков адресов локальных переменных относительно фрейма текущей функции

    stack  *scope;          ///< стек адресов начал областей видимости
    size_t  relative;       ///< номер свободной ячейки для следующей переменной
};

//================================================================================================================================
// FUNCTION DECLARATION
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

bool       prog_info_ctor(prog_info *const prog, const size_t var_quantity, const size_t func_quantity);
prog_info *prog_info_new (                       const size_t var_quantity, const size_t func_quantity);

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

void    prog_info_dtor        (void *const _prog);
void    prog_info_delete      (void *const _prog);

vector *prog_info_dtor_no_IR  (void *const _prog);
vector *prog_info_delete_no_IR(void *const _prog);

//--------------------------------------------------------------------------------------------------------------------------------
// var declaration
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_set_global_var_addr(prog_info *const prog, const size_t var_ind);
bool prog_info_add_local_var_addr (prog_info *const prog, const size_t var_ind);

//--------------------------------------------------------------------------------------------------------------------------------
// var access
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_get_global_var_addr(prog_info *const prog, const size_t var_ind, size_t *const var_addr);
bool prog_info_get_local_var_addr (prog_info *const prog, const size_t var_ind, size_t *const var_addr);

//--------------------------------------------------------------------------------------------------------------------------------
// func declaration
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_func_begin(prog_info *const prog, const size_t func_ind);
bool prog_info_func_end  (prog_info *const prog);

//--------------------------------------------------------------------------------------------------------------------------------
// func access
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_get_func_addr(prog_info *const prog, const size_t func_ind, size_t *const func_addr);

//--------------------------------------------------------------------------------------------------------------------------------
// scope
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_scope_in (prog_info *const prog);
bool prog_info_scope_out(prog_info *const prog);

//--------------------------------------------------------------------------------------------------------------------------------
// other
//--------------------------------------------------------------------------------------------------------------------------------

size_t prog_info_create_command      (      prog_info *const prog, const IR_CMD type, const bool is_reg, const bool is_mem, const bool is_imm, ...);
size_t prog_info_get_next_command_num(const prog_info *const prog);
bool   prog_info_fixup_jmp_addr      (      prog_info *const prog, const size_t jmp_cmd_num, const size_t label_num);

#endif //PROG_INFO_H
