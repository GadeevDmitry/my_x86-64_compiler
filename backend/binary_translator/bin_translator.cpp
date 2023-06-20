#include "bin_translator_static.h"

//================================================================================================================================
// BINARY_TRANSLATOR
//================================================================================================================================

buffer *binary_translator(const vector *const x64, size_t *const main_func_x64_addr)
{
    vec_verify(x64, nullptr);
    log_verify(main_func_x64_addr != nullptr, nullptr);

    binary_info *binary = binary_info_new(x64->size, *main_func_x64_addr);

    translate_general     (binary, x64);
    binary_info_fixup_addr(binary, x64);

    buffer *exe = binary_info_get_exe_buff(binary);
    *main_func_x64_addr = binary->main_func_addr;

    binary_info_delete(binary);

    return exe;
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
#define is_imm_only(operand) ((operand).is_imm && !(operand).is_mem && !(operand).is_reg)
#define is_mem(     operand) ((operand).is_mem)

//--------------------------------------------------------------------------------------------------------------------------------

#define cmd_init()              binary_node cmd = {}; binary_node_reset(&cmd);

#define set_operand_pref()      binary_node_set_operand_pref(&cmd)
#define set_address_pref()      binary_node_set_address_pref(&cmd)

#define set_REX(REX)            binary_node_set_REX  (&cmd, REX)
#define set_REX_W()             binary_node_set_REX_W(&cmd)
#define set_REX_R()             binary_node_set_REX_R(&cmd)
#define set_REX_X()             binary_node_set_REX_X(&cmd)
#define set_REX_B()             binary_node_set_REX_B(&cmd)

#define set_opcode(opcode)      binary_node_set_opcode(&cmd, (BYTE) opcode)
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
                            #define translate(opcode) translate_general_arithmetic(binary, x64_cmd, cmd_num, opcode)
        case X64_CMD_ADD  : translate('\x01'); break;
        case X64_CMD_SUB  : translate('\x29'); break;

        case X64_CMD_OR   : translate('\x09'); break;
        case X64_CMD_AND  : translate('\x21'); break;
        case X64_CMD_XOR  : translate('\x31'); break;

        case X64_CMD_CMP  : translate('\x39'); break;
        case X64_CMD_TEST : translate('\x85'); break;
                            #undef translate
        case X64_CMD_IMUL :
        case X64_CMD_IDIV : translate_imul_idiv(binary, x64_cmd, cmd_num); break;

        case X64_CMD_SETcc: translate_setcc    (binary, x64_cmd, cmd_num); break;
        case X64_CMD_Jcc  : translate_jcc      (binary, x64_cmd, cmd_num); break;

        case X64_CMD_CALL : translate_call     (binary, x64_cmd, cmd_num); break;
        case X64_CMD_JMP  : translate_jmp      (binary, x64_cmd, cmd_num); break;
        case X64_CMD_RET  : translate_ret      (binary, x64_cmd, cmd_num); break;

        case X64_CMD_PUSH : translate_push     (binary, x64_cmd, cmd_num); break;
        case X64_CMD_POP  : translate_pop      (binary, x64_cmd, cmd_num); break;

        default           : log_assert_verbose(false, "undefind X64_CMD");
                            break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

static void translate_general_arithmetic(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num, const BYTE opcode)
{
    translate_verify;

    if (!is_reg_only($op_1) ||
        !is_reg_only($op_2)) no_support;

    cmd_init();

    set_REX_W ();
    set_opcode(opcode);
    set_ModRM (3, $op_2.reg, $op_1.reg);

    cmd_push();
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_imul_idiv(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    log_assert(($type == X64_CMD_IMUL) ||
               ($type == X64_CMD_IDIV));

    if (!is_reg_only($op_1)) no_support;

    cmd_init();

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

    cmd_init();

    set_REX   ('\x0F');
    set_opcode(0x90 + (BYTE) $cond_code);
    set_ModRM (3, (BYTE) 0, $op_1.reg);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_jcc(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;
    log_assert($type == X64_CMD_Jcc);

    if (!is_imm_only($op_1)) no_support;

    cmd_init();

    set_REX   ('\x0F');
    set_opcode(0x80 + (BYTE) $cond_code);
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

    cmd_init();

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

    cmd_init();

    set_opcode('\xFF');
    set_ModRM (3, (BYTE) 2, $op_1.reg);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_jmp(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;
    log_assert($type == X64_CMD_JMP);

    if (!is_imm_only($op_1)) no_support;

    cmd_init();

    set_opcode('\xE9');
    set_disp32($op_1.imm);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_ret(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;
    log_assert($type == X64_CMD_RET);

    cmd_init();
    set_opcode('\xC3');
    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_push(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;
    log_assert($type == X64_CMD_PUSH);

    if      (is_imm_only($op_1)) translate_push_imm(binary, x64_cmd, cmd_num);
    else if (is_reg_only($op_1)) translate_push_reg(binary, x64_cmd, cmd_num);
    else                         translate_push_mem(binary, x64_cmd, cmd_num);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_push_imm(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    log_assert($type == X64_CMD_PUSH);
    log_assert(is_imm_only($op_1));

    cmd_init();

    set_opcode('\x68');
    set_imm32 ($op_1.imm);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_push_reg(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    log_assert($type == X64_CMD_PUSH);
    log_assert(is_reg_only($op_1));

    cmd_init();

    set_opcode('\xFF');
    set_ModRM (3, (BYTE) 6, $op_1.reg);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_push_mem(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    log_assert($type == X64_CMD_PUSH);
    log_assert(is_mem($op_1));

    cmd_init();

    set_opcode('\xFF');
    set_ModRM (2, (BYTE) 6, (BYTE) 4);  // SIB + disp32

    translate_push_pop_mem(binary, x64_cmd, &cmd);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_pop(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    log_assert($type == X64_CMD_POP);
    log_assert(!is_imm_only($op_1));

    if (is_reg_only($op_1)) translate_pop_reg(binary, x64_cmd, cmd_num);
    else if (is_mem($op_1)) translate_pop_mem(binary, x64_cmd, cmd_num);
    else                    no_support;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_pop_reg(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    log_assert($type == X64_CMD_POP);
    log_assert(is_reg_only($op_1));

    cmd_init();

    set_opcode('\x8F');
    set_ModRM (3, (BYTE) 0, $op_1.reg);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_pop_mem(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num)
{
    translate_verify;

    log_assert($type == X64_CMD_POP);
    log_assert(is_mem($op_1));

    cmd_init();

    set_opcode('\x8F');
    set_ModRM (2, (BYTE) 0, (BYTE) 4);

    translate_push_pop_mem(binary, x64_cmd, &cmd);

    cmd_push();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_push_pop_mem(binary_info *const binary, const x64_node *const x64_cmd, binary_node *const bin_cmd)
{
    log_verify(binary  != nullptr, (void) 0);
    log_verify(x64_cmd != nullptr, (void) 0);
    log_verify(bin_cmd != nullptr, (void) 0);

    log_assert(($type == X64_CMD_PUSH) ||
               ($type == X64_CMD_POP ));
    log_assert(is_mem($op_1));

    BYTE scale = 0;

    switch ($op_1.scale_factor)
    {
        case  1: scale = 0; break;
        case  2: scale = 1; break;
        case  4: scale = 2; break;
        case  8: scale = 3; break;

        default: log_assert_verbose(false, "impossible scale factor");
                 break;
    }

    if ($op_1.is_reg == false) binary_node_set_SIB(bin_cmd, scale, (BYTE) 4 /* none */, R10);
    else                       binary_node_set_SIB(bin_cmd, scale, $op_1.reg          , R10); // R10 := начало блока RAM

    binary_node_set_disp32(bin_cmd, $op_1.scale_factor * $op_1.imm);
}
