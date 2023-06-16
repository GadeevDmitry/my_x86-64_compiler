#include "x64_translator_static.h"

//================================================================================================================================
// X64_TRANSLATOR
//================================================================================================================================

vector *x64_translator(const vector *const IR, size_t *const main_func_ir_addr)
{
    vec_verify(IR, nullptr);

    x64_info *x64 = x64_info_new(IR->size, *main_func_ir_addr);

    translate_general  (x64, IR);
    x64_info_fixup_addr(x64);

    *main_func_ir_addr = x64->main_func_addr;
    return x64_info_delete_no_cmds(x64);
}

//================================================================================================================================

#define translate_verify                                                                                                \
        {                                                                                                               \
            log_verify(x64    != nullptr, (void) 0);                                                                    \
            log_verify(IR_cmd != nullptr, (void) 0);                                                                    \
        }

//--------------------------------------------------------------------------------------------------------------------------------

#define cmd_set(type, ...)                   x64_node_ctor(&cmd, type, ##__VA_ARGS__)
#define cmd_push                             x64_info_push_cmd(x64, &cmd)

#define op1_set(is_reg, is_mem, is_imm, ...) x64_node_set_operand_1(&cmd, is_reg, is_mem, is_imm, ##__VA_ARGS__)
#define op2_set(is_reg, is_mem, is_imm, ...) x64_node_set_operand_2(&cmd, is_reg, is_mem, is_imm, ##__VA_ARGS__)

#define op1_reg(reg)                         x64_node_set_operand_1(&cmd, true, false, false, reg)
#define op2_reg(reg)                         x64_node_set_operand_2(&cmd, true, false, false, reg)

#define op1_imm(imm)                         x64_node_set_operand_1(&cmd, false, false, true, imm)
#define op2_imm(imm)                         x64_node_set_operand_2(&cmd, false, false, true, imm)

//--------------------------------------------------------------------------------------------------------------------------------

#define cmd_unary(type, reg)                                                                                            \
        {                                                                                                               \
        cmd_set  (type);                                                                                                \
        op1_reg  (reg );                                                                                                \
        cmd_push;                                                                                                       \
        }

#define cmd_binary(type, reg_1, reg_2)                                                                                  \
        {                                                                                                               \
        cmd_set   (type );                                                                                              \
        op1_reg   (reg_1);                                                                                              \
        op2_reg   (reg_2);                                                                                              \
        cmd_push;                                                                                                       \
        }

#define cmd_setcc(cc, reg)                                                                                              \
        {                                                                                                               \
        cmd_set     (X64_CMD_SETcc, cc);                                                                                \
        op1_reg     (reg);                                                                                              \
        cmd_push;                                                                                                       \
        }

//--------------------------------------------------------------------------------------------------------------------------------
// general
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_general(x64_info *const x64, const vector *const IR)
{
    log_verify(x64 != nullptr, (void) 0);
    log_verify(IR  != nullptr, (void) 0);

    const IR_node *IR_cur = (const IR_node *) vector_begin(IR);
    const IR_node *IR_end = (const IR_node *) vector_end  (IR);

    size_t num = 0UL;

    for (; IR_cur != IR_end; ++IR_cur)
    {
        x64_info_set_ir_node_addr(x64, num);
        translate_IR_node        (x64, IR_cur);

        num++;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_IR_node
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_IR_node(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;

    switch (IR_cmd->type)
    {
        case IR_CMD_ADD       :
        case IR_CMD_SUB       : translate_add_sub    (x64, IR_cmd); break;
        case IR_CMD_MUL       : translate_mul        (x64, IR_cmd); break;
        case IR_CMD_DIV       : translate_div        (x64, IR_cmd); break;

        case IR_CMD_ARE_EQUAL :
        case IR_CMD_NOT_EQUAL :
        case IR_CMD_MORE      :
        case IR_CMD_LESS      :
        case IR_CMD_MORE_EQUAL:
        case IR_CMD_LESS_EQUAL: translate_conditional(x64, IR_cmd); break;

        case IR_CMD_NOT       : translate_not        (x64, IR_cmd); break;

        case IR_CMD_LOG_OR    : translate_log_or     (x64, IR_cmd); break;
        case IR_CMD_LOG_AND   : translate_log_and    (x64, IR_cmd); break;

        case IR_CMD_JMP       : translate_jmp        (x64, IR_cmd); break;
        case IR_CMD_JG        :
        case IR_CMD_JL        :
        case IR_CMD_JE        :
        case IR_CMD_JGE       :
        case IR_CMD_JLE       :
        case IR_CMD_JNE       : translate_jcc        (x64, IR_cmd); break;

        case IR_CMD_PUSH      :
        case IR_CMD_POP       : translate_push_pop   (x64, IR_cmd); break;

        case IR_CMD_CALL      : translate_call       (x64, IR_cmd); break;
        case IR_CMD_RET       : translate_ret        (x64, IR_cmd); break;

        case IR_CMD_IN        :
        case IR_CMD_OUT       : translate_in_out     (x64, IR_cmd); break;

        default               : log_assert_verbose(false, "undefined IR_CMD");
                                break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_add_sub(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;

    log_assert(IR_cmd->type == IR_CMD_ADD ||
               IR_cmd->type == IR_CMD_SUB);

    x64_node cmd = {};

    cmd_unary(X64_CMD_POP, RSI)
    cmd_unary(X64_CMD_POP, RAX)

    if (IR_cmd->type == IR_CMD_ADD) cmd_binary(X64_CMD_ADD, RAX, RSI)
    else                            cmd_binary(X64_CMD_SUB, RAX, RSI)

    cmd_unary(X64_CMD_PUSH, RAX)
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_mul(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_MUL);

    x64_node cmd = {};

    cmd_unary(X64_CMD_POP , RSI)                            // pop rsi  ; second operand
    cmd_unary(X64_CMD_POP , RAX)                            // pop rax  ; first  operand

    cmd_unary(X64_CMD_IMUL, RSI)                            // imul rsi ; rdx:rax = rax * rsi
    cmd_unary(X64_CMD_IDIV, RCX)                            // idiv rcx ; rax = rdx:rax / rcx,
                                                            //          ; где rcx = 10^n необходимо для имитации
                                                            //          ; десятичных дробей

    cmd_unary(X64_CMD_PUSH, RAX)                            // push rax ; result
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_div(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_DIV);

    x64_node cmd = {};

    cmd_unary(X64_CMD_POP , RSI)                            // pop rsi  ; second operand
    cmd_unary(X64_CMD_POP , RAX)                            // pop rax  ; first  operand

    cmd_unary(X64_CMD_IMUL, RCX)                            // imul rcx ; rdx:rax = rax * rcx,
                                                            //          ; где rcx = 10^n необходимо для имитации
                                                            //          ; десятичных дробей
    cmd_unary(X64_CMD_IDIV, RSI)                            // idiv rsi ; rax = rdx:rax / rsi

    cmd_unary(X64_CMD_PUSH, RAX)                            // push rax ; result
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_conditional(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;

    x64_node cmd = {};

    cmd_unary (X64_CMD_POP, RSI)
    cmd_unary (X64_CMD_POP, RAX)

    cmd_binary(X64_CMD_XOR, RBX, RBX)
    cmd_binary(X64_CMD_CMP, RAX, RSI)

    switch (IR_cmd->type)
    {
        case IR_CMD_ARE_EQUAL : cmd_set(X64_CMD_SETcc, X64_cc_E ); break; // sete
        case IR_CMD_NOT_EQUAL : cmd_set(X64_CMD_SETcc, X64_cc_NE); break; // setne
        case IR_CMD_MORE      : cmd_set(X64_CMD_SETcc, X64_cc_G ); break; // setg
        case IR_CMD_LESS      : cmd_set(X64_CMD_SETcc, X64_cc_L ); break; // setl
        case IR_CMD_MORE_EQUAL: cmd_set(X64_CMD_SETcc, X64_cc_GE); break; // setge
        case IR_CMD_LESS_EQUAL: cmd_set(X64_CMD_SETcc, X64_cc_LE); break; // setle

        default               : log_assert_verbose(false, "expected conditional binary operator");
                                break;
    }

    op1_reg(RBX);
    cmd_push;

    cmd_unary(X64_CMD_PUSH, RBX)
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_not(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_NOT);

    x64_node cmd = {};

    cmd_unary (X64_CMD_POP , RAX)

    cmd_binary(X64_CMD_XOR , RBX, RBX)
    cmd_binary(X64_CMD_TEST, RAX, RAX)

    cmd_setcc (X64_cc_E    , RBX)

    cmd_unary (X64_CMD_PUSH, RBX)
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_log_or(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_LOG_OR);

    x64_node cmd = {};

    cmd_unary (X64_CMD_POP , RSI)
    cmd_unary (X64_CMD_POP , RAX)

    cmd_binary(X64_CMD_XOR , RBX, RBX)
    cmd_binary(X64_CMD_OR  , RAX, RSI)

    cmd_setcc (X64_cc_NE   , RBX)

    cmd_unary (X64_CMD_PUSH, RBX)
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_log_and(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_LOG_AND);

    x64_node cmd = {};

    cmd_unary (X64_CMD_POP , RSI)
    cmd_unary (X64_CMD_POP , RAX)

    cmd_binary(X64_CMD_XOR , RBX, RBX)
    cmd_binary(X64_CMD_XOR , RDX, RDX)

    cmd_binary(X64_CMD_TEST, RSI, RSI)
    cmd_setcc (X64_cc_NE   , RBX)

    cmd_binary(X64_CMD_TEST, RAX, RAX)
    cmd_setcc (X64_cc_NE   , RDX)

    cmd_binary(X64_CMD_AND , RBX, RDX)

    cmd_unary (X64_CMD_PUSH, RBX)
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_jmp(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_JMP);

    x64_node cmd = {};

    cmd_set(X64_CMD_JMP);
    op1_imm(IR_cmd->imm_val);
    cmd_push;    
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_jcc(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;

    x64_node cmd = {};

    cmd_unary (X64_CMD_POP, RSI)
    cmd_unary (X64_CMD_POP, RAX)

    cmd_binary(X64_CMD_CMP, RAX, RSI)

    switch (IR_cmd->type)
    {
        case IR_CMD_JG : cmd_set(X64_CMD_Jcc, X64_cc_G ); break;
        case IR_CMD_JL : cmd_set(X64_CMD_Jcc, X64_cc_L ); break;
        case IR_CMD_JE : cmd_set(X64_CMD_Jcc, X64_cc_E ); break;
        case IR_CMD_JGE: cmd_set(X64_CMD_Jcc, X64_cc_GE); break;
        case IR_CMD_JLE: cmd_set(X64_CMD_Jcc, X64_cc_LE); break;
        case IR_CMD_JNE: cmd_set(X64_CMD_Jcc, X64_cc_NE); break;

        default        : log_assert_verbose(false, "expected conditional jump command");
                         break;
    }

    op1_imm(IR_cmd->imm_val);
    cmd_push;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_push_pop(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_PUSH ||
               IR_cmd->type == IR_CMD_POP);

    x64_node cmd = {};

    if (IR_cmd->type == IR_CMD_PUSH) cmd_set(X64_CMD_PUSH);
    else                             cmd_set(X64_CMD_POP );

    op1_set(IR_cmd->is_arg.is_reg_arg,
            IR_cmd->is_arg.is_mem_arg,
            IR_cmd->is_arg.is_imm_arg, 0, 0);

    if (IR_cmd->is_arg.is_reg_arg) x64_operand_set_reg(&(cmd.op_1), (GPR) IR_cmd->reg_num);
    if (IR_cmd->is_arg.is_imm_arg) x64_operand_set_imm(&(cmd.op_1),       IR_cmd->imm_val);

    cmd_push;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_call(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_CALL);

    x64_node cmd = {};

    cmd_set(X64_CMD_CALL);
    op1_imm(IR_cmd->imm_val);
    cmd_push;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_ret(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_RET);

    x64_node cmd = {};

    cmd_set(X64_CMD_RET);
    cmd_push;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_in_out(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_IN ||
               IR_cmd->type == IR_CMD_OUT);

    x64_node cmd = {};

    cmd_set(X64_CMD_CALL);

    if (IR_cmd->type == IR_CMD_IN) op1_reg(R8); // R8 := адрес функции input
    else                           op1_reg(R9); // R9 := адрес функции output
}
