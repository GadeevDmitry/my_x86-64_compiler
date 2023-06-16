#include "x64_info_static.h"

//================================================================================================================================
// X64_INFO
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

x64_info *x64_info_new(const size_t IR_size,
                       const size_t main_func_ir_addr)
{
    x64_info  *x64_new = (x64_info *) log_calloc(1, sizeof(x64_info));
    log_verify(x64_new != nullptr, nullptr);

    if (!x64_info_ctor(x64_new, IR_size, main_func_ir_addr)) { log_free(x64_new); return nullptr; }
    return x64_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool x64_info_ctor(x64_info *const x64, const size_t IR_size,
                                        const size_t main_func_ir_addr)
{
    log_verify(x64 != nullptr, false);

    $cmds         = vector_new(         sizeof(x64_node), nullptr, nullptr, x64_node_dump);
    $IR_node_addr =  array_new(IR_size, sizeof(size_t  ));
    $main_addr    = main_func_ir_addr; // изменится после fixup адресов

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void x64_info_delete(void *const _x64)
{
    x64_info_dtor(_x64);
    log_free     (_x64);
}

//--------------------------------------------------------------------------------------------------------------------------------

void x64_info_dtor(void *const _x64)
{
    if (_x64 == nullptr) return;

    x64_info *const x64 = (x64_info *) _x64;

    vector_free($cmds);
    array_free ($IR_node_addr);
}

//--------------------------------------------------------------------------------------------------------------------------------

vector *x64_info_delete_no_cmds(void *const _x64)
{
    vector *cmds = x64_info_dtor_no_cmds(_x64);
    log_free(_x64);

    return cmds;
}

//--------------------------------------------------------------------------------------------------------------------------------

vector *x64_info_dtor_no_cmds(void *const _x64)
{
    if (_x64 == nullptr) return nullptr;

    x64_info *const x64 = (x64_info *) _x64;

    array_free($IR_node_addr);
    return $cmds;
}

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

bool x64_info_push_cmd(x64_info *const x64, const x64_node *const cmd)
{
    log_verify(x64 != nullptr, false);
    log_verify(cmd != nullptr, false);

    vector_push_back($cmds, cmd);
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool x64_info_set_ir_node_addr(x64_info *const x64, const size_t IR_node_num)
{
    log_verify(x64 != nullptr, false);
    log_verify(IR_node_num < $IR_node_addr->size, false);

    ((size_t *) array_begin($IR_node_addr))[IR_node_num] = $cmds->size;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

int x64_info_get_ir_node_addr(x64_info *const x64, const size_t IR_node_num)
{
    log_verify(x64 != nullptr, -1);
    log_verify(IR_node_num < $IR_node_addr->size, -1);

    return (int) ((size_t *) array_begin($IR_node_addr))[IR_node_num];
}

//--------------------------------------------------------------------------------------------------------------------------------

bool x64_info_fixup_addr(x64_info *const x64)
{
    log_verify(x64 != nullptr, false);

    x64_node *node_cur = (x64_node *) vector_begin($cmds);
    x64_node *node_end = (x64_node *) vector_end  ($cmds);

    for (; node_cur != node_end; ++node_cur)
        x64_info_fixup_cmd_addr(x64, node_cur);

    x64_info_fixup_main_func_addr(x64);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void x64_info_fixup_cmd_addr(x64_info *const x64, x64_node *const node)
{
    log_assert(x64  != nullptr);
    log_assert(node != nullptr);

    if ((node->type != X64_CMD_Jcc ) &&
        (node->type != X64_CMD_JMP ) &&
        (node->type != X64_CMD_CALL)) return;

    if (node->op_1.is_imm == false) return;

    int  IR_node_target = x64_operand_get_imm(&(node->op_1));
    int x64_node_target = x64_info_get_ir_node_addr(x64, (size_t) IR_node_target);

    x64_operand_set_imm(&(node->op_1), x64_node_target);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void x64_info_fixup_main_func_addr(x64_info *const x64)
{
    log_assert(x64 != nullptr);

    $main_addr = (size_t) x64_info_get_ir_node_addr(x64, $main_addr);
}
