#include "bin_translator_static.h"

//================================================================================================================================
// BINARY_TRANSLATOR
//================================================================================================================================

array *binary_translator(const vector *const x64, size_t *const main_func_x64_addr)
{
    vec_verify(x64, nullptr);
    log_verify(main_func_x64_addr != nullptr, nullptr);

    binary_info *binary = binary_info_new(x64->size, *main_func_x64_addr);

    translate_general     (binary, x64);
    binary_info_fixup_addr(binary, x64);

    *main_func_x64_addr = binary->main_func_addr;
}

//================================================================================================================================

#define translate_verify                                                                                        \
        {                                                                                                       \
            log_verify(binary  != nullptr, (void) 0);                                                           \
            log_verify(x64_cmd != nullptr, (void) 0);                                                           \
        }

#define no_support                                                                                              \
        {                                                                                                       \
            log_error("this case is not supported yet");                                                        \
            x64_node_dump(x64_cmd);                                                                             \
            log_tab_error_message("====================", "\n");                                                \
            return;                                                                                             \
        }

//--------------------------------------------------------------------------------------------------------------------------------

#define is_reg_only(operand) ((operand).is_reg && !(operand).is_mem && !(operand).is_imm)
#define is_mem(     operand) ((operand).is_mem)
#define is_imm_only(operand) ((operand).is_imm && !(operand).is_mem && !(operand).is_reg)

//--------------------------------------------------------------------------------------------------------------------------------

#define reset()                 binary_node_reset(&cmd);

#define set_operand_pref()      binary_node_set_operand_pref(&cmd)
#define set_address_pref()      binary_node_set_address_pref(&cmd)

#define set_REX(REX)            binary_node_set_REX  (&cmd, REX)
#define set_REX_W()             binary_node_set_REX_W(&cmd)
#define set_REX_R()             binary_node_set_REX_R(&cmd)
#define set_REX_X()             binary_node_set_REX_X(&cmd)
#define set_REX_B()             binary_node_set_REX_B(&cmd)

#define set_opcode(opcode)      binary_node_set_opcode(&cmd, opcode)
#define set_ModRM(mod, reg, rm) binary_node_set_ModRM (&cmd, mod, reg, rm)
#define set_SIB(  scl, ind, bs) binary_node_set_SIB   (&cmd, scl, ind, bs)

#define set_disp32(disp32)      binary_node_set_disp32(&cmd, disp32)
#define set_imm32(  imm32)      binary_node_set_imm32 (&cmd,  imm32)

#define cmd_push()              binary_info_push_cmd  (binary, cmd_num, &cmd)

//--------------------------------------------------------------------------------------------------------------------------------
// translate_general
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_general(binary_info *const binary, const vector *const x64)
{
    log_verify(binary != nullptr, (void) 0);
    log_verify(x64    != nullptr, (void) 0);

    const x64_node *x64_cur = (const x64_node *) vector_begin(x64);
    const x64_node *x64_end = (const x64_node *) vector_end  (x64);

    size_t num = 0UL;

    for (; x64_cur != x64_end; ++x64_cur)
    {
        binary_info_set_x64_node_pc(binary, num);
        translate_x64_node         (binary, x64_cur, num);

        num++;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_x64_node
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_x64_node(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    switch (x64_cmd->type)
    {
        case X64_CMD_ADD  :
        case X64_CMD_SUB  :

        case X64_CMD_OR   :
        case X64_CMD_AND  :
        case X64_CMD_XOR  :

        case X64_CMD_CMP  :
        case X64_CMD_TEST : translate_general_arithmetic(binary, x64_cmd, cmd_num); break;

        case X64_CMD_IMUL :
        case X64_CMD_IDIV : translate_imul_idiv         (binary, x64_cmd, cmd_num); break;

        case X64_CMD_SETcc: translate_setcc             (binary, x64_cmd, cmd_num); break;
        case X64_CMD_Jcc  : translate_jcc               (binary, x64_cmd, cmd_num); break;

        case X64_CMD_CALL : translate_call              (binary, x64_cmd, cmd_num); break;
        case X64_CMD_JMP  :
        case X64_CMD_RET  :

        case X64_CMD_PUSH :
        case X64_CMD_POP  :

        default           : log_assert_verbose(false, "undefind X64_CMD");
                            break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_general_arithmetic(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    if (!is_reg_only($op_1) ||
        !is_reg_only($op_2)) no_support;

    binary_node cmd = {};
    reset();

    switch ($type)
    {
        case X64_CMD_ADD : set_opcode('\x01'); break;
        case X64_CMD_SUB : set_opcode('\x29'); break;

        case X64_CMD_OR  : set_opcode('\x09'); break;
        case X64_CMD_AND : set_opcode('\x21'); break;
        case X64_CMD_XOR : set_opcode('\x31'); break;

        case X64_CMD_CMP : set_opcode('\x39'); break;
        case X64_CMD_TEST: set_opcode('\x85'); break;

        default          : log_assert_verbose(false, "unexpected x64 instruction");
                           break;
    }

    set_REX_W ();
    set_ModRM (3, $op_2.reg, $op_1.reg);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_imul_idiv(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    log_assert(($type == X64_CMD_IMUL) ||
               ($type == X64_CMD_IDIV));

    if (!is_reg_only($op_1)) no_support;

    binary_node cmd = {};
    reset();

    set_REX_W ();
    set_opcode('\xF7');

    if ($type == X64_CMD_IMUL) set_ModRM(3, (BYTE) 5, $op_1.reg);
    else                       set_ModRM(3, (BYTE) 7, $op_1.reg);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_setcc(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;
    log_assert($type == X64_CMD_SETcc);

    if (!is_reg_only($op_1)) no_support;

    binary_node cmd = {};
    reset();

    set_REX   ('\x0F');
    set_opcode(0x90 + (BYTE) $cc);
    set_ModRM (3, (BYTE) 0, $op_1.reg);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_jcc(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;
    log_assert($type == X64_CMD_Jcc);

    if (!is_imm_only($op_1)) no_support;

    binary_node cmd = {};
    reset();

    set_REX   ('\x0F');
    set_opcode(0x80 + (BYTE) $cc);
    set_disp32($op_1.imm);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_call(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;
    log_assert($type == X64_CMD_CALL);

    if (is_mem($op_1)) no_support;

    if (is_imm_only($op_1)) translate_call_rel(binary, x64_cmd, cmd_num);
    else                    translate_call_abs(binary, x64_cmd, cmd_num);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_call_rel(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    log_assert($type == X64_CMD_CALL);
    log_assert(is_imm_only($op_1));

    binary_node cmd = {};
    reset();

    set_opcode('\xE8');
    set_disp32($op_1.imm);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_call_abs(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    log_assert($type == X64_CMD_CALL);
    log_assert(is_reg_only($op_1));

    binary_node cmd = {};
    reset();

    set_opcode('\xFF');
    set_ModRM (3, (BYTE) 2, $op_1.reg);

    cmd_push();
}
