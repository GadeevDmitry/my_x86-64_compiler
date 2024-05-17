#include "bin_info_static.h"

//================================================================================================================================
// BINARY_INFO
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

binary_info *binary_info_new(const size_t x64_size,
                             const size_t main_func_x64_addr)
{
    binary_info *binary_new = (binary_info *) LOG_CALLOC(1, sizeof(binary_info));
    LOG_VERIFY  (binary_new != nullptr, nullptr);

    if (!binary_info_ctor(binary_new, x64_size, main_func_x64_addr)) { LOG_FREE(binary_new); return nullptr; }
    return binary_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_info_ctor(binary_info *const binary, const size_t x64_size,
                                                 const size_t main_func_x64_addr)
{
    LOG_VERIFY(binary != nullptr, false);

    $cmds     = array_new(x64_size, sizeof(binary_node), nullptr, binary_node_dump);
    $cmd_addr = array_new(x64_size, sizeof(size_t     ), nullptr, nullptr);

    $pc        = 0;
    $main_addr = main_func_x64_addr;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

void binary_info_delete(void *const _binary)
{
    binary_info_dtor(_binary);
    LOG_FREE        (_binary);
}

//--------------------------------------------------------------------------------------------------------------------------------

void binary_info_dtor(void *const _binary)
{
    if (_binary == nullptr) return;

    binary_info *const binary = (binary_info *) _binary;

    array_delete($cmds);
    array_delete($cmd_addr);
}

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_info_push_cmd(binary_info *const binary, const size_t cmd_num, const binary_node *const cmd)
{
    LOG_VERIFY(binary != nullptr, false);
    LOG_VERIFY(cmd    != nullptr, false);

    array_set($cmds, cmd_num, cmd);
    $pc += binary_node_get_size(cmd);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_info_set_x64_node_pc(binary_info *const binary, const size_t cmd_num)
{
    LOG_VERIFY(binary != nullptr, false);
    LOG_VERIFY(cmd_num < $cmd_addr->size, false);

    ((size_t *) array_begin($cmd_addr))[cmd_num] = $pc;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

int binary_info_get_x64_node_pc(binary_info *const binary, const size_t cmd_num)
{
    LOG_VERIFY(binary != nullptr, -1);
    LOG_VERIFY(cmd_num < $cmd_addr->size, false);

    return (int) ((size_t *) array_begin($cmd_addr))[cmd_num];
}

//--------------------------------------------------------------------------------------------------------------------------------

buffer *binary_info_get_exe_buff(binary_info *const binary)
{
    LOG_VERIFY(binary != nullptr, nullptr);

    buffer *exe = buffer_new($pc);
    LOG_VERIFY(exe != nullptr, nullptr);

    binary_node *cmd = (binary_node *) array_begin($cmds);
    binary_node *end = (binary_node *) array_end  ($cmds);

    for (; cmd != end; ++cmd)
        binary_node_store(cmd, exe);

    return exe;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_info_fixup_addr(binary_info *const binary, const vector *const x64)
{
    LOG_VERIFY(binary != nullptr, false);
    LOG_VERIFY(x64    != nullptr, false);

    for (size_t cmd_num = 0; cmd_num < $cmds->size; ++cmd_num)
        binary_info_fixup_cmd_addr(binary, x64, cmd_num);

    binary_info_fixup_main_func_addr(binary);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_info_fixup_cmd_addr(binary_info *const binary, const vector *const x64, const size_t cmd_num)
{
    LOG_ASSERT(binary != nullptr);
    LOG_ASSERT(cmd_num < $cmds->size);

    binary_node *bin_cmd = (binary_node *)  array_get($cmds, cmd_num);
    x64_node    *x64_cmd = (   x64_node *) vector_get(x64  , cmd_num);

    if ((x64_cmd->type != X64_CMD_Jcc ) &&
        (x64_cmd->type != X64_CMD_JMP ) &&
        (x64_cmd->type != X64_CMD_CALL)) return;
    if  (x64_cmd->op_1.is_imm == false)  return;

    LOG_VERIFY_VERBOSE(cmd_num != $cmds->size - 1, "last instruction is call or jmp", (void) 0);

    int target_x64 = x64_operand_get_imm(&(x64_cmd->op_1));
    int target_pc  = binary_info_get_x64_node_pc(binary, (size_t) target_x64);
    int   next_pc  = binary_info_get_x64_node_pc(binary, cmd_num + 1);

    binary_node_set_disp32(bin_cmd, target_pc - next_pc);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_info_fixup_main_func_addr(binary_info *const binary)
{
    LOG_ASSERT(binary != nullptr);

    $main_addr = (size_t) binary_info_get_x64_node_pc(binary, $main_addr);
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void binary_info_dump(const void *const _binary, const vector *const x64)
{
    LOG_VERIFY(_binary != nullptr, (void) 0);
    LOG_VERIFY(x64     != nullptr, (void) 0);

    const binary_info *const binary = (const binary_info *) _binary;

    LOG_VERIFY($cmds->size == $cmd_addr->size &&
               $cmds->size == x64->size, (void) 0);

    binary_info_header_dump(binary);
    binary_info_fields_dump(binary, x64);
    binary_info_ending_dump();
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void binary_info_header_dump(const binary_info *const binary)
{
    LOG_ASSERT(binary != nullptr);

    LOG_TAB_SERVICE_MESSAGE("binary_info (addr: %p)\n"
                            "{", "\n",      binary);
    LOG_TAB++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_info_fields_dump(const binary_info *const binary, const vector *const x64)
{
    LOG_ASSERT(binary != nullptr);

    USUAL_FIELD_DUMP("pc     ", "%lu", $pc);
    USUAL_FIELD_DUMP("main pc", "%lu", $main_addr);

    LOG_MESSAGE("\n");

    binary_info_cmds_dump(binary, x64);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_info_cmds_dump(const binary_info *const binary, const vector *const x64)
{
    LOG_ASSERT(binary != nullptr);

    const binary_node *bin_cmd = (const binary_node *)  array_begin($cmds);
    const binary_node *bin_end = (const binary_node *)  array_end  ($cmds);

    const x64_node *x64_cmd = (const x64_node *) vector_begin(x64);
    const size_t   *cmd_pc  = (const size_t   *)  array_begin($cmd_addr);

    size_t cmd_num = 0UL;
    while (bin_cmd != bin_end)
    {
        binary_info_cmd_dump(bin_cmd, x64_cmd, *cmd_pc, cmd_num);
        cmd_num++;

        x64_cmd++;
        bin_cmd++;
        cmd_pc ++;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_info_cmd_dump(const binary_node *const bin_cmd,
                                 const    x64_node *const x64_cmd, const size_t cmd_pc,
                                                                   const size_t cmd_num)
{
    LOG_ASSERT(bin_cmd != nullptr);
    LOG_ASSERT(x64_cmd != nullptr);

    LOG_TAB_SERVICE_MESSAGE("num = %lu\n"
                            "pc  = %lu\n"
                            "{",     "\n", cmd_num, cmd_pc);
    LOG_TAB++;

       x64_node_dump(x64_cmd);
    binary_node_dump(bin_cmd);

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("}", "\n\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void binary_info_ending_dump()
{
    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("}", "\n\n");
}
