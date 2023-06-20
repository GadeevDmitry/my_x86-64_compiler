#include "x64_translator_static.h"

//================================================================================================================================
// X64_TRANSLATOR
//================================================================================================================================

vector *x64_translator(const vector *const IR, size_t *const main_func_ir_addr)
{
    vec_verify(IR, nullptr);
    log_verify(main_func_ir_addr != nullptr, nullptr);

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

#define cmd_init                             x64_node cmd = {}
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
        cmd_init;                                                                                                       \
        cmd_set(type);                                                                                                  \
        op1_reg(reg );                                                                                                  \
        cmd_push;                                                                                                       \
        }

#define cmd_binary(type, reg_1, reg_2)                                                                                  \
        {                                                                                                               \
        cmd_init;                                                                                                       \
        cmd_set(type );                                                                                                 \
        op1_reg(reg_1);                                                                                                 \
        op2_reg(reg_2);                                                                                                 \
        cmd_push;                                                                                                       \
        }

#define cmd_setcc(cc, reg)                                                                                              \
        {                                                                                                               \
        cmd_init;                                                                                                       \
        cmd_set(X64_CMD_SETcc, cc);                                                                                     \
        op1_reg(reg);                                                                                                   \
        cmd_push;                                                                                                       \
        }

#define cmd_jcc(cc, imm)                                                                                                \
        {                                                                                                               \
        cmd_init;                                                                                                       \
        cmd_set(X64_CMD_Jcc, cc);                                                                                       \
        op1_imm(imm);                                                                                                   \
        cmd_push;                                                                                                       \
        }

//--------------------------------------------------------------------------------------------------------------------------------

#define ADD( reg_1, reg_2) cmd_binary(X64_CMD_ADD , reg_1, reg_2)
#define SUB( reg_1, reg_2) cmd_binary(X64_CMD_SUB , reg_1, reg_2)
#define IMUL(reg)          cmd_unary (X64_CMD_IMUL, reg)
#define IDIV(reg)          cmd_unary (X64_CMD_IDIV, reg)

#define OR(  reg_1, reg_2) cmd_binary(X64_CMD_OR  , reg_1, reg_2)
#define AND( reg_1, reg_2) cmd_binary(X64_CMD_AND , reg_1, reg_2)
#define XOR( reg_1, reg_2) cmd_binary(X64_CMD_XOR , reg_1, reg_2)

#define CMP( reg_1, reg_2) cmd_binary(X64_CMD_CMP , reg_1, reg_2)
#define TEST(reg_1, reg_2) cmd_binary(X64_CMD_TEST, reg_1, reg_2)

#define PUSH(reg)          cmd_unary (X64_CMD_PUSH, reg)
#define POP( reg)          cmd_unary (X64_CMD_POP , reg)

//--------------------------------------------------------------------------------------------------------------------------------

#define reg_save translate_caller_save(x64)
#define reg_load translate_caller_load(x64)

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
        case IR_CMD_SQRT      : translate_sqrt       (x64, IR_cmd); break;

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

        case IR_CMD_IN        : translate_in         (x64, IR_cmd); break;
        case IR_CMD_OUT       : translate_out        (x64, IR_cmd); break;

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

    POP(RSI)
    POP(RAX)

    if (IR_cmd->type == IR_CMD_ADD) ADD(RAX, RSI)
    else                            SUB(RAX, RSI)

    PUSH(RAX)
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_mul(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_MUL);

    POP(RSI)    // second operand
    POP(RAX)    // first  operand

    IMUL(RSI)   // rdx:rax = rax * rsi
    IDIV(RCX)   // rax = rdx:rax / rcx,
                // где rcx = 10^n необходимо для имитации
                // десятичных дробей

    PUSH(RAX)   // result
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_div(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_DIV);

    POP(RSI)    // second operand
    POP(RAX)    // first  operand

    IMUL(RCX)   // rdx:rax = rax * rcx,
                // где rcx = 10^n необходимо для имитации
                // десятичных дробей
    IDIV(RSI)   // rax = rdx:rax / rsi

    PUSH(RAX)   // result
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_sqrt(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;

    POP(RDI)                     // аргумент из стека

    reg_save;
    cmd_unary(X64_CMD_CALL, R11) // R11 := адрес функции sqrt
    reg_load;

    PUSH(RAX)                    // возвращаемое значение в стек
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_conditional(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;

    POP(RSI)
    POP(RAX)

    XOR(RBX, RBX)
    CMP(RAX, RSI)

    switch (IR_cmd->type)
    {
        case IR_CMD_ARE_EQUAL : cmd_setcc(X64_cc_E , RBX); break;
        case IR_CMD_NOT_EQUAL : cmd_setcc(X64_cc_NE, RBX); break;
        case IR_CMD_MORE      : cmd_setcc(X64_cc_G , RBX); break;
        case IR_CMD_LESS      : cmd_setcc(X64_cc_L , RBX); break;
        case IR_CMD_MORE_EQUAL: cmd_setcc(X64_cc_GE, RBX); break;
        case IR_CMD_LESS_EQUAL: cmd_setcc(X64_cc_LE, RBX); break;

        default               : log_assert_verbose(false, "expected conditional binary operator");
                                break;
    }

    PUSH(RBX)
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_not(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_NOT);

    POP(RAX)

    XOR (RBX, RBX)
    TEST(RAX, RAX)

    cmd_setcc(X64_cc_E, RBX)

    PUSH(RBX)
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_log_or(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_LOG_OR);

    POP(RSI)
    POP(RAX)

    XOR(RBX, RBX)
    OR (RAX, RSI)

    cmd_setcc(X64_cc_NE, RBX)

    PUSH(RBX)
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_log_and(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_LOG_AND);

    POP(RSI)
    POP(RAX)

    XOR(RBX, RBX)
    XOR(RDX, RDX)

    TEST(RSI, RSI)
    cmd_setcc(X64_cc_NE, RBX)

    TEST(RAX, RAX)
    cmd_setcc(X64_cc_NE, RDX)

    AND(RBX, RDX)

    PUSH(RBX)
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_jmp(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_JMP);

    cmd_init;
    cmd_set(X64_CMD_JMP);
    op1_imm(IR_cmd->imm_val);
    cmd_push;    
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_jcc(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;

    POP(RSI)
    POP(RAX)

    CMP(RAX, RSI)

    int imm = IR_cmd->imm_val;
    switch   (IR_cmd->type)
    {
        case IR_CMD_JG : cmd_jcc(X64_cc_G , imm); break;
        case IR_CMD_JL : cmd_jcc(X64_cc_L , imm); break;
        case IR_CMD_JE : cmd_jcc(X64_cc_E , imm); break;
        case IR_CMD_JGE: cmd_jcc(X64_cc_GE, imm); break;
        case IR_CMD_JLE: cmd_jcc(X64_cc_LE, imm); break;
        case IR_CMD_JNE: cmd_jcc(X64_cc_NE, imm); break;

        default        : log_assert_verbose(false, "expected conditional jump command");
                         break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_push_pop(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_PUSH ||
               IR_cmd->type == IR_CMD_POP);

    cmd_init;

    if (IR_cmd->type == IR_CMD_PUSH) cmd_set(X64_CMD_PUSH);
    else                             cmd_set(X64_CMD_POP );

    op1_set(IR_cmd->is_arg.is_reg_arg,
            IR_cmd->is_arg.is_mem_arg,
            IR_cmd->is_arg.is_imm_arg, 0, 0);

    if (IR_cmd->is_arg.is_reg_arg) x64_operand_set_reg(&(cmd.op_1), (GPR) IR_cmd->reg_num);
    if (IR_cmd->is_arg.is_mem_arg) x64_operand_set_scl(&(cmd.op_1), (char) sizeof(type_t));
    if (IR_cmd->is_arg.is_imm_arg) x64_operand_set_imm(&(cmd.op_1),       IR_cmd->imm_val);

    cmd_push;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_call(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_CALL);

    cmd_init;
    cmd_set(X64_CMD_CALL);
    op1_imm(IR_cmd->imm_val);
    cmd_push;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_ret(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_RET);

    cmd_init;
    cmd_set(X64_CMD_RET);
    cmd_push;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_in(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_IN);

    reg_save;
    cmd_unary(X64_CMD_CALL,  R8); // R8 := адрес функции input
    reg_load;

    PUSH(RAX)                     // возвращаемое значение в стек
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_out(x64_info *const x64, const IR_node *const IR_cmd)
{
    translate_verify;
    log_assert(IR_cmd->type == IR_CMD_OUT);

    POP(RDI)                      // аргумент из стека

    reg_save;
    cmd_unary(X64_CMD_CALL,  R9); // R9 := адрес функции output
    reg_load;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_caller_save(x64_info *const x64)
{
    log_verify(x64 != nullptr, (void) 0);

    PUSH(RDI)
    PUSH(RSI)
    PUSH(RDX)
    PUSH(RCX)
    PUSH(R8 )
    PUSH(R9 )
    PUSH(R10)
    PUSH(R11)
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_caller_load(x64_info *const x64)
{
    log_verify(x64 != nullptr, (void) 0);

    POP(R11)
    POP(R10)
    POP(R9 )
    POP(R8 )
    POP(RCX)
    POP(RDX)
    POP(RSI)
    POP(RDI)
}
