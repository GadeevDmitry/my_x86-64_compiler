#include "IR_translator_static.h"

//================================================================================================================================
// IR_TRANSLATOR
//================================================================================================================================

vector *IR_translator(const AST_node *const subtree, const size_t  var_quantity,
                                                     const size_t func_quantity)
{
    log_verify(subtree != nullptr, nullptr);

    prog_info *prog = prog_info_new(var_quantity, func_quantity);

    translate_general(prog, subtree);

    vector *IR = prog_info_delete_no_IR(prog);
    return  IR;
}

//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// AST_node DSL
//--------------------------------------------------------------------------------------------------------------------------------

#define $type       (subtree->type)

#define $L          (subtree->left)
#define $R          (subtree->right)
#define $P          (subtree->prev)

#define $imm_int    (subtree->value.imm_int)
#define $var_ind    (subtree->value.var_ind)
#define $func_ind   (subtree->value.func_ind)
#define $operator   (subtree->value.op)

//--------------------------------------------------------------------------------------------------------------------------------

#define ast_node_fictional_handler(translate_func)                          \
        if ($L != nullptr) translate_func(prog, $L);                        \
        if ($R != nullptr) translate_func(prog, $R);                        \
        return;

#define ast_node_wrong_type_handler                                         \
        log_verify_verbose(false, "wrong AST_node type", (void) 0);         \
        return;

#define ast_node_wrong_operator_handler                                     \
        log_verify_verbose(false, "wrong AST_node operator", (void) 0);     \
        return;

//--------------------------------------------------------------------------------------------------------------------------------
// prog_info DSL
//--------------------------------------------------------------------------------------------------------------------------------

#define $rel (prog->relative)

//--------------------------------------------------------------------------------------------------------------------------------
// translate_... DSL
//--------------------------------------------------------------------------------------------------------------------------------

#define translate_verify                                                    \
        {                                                                   \
        log_verify(prog    != nullptr, (void) 0);                           \
        log_verify(subtree != nullptr, (void) 0);                           \
        }

//--------------------------------------------------------------------------------------------------------------------------------

#define create_command(type, is_reg, is_mem, is_imm, ...)                   \
   (IR_node_ctor(&cmd, type, is_reg, is_mem, is_imm, ##__VA_ARGS__),        \
    prog_info_create_command(prog, &cmd))

#define create_command_no_args(type) create_command(type, false, false, false)
#define next_command() prog_info_get_next_command_num(prog)

//--------------------------------------------------------------------------------------------------------------------------------

#define memory_frame_in                                                     \
        {                                                                   \
        create_command(IR_CMD_PUSH, true , false, false, RBP);              \
        create_command(IR_CMD_PUSH, false, false, true , (int) $rel);       \
        create_command(IR_CMD_ADD , false, false, false);                   \
        create_command(IR_CMD_POP , true , false, false, RBP);              \
        } /* RBP += relative */

#define memory_frame_out                                                    \
        {                                                                   \
        create_command(IR_CMD_PUSH, true , false, false, RBP);              \
        create_command(IR_CMD_PUSH, false, false, true , (int) $rel);       \
        create_command(IR_CMD_SUB , false, false, false);                   \
        create_command(IR_CMD_POP , true , false, false, RBP);              \
        } /* RBP -= relative */

//--------------------------------------------------------------------------------------------------------------------------------

#define in_out_variable(var_ind, var_addr, ir_cmd)                                                                                      \
    if      (prog_info_get_local_var_addr (prog, var_ind, &(var_addr))) create_command(ir_cmd, true , true, true, RBP, (int) var_addr); \
    else if (prog_info_get_global_var_addr(prog, var_ind, &(var_addr))) create_command(ir_cmd, false, true, true,      (int) var_addr); \
    else                                                                                                                                \
    {                                                                                                                                   \
        log_error("variable %lu is not declared yet\n", var_ind);                                                                       \
        return;                                                                                                                         \
    }

#define push_variable(var_ind, var_addr) in_out_variable(var_ind, var_addr, IR_CMD_PUSH)
#define  pop_variable(var_ind, var_addr) in_out_variable(var_ind, var_addr, IR_CMD_POP )

//--------------------------------------------------------------------------------------------------------------------------------
// translate_general
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_general(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    switch ($type)
    {
        case AST_NODE_FICTIONAL: ast_node_fictional_handler(translate_general);

        case AST_NODE_DECL_VAR : prog_info_set_global_var_addr(prog, $var_ind);
                                 break;

        case AST_NODE_DECL_FUNC: prog_info_func_begin(prog, $func_ind);

                                 if ($L != nullptr) translate_args     (prog, $L);
                                 if ($R != nullptr) translate_operators(prog, $R);

                                 prog_info_func_end(prog);
                                 break;

        default                : ast_node_wrong_type_handler;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_args
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_args(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    switch ($type)
    {
        case AST_NODE_FICTIONAL: ast_node_fictional_handler(translate_args);

        case AST_NODE_DECL_VAR : prog_info_add_local_var_addr(prog, $var_ind);
                                 break;

        default                : ast_node_wrong_type_handler;
    }

    if ($L != nullptr) translate_args(prog, $L);
    if ($R != nullptr) translate_args(prog, $R);
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_operators
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_operators(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    switch ($type)
    {
        case AST_NODE_FICTIONAL      : ast_node_fictional_handler(translate_operators);
        case AST_NODE_DECL_VAR       : prog_info_add_local_var_addr(prog, $var_ind);       break;

        case AST_NODE_OPERATOR_IF    : translate_if                       (prog, subtree); break;
        case AST_NODE_OPERATOR_WHILE : translate_while                    (prog, subtree); break;
        case AST_NODE_OPERATOR_RETURN: translate_return                   (prog, subtree); break;
        case AST_NODE_CALL_FUNC      : translate_func_call_ret_val_ignored(prog, subtree); break;

        case AST_NODE_OPERATOR       : translate_operator_independent     (prog, subtree); break;

        default                      : ast_node_wrong_type_handler;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_if
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_if(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    log_assert($type == AST_NODE_OPERATOR_IF);
    log_verify($L != nullptr, (void) 0);
    log_verify($R != nullptr, (void) 0);

    IR_node cmd = {};

    translate_expression(prog, $L);                                         // <condition>

                      create_command(IR_CMD_PUSH, false, false, true, 0);   // push 0
    size_t jmp_else = create_command(IR_CMD_JE  , false, false, true, 0);   // je else

    if ($R->left != nullptr)                                                // <case then>
    {
        prog_info_scope_in (prog);
        translate_operators(prog, $R->left);
        prog_info_scope_out(prog);
    }

    size_t jmp_end    = create_command(IR_CMD_JMP, false, false, true, 0);  // jmp end
    size_t label_else =   next_command();                                   // else:

    if ($R->right != nullptr)                                               // <case else>
    {
        prog_info_scope_in (prog);
        translate_operators(prog, $R->right);
        prog_info_scope_out(prog);
    }

    size_t label_end = next_command();                                      // end:

    prog_info_fixup_jmp_addr(prog, jmp_else, label_else);
    prog_info_fixup_jmp_addr(prog, jmp_end , label_end );
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_while
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_while(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    log_assert($type == AST_NODE_OPERATOR_WHILE);
    log_verify($L != nullptr, (void) 0);

    IR_node cmd = {};
    size_t label_cond = next_command();                                     // cond:

    translate_expression(prog, $L);                                         // <condition>

                     create_command(IR_CMD_PUSH, false, false, true, 0);    // push 0
    size_t jmp_end = create_command(IR_CMD_JE  , false, false, true, 0);    // je end

    if ($R != nullptr)                                                      // <cycle body>
    {
        prog_info_scope_in (prog);
        translate_operators(prog, $R);
        prog_info_scope_out(prog);
    }

    create_command(IR_CMD_JMP, false, false, true, (int) label_cond);       // jmp cond
    size_t label_end = next_command();                                      // end:

    prog_info_fixup_jmp_addr(prog, jmp_end, label_end);
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_func_call
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_func_call_ret_val_ignored(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;
    log_assert($type == AST_NODE_CALL_FUNC);

    IR_node cmd = {};

    translate_func_call_ret_val_used(prog, subtree);
    create_command_no_args(IR_CMD_POP); // удаление возвращаемого значения функции из стека
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_func_call_ret_val_used(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;
    log_assert($type == AST_NODE_CALL_FUNC);

    IR_node cmd = {};
    size_t  func_addr = 0UL;

    if (!prog_info_get_func_addr(prog, $func_ind, &func_addr))
    {
        log_error("function %lu is not declared yet\n", $func_ind);
        return;
    }

    if ($L != nullptr)
    {
        size_t param_quantity = translate_params(prog, $L);

        for (size_t param = 0; param < param_quantity; ++param)
            create_command(IR_CMD_POP, true, true, true, RBP, (int) ($rel + param));
    }

    memory_frame_in;                                            // RBP += relative
    create_command(IR_CMD_CALL, false, false, true, func_addr); // call func_addr
    memory_frame_out;                                           // RBP -= relative
}

//--------------------------------------------------------------------------------------------------------------------------------

static size_t translate_params(prog_info *const prog, const AST_node *const subtree)
{
    log_verify(prog    != nullptr, 0UL);
    log_verify(subtree != nullptr, 0UL);

    switch ($type)
    {
        case AST_NODE_FICTIONAL: {
                                 size_t param_quantity = 0UL;

                                 if ($R != nullptr) param_quantity += translate_params(prog, $R); // обработка параметров в обратном порядке
                                 if ($L != nullptr) param_quantity += translate_params(prog, $L); // в стеке параметры будут в прямом порядке

                                 return param_quantity;
                                 }

        default                : translate_expression(prog, subtree);
                                 return 1UL;
    }

    return 0UL;
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_return
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_return(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    log_assert($type == AST_NODE_OPERATOR_RETURN);
    log_verify($L != nullptr, (void) 0);

    translate_expression(prog, $L);

    IR_node cmd = {};
    create_command_no_args(IR_CMD_RET);
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_independent_operator
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_operator_independent(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;
    log_assert($type == AST_NODE_OPERATOR);

    switch ($operator)
    {
        case AST_OPERATOR_INPUT     : translate_input                 (prog, subtree); break;
        case AST_OPERATOR_OUTPUT    : translate_output                (prog, subtree); break;
        case AST_OPERATOR_ASSIGNMENT: translate_assignment_independent(prog, subtree); break;

        default                     : ast_node_wrong_operator_handler;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_input
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_input(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    log_assert($type     == AST_NODE_OPERATOR );
    log_assert($operator == AST_OPERATOR_INPUT);
    log_verify($L != nullptr, (void) 0);

    IR_node cmd = {};
    create_command_no_args(IR_CMD_IN);

    size_t var_addr = 0UL;

    log_verify  ($L->type == AST_NODE_VARIABLE, (void) 0);
    pop_variable($L->value.var_ind, var_addr);
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_output
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_output(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    log_assert($type     == AST_NODE_OPERATOR  );
    log_assert($operator == AST_OPERATOR_OUTPUT);
    log_verify($L != nullptr, (void) 0);

    translate_expression(prog, $L);

    IR_node cmd = {};
    create_command_no_args(IR_CMD_OUT);
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_assignment
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_assignment_independent(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    log_assert($type     == AST_NODE_OPERATOR);
    log_assert($operator == AST_OPERATOR_ASSIGNMENT);

    log_verify($L != nullptr, (void) 0);
    log_verify($R != nullptr, (void) 0);

    translate_expression(prog, $R);

    IR_node cmd = {};
    size_t  var_addr = 0UL;

    log_verify  ($L->type == AST_NODE_VARIABLE, (void) 0);
    pop_variable($L->value.var_ind, var_addr);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void translate_assignment_dependent(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    log_assert($type     == AST_NODE_OPERATOR);
    log_assert($operator == AST_OPERATOR_ASSIGNMENT);

    log_verify($L != nullptr, (void) 0);
    log_verify($R != nullptr, (void) 0);

    translate_expression(prog, $R);

    IR_node cmd = {};
    size_t  var_addr = 0UL;

    log_verify   ($L->type == AST_NODE_VARIABLE, (void) 0);
    pop_variable ($L->value.var_ind, var_addr);
    push_variable($L->value.var_ind, var_addr);
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_expression
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_expression(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    switch ($type)
    {
        case AST_NODE_IMM_INT  : translate_immediate_int_operand (prog, subtree); break;
        case AST_NODE_VARIABLE : translate_variable_operand      (prog, subtree); break;
        case AST_NODE_CALL_FUNC: translate_func_call_ret_val_used(prog, subtree); break;
        case AST_NODE_OPERATOR : translate_operator_dependent    (prog, subtree); break;

        default                : ast_node_wrong_type_handler;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_immediate_int_operand
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_immediate_int_operand(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    log_assert($type == AST_NODE_IMM_INT );
    log_verify($L    == nullptr, (void) 0);
    log_verify($R    == nullptr, (void) 0);

    IR_node cmd = {};
    create_command(IR_CMD_PUSH, false, false, true, $imm_int);
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_variable_operand
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_variable_operand(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    log_assert($type == AST_NODE_VARIABLE);
    log_verify($L    == nullptr, (void) 0);
    log_verify($R    == nullptr, (void) 0);

    IR_node cmd = {};
    size_t  var_addr = 0UL;

    push_variable($var_ind, var_addr);
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_operator_dependent
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_operator_dependent(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;
    log_assert($type == AST_NODE_OPERATOR);

    switch ($operator)
    {
        case AST_OPERATOR_ASSIGNMENT: translate_assignment_dependent(prog, subtree); break;
        case AST_OPERATOR_NOT       : translate_operator_unary      (prog, subtree); break;
        default                     : translate_operator_binary     (prog, subtree); break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_operator_unary
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_operator_unary(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    log_assert($type == AST_NODE_OPERATOR);
    log_verify($L    != nullptr, (void) 0);

    IR_node cmd = {};

    translate_expression(prog, $L);

    switch ($operator)
    {
        case AST_OPERATOR_NOT: create_command_no_args(IR_CMD_NOT); break;
        default              : ast_node_wrong_operator_handler;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// translate_operator_binary
//--------------------------------------------------------------------------------------------------------------------------------

static void translate_operator_binary(prog_info *const prog, const AST_node *const subtree)
{
    translate_verify;

    log_assert($type == AST_NODE_OPERATOR);
    log_verify($L    != nullptr, (void) 0);
    log_verify($R    != nullptr, (void) 0);

    IR_node cmd = {};

    translate_expression(prog, $L);
    translate_expression(prog, $R);

    switch ($operator)
    {
        case AST_OPERATOR_ADD       : create_command_no_args(IR_CMD_ADD);            break;
        case AST_OPERATOR_SUB       : create_command_no_args(IR_CMD_SUB);            break;
        case AST_OPERATOR_MUL       : create_command_no_args(IR_CMD_MUL);            break;
        case AST_OPERATOR_DIV       : create_command_no_args(IR_CMD_DIV);            break;

        case AST_OPERATOR_ARE_EQUAL : create_command_no_args(IR_CMD_ARE_EQUAL);      break;
        case AST_OPERATOR_NOT_EQUAL : create_command_no_args(IR_CMD_NOT_EQUAL);      break;
        case AST_OPERATOR_MORE      : create_command_no_args(IR_CMD_MORE);           break;
        case AST_OPERATOR_MORE_EQUAL: create_command_no_args(IR_CMD_MORE_EQUAL);     break;
        case AST_OPERATOR_LESS      : create_command_no_args(IR_CMD_LESS);           break;
        case AST_OPERATOR_LESS_EQUAL: create_command_no_args(IR_CMD_LESS_EQUAL);     break;

        case AST_OPERATOR_LOG_OR    : create_command_no_args(IR_CMD_LOG_OR);         break;
        case AST_OPERATOR_LOG_AND   : create_command_no_args(IR_CMD_LOG_AND);        break;

        default                     : ast_node_wrong_operator_handler;
    }
}