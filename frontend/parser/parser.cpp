#include "parser_static.h"

//================================================================================================================================
// PARSER
//================================================================================================================================

AST_node *parser(vector *token_arr, size_t *const main_func_id,
                                    size_t *const var_quantity,
                                    size_t *const func_quantity)
{
$i
$   vec_verify(token_arr, nullptr);
    log_verify(main_func_id  != nullptr, nullptr);
    log_verify(var_quantity  != nullptr, nullptr);
    log_verify(func_quantity != nullptr, nullptr);

$   prog_info      *program  = prog_info_new(MAIN_FUNC_NAME);
$   token_arr_pass *tkn_pass = token_arr_pass_new(token_arr);
    AST_node       *result   = nullptr;

$   parse_general(program, tkn_pass, &result);

    *main_func_id  = program->main_func_id;
    *var_quantity  = program->var_storage->size;
    *func_quantity = program->func_storage->size;

$        prog_info_delete(program);
$   token_arr_pass_delete(tkn_pass);

$o return result;
}

//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// MACRO
//--------------------------------------------------------------------------------------------------------------------------------

#define $tkn_pos (tkn_pass->arr_pos)
#define $tkn_end (tkn_pass->arr_end)

#define next      { $ token_arr_pass_next     (tkn_pass); }
#define reset     { $ token_arr_pass_reset    (tkn_pass, tkn_entry); }
#define is_passed     token_arr_pass_is_passed(tkn_pass)

//--------------------------------------------------------------------------------------------------------------------------------

#define parse_start $i                                                                                                          \
        log_verify(prog     != nullptr, false);                                                                                 \
        log_verify(tkn_pass != nullptr, false);                                                                                 \
        log_verify(subtree  != nullptr, false);                                                                                 \
        *subtree = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------

#define parse_success { $o return true; }
#define parse_fail                                                                                                              \
        {                                                                                                                       \
        $  AST_tree_delete(*subtree); *subtree = nullptr;                                                                       \
        $o return false;                                                                                                        \
        }

#define parse_error(message, ...)                                                                                               \
        {                                                                                                                       \
        fprintf(stderr, "line %3lu: " BASH_COLOR_RED "ERROR: " BASH_COLOR_WHITE message "\n", $tkn_pos->line, ##__VA_ARGS__);   \
        parse_fail                                                                                                              \
        }

//--------------------------------------------------------------------------------------------------------------------------------

#define try_parse(parse_func, ...) { $ if (!parse_func(__VA_ARGS__)) parse_fail }

/**
*   Параметр op_type должен быть равен соответствующему параметру op_type в макросе token_op_is_smth в файле tokenizer.h.
*/
#define try_skip_separator(op_type, error_message)                                                                              \
    $   if (!is_passed && token_op_is_##op_type($tkn_pos)) next                                                                 \
        else parse_error(error_message)

/**
*   Параметр token_key должен быть равен соответствующему параметру token_key в макросе token_key_is_smth в файле tokenizer.h.
*/
#define try_skip_keyword(token_key, error_message)                                                                              \
    $   if (!is_passed && token_key_is_##token_key($tkn_pos)) next                                                              \
        else parse_error(error_message)

//--------------------------------------------------------------------------------------------------------------------------------
// parse_general
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_general(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    const token *tkn_entry = $tkn_pos;

    AST_node *child   = nullptr;
    AST_node *adapter = nullptr;

$  *subtree = AST_node_new(AST_NODE_FICTIONAL);
    adapter = *subtree;

$   while (!is_passed)
    {
        tkn_entry = $tkn_pos;
        try_parse(parse_name_decl, prog, tkn_pass, &child)

        if      (!is_passed && token_op_is_comma_point   ($tkn_pos)) { reset; try_parse(parse_var_decl_independent, prog, tkn_pass, &child) }
        else if (!is_passed && token_op_is_l_scope_circle($tkn_pos)) { reset; try_parse(parse_func_decl           , prog, tkn_pass, &child) }
        else    parse_error("expected variable or function declaration")

$       adapter = AST_tree_hang_by_adapter(adapter, child);
    }

    if (prog->main_func_id == -1UL) parse_error("main function is not exist")
    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_name_decl(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    try_skip_keyword(int, "expected type before function or variable declaration")

$   if (is_passed || !token_type_is_name(      $tkn_pos)) parse_error("expected name of new variable or function")
$   if (!prog_info_is_name_decl_possible(prog, $tkn_pos)) parse_error("name is already declared")

    next;
    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_var_decl
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_var_decl_independent(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    try_parse(parse_var_decl, prog, tkn_pass, subtree)
    try_skip_separator(comma_point, "expected \';\' after variable declaration")

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_var_decl(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    try_skip_keyword(int, "expected type before variable declaration")

$   if (is_passed || !token_type_is_name(      $tkn_pos)) parse_error("expected name of new variable")
$   if (!prog_info_is_name_decl_possible(prog, $tkn_pos)) parse_error("name is already declared")

$   *subtree = AST_node_new(AST_NODE_DECL_VAR, prog_info_var_push(prog, $tkn_pos));

    next;
    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_func_decl
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_func_decl(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    try_skip_keyword(int, "expected type before function declaration")

$   if (is_passed || !token_type_is_name(      $tkn_pos)) parse_error("expected name of new function")
$   if (!prog_info_is_name_decl_possible(prog, $tkn_pos)) parse_error("name is already declared")

    func_info func = {};
$   func_info_ctor(&func, $tkn_pos);
    next;

    AST_node *child = nullptr;

    try_parse(parse_args, prog, tkn_pass, &child, &func)
$  *subtree = AST_node_new(AST_NODE_DECL_FUNC, prog_info_func_push(prog, &func));
$   AST_node_hang_left(*subtree, child);

    try_parse(parse_func_body, prog, tkn_pass, &child)
$   AST_node_hang_right(*subtree, child);

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_args(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree, func_info *const func)
{
    parse_start;
    log_verify(func != nullptr, false);

    size_t args_quantity = 0UL;

    try_skip_separator(l_scope_circle, "expected \'(\' after function name")
$   prog_info_scope_open(prog);

    try_parse(parse_args_only, prog, tkn_pass, subtree, &args_quantity)
$   func_info_set_arg(func, args_quantity);

    try_skip_separator(r_scope_circle, "expected \')\' after function arguments");

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_args_only(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree, size_t *const args_quantity)
{
    parse_start;
    log_verify(args_quantity != nullptr, false);

    AST_node *arg_cur  = nullptr;
    AST_node *arg_next = nullptr;
    size_t    arg_num  = 0UL;

$   if (is_passed || !token_key_is_int($tkn_pos)) parse_success;

    try_parse(parse_var_decl, prog, tkn_pass, &arg_cur)
   *subtree = arg_cur;
    arg_num++;

$   while (!is_passed && token_op_is_comma($tkn_pos))
    {
        next;
        try_parse(parse_var_decl, prog, tkn_pass, &arg_next)

        AST_node_hang_left(arg_cur, arg_next);
        arg_cur = arg_next;
        arg_num++;
    }

   *args_quantity = arg_num;
    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_func_body(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    try_skip_separator(l_scope_figure, "expected \'{\' before function operators")
    try_parse(parse_operators, prog, tkn_pass, subtree)
    try_skip_separator(r_scope_figure, "expected \'}\' after function operators")

$   prog_info_scope_close(prog);

$   if (!prog_info_func_exit(prog)) parse_error("no return operator at the end of the function")
    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_operators
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_operators(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    AST_node *child   = nullptr;
    AST_node *adapter = nullptr;

$  *subtree = AST_node_new(AST_NODE_FICTIONAL);
    adapter = *subtree;

$   while (!is_passed)
    {
        if      (token_type_is_name($tkn_pos)) try_parse(parse_name_access_independent, prog, tkn_pass, &child)
        else if (token_type_is_key ($tkn_pos)) try_parse(parse_syntactic_unit         , prog, tkn_pass, &child)
        else    break;

        adapter = AST_tree_hang_by_adapter(adapter, child);
    }

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_name_access_independent(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    const token *const tkn_entry = $tkn_pos;

    try_parse(parse_name_access, prog, tkn_pass, subtree)

$   if      (!is_passed && token_op_is_assignment    ($tkn_pos)) { reset; try_parse(parse_assignment_independent, prog, tkn_pass, subtree) }
    else if (!is_passed && token_op_is_l_scope_circle($tkn_pos)) { reset; try_parse(parse_func_call_independent , prog, tkn_pass, subtree) }
    else    parse_error("undefined name access operator")

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_name_access_dependent(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    const token *const tkn_entry = $tkn_pos;

    try_parse(parse_name_access, prog, tkn_pass, subtree)

$   if (!is_passed && token_op_is_l_scope_circle($tkn_pos)) { reset; try_parse(parse_func_call, prog, tkn_pass, subtree); }
    else                                                    { reset; try_parse(parse_lvalue   , prog, tkn_pass, subtree); }

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_name_access(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

$   if (is_passed || !token_type_is_name  (      $tkn_pos)) parse_error("expected name of an existing variable or function")
$   if (!prog_info_is_name_access_possible(prog, $tkn_pos)) parse_error("no variable or function with the same name exist")

    next;
    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_syntactic_unit(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

$   if (is_passed || !token_type_is_key($tkn_pos)) parse_error("expected keyword at the beginning of the syntactic unit")

$   switch ($tkn_pos->value.key)
    {
        case KEY_IF    : try_parse(parse_if                  , prog, tkn_pass, subtree) break;
        case KEY_WHILE : try_parse(parse_while               , prog, tkn_pass, subtree) break;
        case KEY_INPUT : try_parse(parse_input               , prog, tkn_pass, subtree) break;
        case KEY_OUTPUT: try_parse(parse_output              , prog, tkn_pass, subtree) break;
        case KEY_RETURN: try_parse(parse_return              , prog, tkn_pass, subtree) break;
        case KEY_INT   : try_parse(parse_var_decl_independent, prog, tkn_pass, subtree) break;
        case KEY_ELSE  : parse_error("operator can't start with keyword \"else\"")      break;

        case KEY_UNDEF :
        default        : log_assert_verbose(false, "undefined token key_type");         break;
    }

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_assignment
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_assignment_independent(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    try_parse(parse_assignment, prog, tkn_pass, subtree);
    try_skip_separator(comma_point, "expected \';\' after assignment operator")

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_assignment(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    AST_node *child = nullptr;

    try_parse(parse_lvalue, prog, tkn_pass, &child)
$  *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ASSIGNMENT);
$   AST_node_hang_left(*subtree, child);

    try_skip_separator(assignment, "expected \'=\' after variable access in assignment operator")

    try_parse(parse_rvalue, prog, tkn_pass, &child)
$   AST_node_hang_right(*subtree, child);

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_lvalue(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

$   if (is_passed || !token_type_is_name($tkn_pos)) parse_error("expected name of variable")
$   if (!prog_info_is_var_exist   (prog, $tkn_pos)) parse_error("no variable with the same name exist")

    *subtree = AST_node_new(AST_NODE_VARIABLE, prog_info_get_var_index(prog, $tkn_pos));

    next;
    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_func_call
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_func_call_independent(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    try_parse(parse_func_call, prog, tkn_pass, subtree)
    try_skip_separator(comma_point, "expected \';\' after function call operator")

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_func_call(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

$   if (is_passed || !token_type_is_name   ($tkn_pos)) parse_error("expected name of function at the beginning of the function call operator")
$   if (!prog_info_is_func_name_exist(prog, $tkn_pos)) parse_error("no function with the same name exist")

    func_info func = {};
$   func_info_ctor(&func, $tkn_pos);
    next;

    AST_node *child = nullptr;

    try_parse(parse_params, prog, tkn_pass, &child, &func)
$  *subtree = AST_node_new(AST_NODE_CALL_FUNC, prog_info_get_func_index(prog, &func));
$   AST_node_hang_left(*subtree, child);

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_params(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree, func_info *const func)
{
    parse_start;
    log_verify(func != nullptr, false);

    size_t params_quantity = 0UL;

    try_skip_separator(l_scope_circle, "expected \'(\' after function name")
    try_parse(parse_params_only, prog, tkn_pass, subtree, &params_quantity)
    try_skip_separator(r_scope_circle, "expected \')\' after parameters")

$   func_info_set_arg(func, params_quantity);

$   if (!prog_info_is_func_exist(prog, func)) parse_error("wrong parameters quantity")
    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_params_only(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree, size_t *const params_quantity)
{
    parse_start;
    log_verify(params_quantity != nullptr, false);

    AST_node *adapter = nullptr;
    AST_node *child   = nullptr;
    size_t param_num  = 0UL;

$   if (!is_passed && token_op_is_r_scope_circle($tkn_pos)) parse_success;

   *subtree = AST_node_new(AST_NODE_FICTIONAL);
    adapter = *subtree;

    try_parse(parse_rvalue, prog, tkn_pass, &child)
$   adapter = AST_tree_hang_by_adapter(adapter, child);
    param_num++;

$   while (!is_passed && token_op_is_comma($tkn_pos))
    {
        next;

        try_parse(parse_rvalue, prog, tkn_pass, &child)
        adapter = AST_tree_hang_by_adapter(adapter, child);
        param_num++;
    }

   *params_quantity = param_num;
    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_input
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_input(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    AST_node *child = nullptr;

    try_skip_keyword(input, "expected keyword \"input\" at the beginning of the input operator")

    try_parse(parse_lvalue, prog, tkn_pass, &child)
$  *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_INPUT);
$   AST_node_hang_left(*subtree, child);

    try_skip_separator(comma_point, "expected \';\' after input operator")

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_output
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_output(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    AST_node *child = nullptr;

    try_skip_keyword(output, "expected keyword \"output\" at the beginning of the output operator")

    try_parse(parse_rvalue, prog, tkn_pass, &child)
$  *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_OUTPUT);
$   AST_node_hang_left(*subtree, child);

    try_skip_separator(comma_point, "expected \';\' after output operator")

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_component
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_scope(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    try_skip_separator(l_scope_figure, "expected \'{\' before scope entry")
$   prog_info_scope_open(prog);

    try_parse(parse_operators, prog, tkn_pass, subtree)

    try_skip_separator(r_scope_figure, "expected \'}\' after scope exit")
$   prog_info_scope_close(prog);

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_condition(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    try_skip_separator(l_scope_circle, "expected \'(\' before condition")
    try_parse(parse_rvalue, prog, tkn_pass, subtree);
    try_skip_separator(r_scope_circle, "expected \')\' after condition")

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_if
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_if(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    AST_node *child = nullptr;

    try_skip_keyword(if, "expected keyword \"if\" at the beginning of the conditional operator")

    try_parse(parse_condition, prog, tkn_pass, &child)
$  *subtree = AST_node_new(AST_NODE_OPERATOR_IF);
$   AST_node_hang_left(*subtree, child);

    try_parse(parse_then_else, prog, tkn_pass, &child);
$   AST_node_hang_right(*subtree, child);

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_then_else(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    AST_node *child = nullptr;

    try_parse(parse_scope, prog, tkn_pass, &child)
$  *subtree = AST_node_new(AST_NODE_OPERATOR_THEN_ELSE);
$   AST_node_hang_left(*subtree, child);

$   if (is_passed || !token_key_is_else($tkn_pos)) parse_success;

    try_parse(parse_else, prog, tkn_pass, &child)
$   AST_node_hang_right(*subtree, child);

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_else(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    try_skip_keyword(else, "expected keyword \"else\" at the beginning of the else case")
    try_parse(parse_scope, prog, tkn_pass, subtree)

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_while
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_while(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    AST_node *child = nullptr;

    try_skip_keyword(while, "expected keyword \"while\" at the beginning of the cycle")

    try_parse(parse_condition, prog, tkn_pass, &child)
$  *subtree = AST_node_new(AST_NODE_OPERATOR_WHILE);
$   AST_node_hang_left(*subtree, child);

    try_parse(parse_scope, prog, tkn_pass, &child)
$   AST_node_hang_right(*subtree, child);

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_return
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_return(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    AST_node *child = nullptr;

    try_skip_keyword(return, "expected keyword \"return\" at the beginning of the return operator")

    try_parse(parse_rvalue, prog, tkn_pass, &child)
$  *subtree = AST_node_new(AST_NODE_OPERATOR_RETURN);
$   AST_node_hang_left(*subtree, child);

    try_skip_separator(comma_point, "expected \';\' after return operator")

$   prog_info_meet_return(prog);

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_rvalue
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_rvalue(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    AST_node *expr_cur  = nullptr;
    AST_node *expr_next = nullptr;

$   if (parse_is_assignment(tkn_pass))
    {
        try_parse(parse_assignment_half, prog, tkn_pass, subtree);
        expr_cur = *subtree;
    }
    else
    {
        try_parse(parse_log_or, prog, tkn_pass, subtree)
        parse_success;
    }

$   while (parse_is_assignment(tkn_pass))
    {
        try_parse(parse_assignment_half, prog, tkn_pass, &expr_next)
        AST_node_hang_right(expr_cur, expr_next);

        expr_cur  = expr_next;
        expr_next =   nullptr;
    }

    try_parse(parse_log_or, prog, tkn_pass, &expr_next)
$   AST_node_hang_right(expr_cur, expr_next);

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_is_assignment(token_arr_pass *const tkn_pass)
{
$i
    log_verify(tkn_pass != nullptr, false);

    const token *const tkn_entry = $tkn_pos;

$   if (is_passed) { $o return false; }
    next;

$   if (is_passed || !token_op_is_assignment($tkn_pos)) { reset; $o return false; }
    reset; $o return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_assignment_half(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    AST_node *child = nullptr;

    try_parse(parse_lvalue, prog, tkn_pass, &child);
$  *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ASSIGNMENT);
$   AST_node_hang_left(*subtree, child);

    try_skip_separator(assignment, "expected \'=\' after variable access in assignment operator")

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_op pattern
//--------------------------------------------------------------------------------------------------------------------------------

static bool   parse_op_pattern(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree ,
       bool (*parse_op_lower) (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree),
       bool (*parse_op_token) (                       token_arr_pass *const tkn_pass, AST_node **const subtree))
{
    parse_start;

    log_verify(parse_op_lower != nullptr, false);
    log_verify(parse_op_token != nullptr, false);

    AST_node *node_op       = nullptr;
    AST_node *node_op_lower = nullptr;

    try_parse(parse_op_lower, prog, tkn_pass, &node_op_lower)
    *subtree = node_op_lower;

$   while (parse_op_token(tkn_pass, &node_op))
    {
        AST_node_hang_left(node_op, *subtree); *subtree = node_op;

        try_parse(parse_op_lower, prog, tkn_pass, &node_op_lower)
        AST_node_hang_right(node_op, node_op_lower);
    }

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_log_or
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool parse_log_or(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(prog, tkn_pass, subtree, parse_log_and, parse_log_or_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_log_or_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    log_verify(tkn_pass != nullptr, false);
    log_verify(subtree  != nullptr, false);

$   if (is_passed || !token_op_is_log_or($tkn_pos)) { $o return false; }
    next;

$   *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LOG_OR);

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_log_and
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool parse_log_and(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(prog, tkn_pass, subtree, parse_equal, parse_log_and_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_log_and_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    log_verify(tkn_pass != nullptr, false);
    log_verify(subtree  != nullptr, false);

$   if (is_passed || !token_op_is_log_and($tkn_pos)) { $o return false; }
    next;

$   *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LOG_AND);

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_equal
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool parse_equal(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(prog, tkn_pass, subtree, parse_cmp, parse_equal_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_equal_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    log_verify(tkn_pass != nullptr, false);
    log_verify(subtree  != nullptr, false);

$   if      (is_passed) { $o return false; }
    else if (token_op_is_are_equal($tkn_pos)) { $ *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ARE_EQUAL); }
    else if (token_op_is_not_equal($tkn_pos)) { $ *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_NOT_EQUAL); }
    else                { $o return false; }

    next;

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_cmp
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool parse_cmp(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(prog, tkn_pass, subtree, parse_add_sub, parse_cmp_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_cmp_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    log_verify(tkn_pass != nullptr, false);
    log_verify(subtree  != nullptr, false);

$   if (is_passed || !token_type_is_op($tkn_pos)) { $o return false; }

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"

$   switch ($tkn_pos->value.op)
    {
        case OPERATOR_LESS      : $ *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LESS);       break;
        case OPERATOR_MORE      : $ *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_MORE);       break;
        case OPERATOR_LESS_EQUAL: $ *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LESS_EQUAL); break;
        case OPERATOR_MORE_EQUAL: $ *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_MORE_EQUAL); break;
        default                 : $o return false;
    }

    #pragma GCC diagnostic pop

    next;

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_add_sub
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool parse_add_sub(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(prog, tkn_pass, subtree, parse_mul_div, parse_add_sub_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_add_sub_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    log_verify(tkn_pass != nullptr, false);
    log_verify(subtree  != nullptr, false);

$   if      (is_passed) { $o return false; }
    else if (token_op_is_add($tkn_pos)) { $ *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ADD); }
    else if (token_op_is_sub($tkn_pos)) { $ *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_SUB); }
    else                { $o return false; }

    next;

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_mul_div
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool parse_mul_div(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(prog, tkn_pass, subtree, parse_pow, parse_mul_div_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_mul_div_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    log_verify(tkn_pass != nullptr, false);
    log_verify(subtree  != nullptr, false);

$   if      (is_passed) { $o return false; }
    else if (token_op_is_mul($tkn_pos)) { $ *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_MUL); }
    else if (token_op_is_div($tkn_pos)) { $ *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_DIV); }
    else                { $o return false; }

    next;

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_pow
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool parse_pow(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(prog, tkn_pass, subtree, parse_not, parse_pow_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_pow_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    log_verify(tkn_pass != nullptr, false);
    log_verify(subtree  != nullptr, false);

$   if (is_passed || !token_op_is_pow($tkn_pos)) { $o return false; }
    next;

$   *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_POW);

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_not
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_not(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

$   if (!is_passed && token_op_is_not($tkn_pos))
    {
$       *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_NOT);
        next;

        AST_node *child = nullptr;
        try_parse(parse_operand, prog, tkn_pass, &child);
$       AST_node_hang_left(*subtree, child);

        parse_success;
    }

$   bool   result = parse_operand(prog, tkn_pass, subtree);
$o  return result;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_operand
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_operand(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

$   if (is_passed) parse_error("expected operand")

$   if      (token_op_is_l_scope_circle($tkn_pos)) try_parse(parse_expression           , prog, tkn_pass, subtree)
    else if (token_type_is_int         ($tkn_pos)) try_parse(parse_imm_int              , prog, tkn_pass, subtree)
    else                                           try_parse(parse_name_access_dependent, prog, tkn_pass, subtree)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_expression(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

    try_skip_separator(l_scope_circle, "expected \'(\' before expression in scopes")
    try_parse(parse_rvalue, prog, tkn_pass, subtree)
    try_skip_separator(r_scope_circle, "expected \')\' after expression in scopes")

    parse_success;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_imm_int(prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_start;

$   if (is_passed || !token_type_is_int($tkn_pos)) parse_error("expected immediate integer value")

$   *subtree = AST_node_new(AST_NODE_IMM_INT, $tkn_pos->value.imm_int);
    next;

    parse_success;
}
