#include "parser_static.h"

//================================================================================================================================
// PARSER
//================================================================================================================================

AST_node *parser(vector *token_arr, size_t *const main_func_id,
                                    size_t *const var_quantity,
                                    size_t *const func_quantity)
{
    vec_verify(token_arr, nullptr);
    log_verify(main_func_id  != nullptr, nullptr);
    log_verify(var_quantity  != nullptr, nullptr);
    log_verify(func_quantity != nullptr, nullptr);

    prog_info      *program  = prog_info_new();
    token_arr_pass *tkn_pass = token_arr_pass_new(token_arr);
    AST_node       *result   = nullptr;

    parse_general(program, tkn_pass, &result);

    *main_func_id  = program->main_func_id;
    *var_quantity  = program->var_storage->size;
    *func_quantity = program->func_storage->size;

         prog_info_delete(program);
    token_arr_pass_delete(tkn_pass);

   return result;
}

//================================================================================================================================

#define $tkn_pos (tkn_pass->arr_pos)
#define $tkn_end (tkn_pass->arr_end)

#define next()      { token_arr_pass_next     (tkn_pass);              }
#define reset()     { token_arr_pass_reset    (tkn_pass, tkn_entry);   }
#define is_passed()   token_arr_pass_is_passed(tkn_pass)

#define parse_verify()                                                                                                      \
          log_verify(tkn_pass != nullptr, false);                                                                           \
          log_verify( program != nullptr, false);                                                                           \
          log_verify( subtree != nullptr, false);                                                                           \
          *subtree = nullptr;


#define parse_fail_cmd
#define parse_fail                                                                                                          \
        {                                                                                                                   \
            parse_fail_cmd                                                                                                  \
            AST_tree_delete(*subtree); *subtree = nullptr;                                                                  \
            reset();                                                                                                        \
            return false;                                                                                                   \
        }

#define parse_success return true;

/**
*   Параметр op_type должен быть равен соответствующему параметру op_type в макросе token_op_is_smth в файле tokenizer.h.
*/
#define try_skip_separator(op_type)                                                                                         \
        if (!is_passed() && token_op_is_##op_type($tkn_pos)) next()                                                         \
        else parse_fail

/**
*   Параметр token_key должен быть равен соответствующему параметру token_key в макросе token_key_is_smth в файле tokenizer.h.
*/
#define try_key_word(token_key)                                                                                             \
        if (!is_passed() && token_key_is_##token_key($tkn_pos)) next()                                                      \
        else parse_fail

#define try_name_decl                                                                                                       \
        if (!is_passed() && token_type_is_name($tkn_pos) && prog_info_is_name_decl_possible(program, $tkn_pos))             \
        {                                                                                                                   \
            name = $tkn_pos;                                                                                                \
            next()                                                                                                          \
        }                                                                                                                   \
        else parse_fail

#define try_func_access                                                                                                     \
        if (!is_passed() && token_type_is_name($tkn_pos) && prog_info_is_func_exist(program, $tkn_pos))                     \
        {                                                                                                                   \
            func_id = prog_info_get_func_index(program, $tkn_pos);                                                          \
            next()                                                                                                          \
        }                                                                                                                   \
        else parse_fail

#define try_var_access                                                                                                      \
        if (!is_passed() && token_type_is_name($tkn_pos) && prog_info_is_var_exist_global(program, $tkn_pos))               \
        {                                                                                                                   \
            var_id = prog_info_get_var_index(program, $tkn_pos);                                                            \
            next()                                                                                                          \
        }                                                                                                                   \
        else parse_fail

#define try_parse_adapter(parse_func)                                                                                       \
        if (parse_func(program, tkn_pass, &child)) { adapter = AST_tree_hang_by_adapter(adapter, child); continue; }

//--------------------------------------------------------------------------------------------------------------------------------
// parse_general
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_general(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;
    AST_node    *adapter   =  nullptr;

   *subtree = AST_node_new(AST_NODE_FICTIONAL);
    adapter = *subtree;

    while (!is_passed())
    {
        try_parse_adapter(parse_var_decl )
        try_parse_adapter(parse_func_decl)

        parse_fail
    }

    if (program->main_func_id == -1UL) parse_fail
    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_var_decl
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_var_decl(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    const token *name      =  nullptr;

    try_key_word(int)
    try_name_decl
    try_skip_separator(comma_point)

    *subtree = AST_node_new(AST_NODE_DECL_VAR, prog_info_var_push(program, name));
    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_func_decl
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_func_decl(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    const token *name      =  nullptr;
    AST_node    *child     =  nullptr;
    size_t       func_id   =        0;

    try_key_word(int)
    try_name_decl
    try_skip_separator(l_scope_circle)

    func_id  = prog_info_func_push_back(program, name);
    *subtree = AST_node_new(AST_NODE_DECL_FUNC, func_id);
    prog_info_scope_open(program);

    #undef  parse_fail_cmd
    #define parse_fail_cmd prog_info_func_pop_back(program); prog_info_scope_close(program);

    parse_args(program, tkn_pass, &child, func_id);
    AST_node_hang_left (*subtree,  child);

    try_skip_separator(r_scope_circle)
    try_skip_separator(l_scope_figure)

    parse_operators(program, tkn_pass, &child);
    AST_node_hang_right     (*subtree,  child);

    try_skip_separator(r_scope_figure)

    if (!prog_info_func_exit(program, name, func_id)) parse_fail
    prog_info_scope_close   (program);

    parse_success
}

#undef  parse_fail_cmd
#define parse_fail_cmd

//--------------------------------------------------------------------------------------------------------------------------------
// parse_args
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_args(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree, const size_t func_id)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *arg_cur   =  nullptr;
    AST_node    *arg_next  =  nullptr;

    if (!parse_first_arg(program, tkn_pass, &arg_cur, func_id)) parse_fail
    *subtree = arg_cur;

    while (true)
    {
        if (!parse_other_arg(program, tkn_pass, &arg_next, func_id)) break;
        AST_node_hang_left(arg_cur, arg_next);

        arg_cur = arg_next;
    }

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_first_arg(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree, const size_t func_id)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    const token *name      =  nullptr;

    try_key_word(int)
    try_name_decl

    size_t arg_index = prog_info_var_push(program, name);
    prog_info_func_arg_push_back(program, func_id, arg_index);

    *subtree = AST_node_new(AST_NODE_DECL_VAR, arg_index);
    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_other_arg(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree, const size_t func_id)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;

    try_skip_separator(comma)

    if (parse_first_arg(program, tkn_pass, subtree, func_id)) parse_success
    /* else */                                                parse_fail
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_operators
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_operators(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;
    AST_node    *adapter   =  nullptr;

   *subtree = AST_node_new(AST_NODE_FICTIONAL);
    adapter = *subtree;

    while (!is_passed())
    {
        try_parse_adapter(parse_var_decl     )
        try_parse_adapter(parse_assignment_op)
        try_parse_adapter(parse_input        )
        try_parse_adapter(parse_output       )
        try_parse_adapter(parse_if           )
        try_parse_adapter(parse_while        )
        try_parse_adapter(parse_func_call_op )
        try_parse_adapter(parse_return       )

        break;
    }

    if ((*subtree)->left != nullptr) parse_success
    /* else */                       parse_fail
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_assignment_op
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_assignment_op(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ASSIGNMENT);

    if (!parse_lvalue(program, tkn_pass, &child)) parse_fail
    AST_node_hang_left(*subtree, child);

    try_skip_separator(assignment)

    if (!parse_rvalue(program, tkn_pass, &child)) parse_fail
    AST_node_hang_right(*subtree, child);

    try_skip_separator(comma_point)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_input
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_input(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    try_key_word(input)

    *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_INPUT);

    if (!parse_lvalue(program, tkn_pass, &child)) parse_fail
    AST_node_hang_left(*subtree, child);

    try_skip_separator(comma_point)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_output
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_output(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    try_key_word(output)

    *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_OUTPUT);

    if (!parse_rvalue(program, tkn_pass, &child)) parse_fail
    AST_node_hang_left(*subtree, child);

    try_skip_separator(comma_point)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_scope
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_scope(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *const tkn_entry = $tkn_pos;

    try_skip_separator(l_scope_figure)

    prog_info_scope_open(program);

    #undef  parse_fail_cmd
    #define parse_fail_cmd prog_info_scope_close(program);

    if (!parse_operators(program, tkn_pass, subtree)) parse_fail

    try_skip_separator(r_scope_figure)

    prog_info_scope_close(program);

    parse_success
}

#undef  parse_fail_cmd
#define parse_fail_cmd

//--------------------------------------------------------------------------------------------------------------------------------
// parse_if
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_if(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    try_key_word(if)
    try_skip_separator(l_scope_circle)

    *subtree = AST_node_new(AST_NODE_OPERATOR_IF);

    if (!parse_rvalue(program, tkn_pass, &child)) parse_fail
    AST_node_hang_left(*subtree, child);

    try_skip_separator(r_scope_circle)

    if (!parse_then_else(program, tkn_pass, &child)) parse_fail
    AST_node_hang_right(*subtree, child);

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_then_else(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    *subtree = AST_node_new(AST_NODE_OPERATOR_THEN_ELSE);

    if (!parse_scope(program, tkn_pass, &child)) parse_fail
    AST_node_hang_left(*subtree, child);

    parse_else(program, tkn_pass, &child);
    AST_node_hang_right(*subtree,  child);

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_else(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;

    try_key_word(else)

    if (parse_scope(program, tkn_pass, subtree)) parse_success
    /* else */                                   parse_fail
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_while
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_while(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    try_key_word(while)
    try_skip_separator(l_scope_circle)

    *subtree = AST_node_new(AST_NODE_OPERATOR_WHILE);

    if (!parse_rvalue(program, tkn_pass, &child)) parse_fail
    AST_node_hang_left(*subtree, child);

    try_skip_separator(r_scope_circle)

    if (!parse_scope(program, tkn_pass, &child)) parse_fail
    AST_node_hang_right(*subtree, child);

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_return
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_return(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    try_key_word(return)

    *subtree = AST_node_new(AST_NODE_OPERATOR_RETURN);

    if (!parse_rvalue(program, tkn_pass, &child)) parse_fail
    AST_node_hang_left(*subtree, child);

    try_skip_separator(comma_point)

    prog_info_meet_return(program);

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_rvalue
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_rvalue(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    parse_assignment(program, tkn_pass, subtree);

    if (parse_log_or(program, tkn_pass, &child))
    {
        if (*subtree == nullptr) *subtree = child;
        else       AST_tree_hang(*subtree,  child);
    }
    else parse_fail

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_assignment
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_assignment(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *assign    =  nullptr;
    AST_node    *child     =  nullptr;

    if (!parse_single_assignment(program, tkn_pass, subtree)) parse_fail
    assign = *subtree;

    while (true)
    {
        if (!parse_single_assignment(program, tkn_pass, &child)) parse_success

        AST_node_hang_right(assign, child);
        assign = child;
    }
    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_single_assignment(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ASSIGNMENT);

    if (!parse_lvalue(program, tkn_pass, &child)) parse_fail
    AST_node_hang_left(*subtree, child);

    try_skip_separator(assignment)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_op pattern
//--------------------------------------------------------------------------------------------------------------------------------

static bool   parse_op_pattern(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree ,
       bool (*parse_op_lower) (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree),
       bool (*parse_op_token)                           (token_arr_pass *const tkn_pass, AST_node **const subtree))
{
    parse_verify()
    log_verify(parse_op_lower != nullptr, false);

    const token *tkn_entry     = $tkn_pos;
    AST_node    *node_op       =  nullptr;
    AST_node    *node_op_lower =  nullptr;

    if (!parse_op_lower(program, tkn_pass, &node_op_lower)) parse_fail    /* else */ *subtree = node_op_lower;
    if (!parse_op_token(         tkn_pass, &node_op      )) parse_success /* else */ *subtree = node_op;

    AST_node_hang_left(node_op, node_op_lower);

    while (true)
    {
        if (!parse_op_lower(program, tkn_pass, &node_op_lower)) parse_fail
        AST_node_hang_right(node_op,            node_op_lower);

        if (!parse_op_token(tkn_pass, &node_op)) parse_success

        AST_node_hang_right(node_op_lower->prev, node_op);
        AST_node_hang_left (node_op,       node_op_lower);
    }

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_log_or
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_log_or(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(program, tkn_pass, subtree, parse_log_and, parse_log_or_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_log_or_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    log_assert(tkn_pass != nullptr);
    log_assert(subtree  != nullptr);

    if (is_passed() || !token_op_is_log_or($tkn_pos)) return false;

    next()
    *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LOG_OR);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_log_and
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_log_and(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(program, tkn_pass, subtree, parse_equal, parse_log_and_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_log_and_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    log_assert(tkn_pass != nullptr);
    log_assert(subtree  != nullptr);

    if (is_passed() || !token_op_is_log_and($tkn_pos)) return false;

    next()
    *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LOG_AND);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_equal
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_equal(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(program, tkn_pass, subtree, parse_cmp, parse_equal_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_equal_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    log_assert(tkn_pass != nullptr);
    log_assert(subtree  != nullptr);

    if      (is_passed()) return false;
    else if (token_op_is_are_equal($tkn_pos)) *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ARE_EQUAL);
    else if (token_op_is_not_equal($tkn_pos)) *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_NOT_EQUAL);
    else                  return false;

    next()

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_cmp
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_cmp(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(program, tkn_pass, subtree, parse_add_sub, parse_cmp_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_cmp_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    log_assert(tkn_pass != nullptr);
    log_assert(subtree  != nullptr);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"

    if     (is_passed() || !token_type_is_op($tkn_pos)) return false;
    switch ($tkn_pos->value.op)
    {
        case OPERATOR_LESS      : *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LESS);       break;
        case OPERATOR_MORE      : *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_MORE);       break;
        case OPERATOR_LESS_EQUAL: *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LESS_EQUAL); break;
        case OPERATOR_MORE_EQUAL: *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_MORE_EQUAL); break;
        default                 : return false;
    }

    #pragma GCC diagnostic pop

    next()

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_add_sub
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_add_sub(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(program, tkn_pass, subtree, parse_mul_div, parse_add_sub_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_add_sub_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    log_assert(tkn_pass != nullptr);
    log_assert(subtree  != nullptr);

    if      (is_passed()) return false;
    else if (token_op_is_add($tkn_pos)) *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ADD);
    else if (token_op_is_sub($tkn_pos)) *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_SUB);
    else                  return false;

    next()

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_mul_div
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_mul_div(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(program, tkn_pass, subtree, parse_pow, parse_mul_div_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_mul_div_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    log_assert(tkn_pass != nullptr);
    log_assert(subtree  != nullptr);

    if      (is_passed()) return false;
    else if (token_op_is_mul($tkn_pos)) *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_MUL);
    else if (token_op_is_div($tkn_pos)) *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_DIV);
    else                  return false;

    next()

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_pow
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_pow(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_op_pattern(program, tkn_pass, subtree, parse_not, parse_pow_token);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_pow_token(token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    log_assert(tkn_pass != nullptr);
    log_assert(subtree  != nullptr);

    if (is_passed() || !token_op_is_pow($tkn_pos)) return false;

    next()
    *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_POW);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_not
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_not(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    if (token_op_is_not($tkn_pos)) { *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_NOT); next() }

    if (parse_operand(program, tkn_pass, &child))
    {
        if (*subtree != nullptr) AST_node_hang_left(*subtree, child);
        else                    *subtree = child;
    }
    else parse_fail

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_operand
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_operand(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;

    if (parse_rvalue_scope(program, tkn_pass, subtree)) parse_success
    if (parse_func_call   (program, tkn_pass, subtree)) parse_success
    if (parse_rvalue_elem (program, tkn_pass, subtree)) parse_success

    parse_fail
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_rvalue_scope
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_rvalue_scope(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;

    try_skip_separator(l_scope_circle)
    if (!parse_rvalue(program, tkn_pass, subtree)) parse_fail
    try_skip_separator(r_scope_circle)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_func_call_op
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_func_call_op(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;

    if (!parse_func_call(program, tkn_pass, subtree)) parse_fail

    try_skip_separator(comma_point)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_func_call
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_func_call(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;
    size_t       func_id   = 0;

    try_func_access
    try_skip_separator(l_scope_circle)

    *subtree = AST_node_new(AST_NODE_CALL_FUNC, func_id);

    size_t param_quantity = 0;
    parse_params(program, tkn_pass, &child, &param_quantity);
    AST_node_hang_left(*subtree   ,  child);

    if (!prog_info_verify_func_param_quantity(program, func_id, param_quantity)) parse_fail

    try_skip_separator(r_scope_circle)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_params(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree, size_t *const param_quantity)
{
    parse_verify()
    log_verify(param_quantity != nullptr, false);

    const token *tkn_entry = $tkn_pos;
    AST_node    *adapter   = nullptr;
    AST_node    *child     = nullptr;
    size_t       param_cnt = 0;

   *subtree = AST_node_new(AST_NODE_FICTIONAL);
    adapter = *subtree;

    if (parse_first_param(program, tkn_pass, &child)) { adapter = AST_tree_hang_by_adapter(adapter, child); param_cnt++; }
    else parse_fail

    while (true)
    {
        if (parse_other_param(program, tkn_pass, &child)) { param_cnt++; adapter = AST_tree_hang_by_adapter(adapter, child); }
        else { *param_quantity = param_cnt; parse_success }
    }

    *param_quantity = param_cnt;
    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool parse_first_param(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    return parse_rvalue(program, tkn_pass, subtree);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_other_param(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;

    try_skip_separator(comma)

    if (parse_first_param(program, tkn_pass, subtree)) parse_success
    /* else */                                         parse_fail
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_rvalue_elem
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_rvalue_elem(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;

    if (token_type_is_int($tkn_pos)) { *subtree = AST_node_new(AST_NODE_IMM_INT, $tkn_pos->value.imm_int); next() parse_success }
    if (parse_lvalue(program, tkn_pass, subtree)) parse_success

    parse_fail
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_lvalue
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_lvalue(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    size_t var_id = 0;

    try_var_access

    *subtree = AST_node_new(AST_NODE_VARIABLE, var_id);

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------



//================================================================================================================================
// TOKEN_ARR_PASS
//================================================================================================================================

#define $pos (tkn_pass->arr_pos)
#define $end (tkn_pass->arr_end)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

static bool token_arr_pass_ctor(token_arr_pass *const tkn_pass, const vector *const tkn_arr)
{
    log_verify(tkn_pass != nullptr, false);
    vec_verify(tkn_arr            , false);

    $pos = (token *) vector_begin(tkn_arr);
    $end = (token *) vector_end  (tkn_arr) - 1; // последний токен - фиктивный

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static token_arr_pass *token_arr_pass_new(const vector *const tkn_arr)
{
    token_arr_pass *tkn_pass_new = (token_arr_pass *) log_calloc(1, sizeof(token_arr_pass));
    log_verify     (tkn_pass_new != nullptr, nullptr);

    if (!token_arr_pass_ctor(tkn_pass_new, tkn_arr)) { log_free(tkn_pass_new); return nullptr; }
    return tkn_pass_new;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void token_arr_pass_delete(token_arr_pass *const tkn_pass)
{
    if (tkn_pass == nullptr) return;
    log_free(tkn_pass);
}

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

static inline void token_arr_pass_next(token_arr_pass *const tkn_pass)
{
    log_verify(tkn_pass != nullptr, (void) 0);

    $pos++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline void token_arr_pass_reset(token_arr_pass *const tkn_pass, const token *const reset_val)
{
    log_verify(tkn_pass != nullptr, (void) 0);

    $pos = reset_val;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_arr_pass_is_passed(const token_arr_pass *const tkn_pass)
{
    log_verify(tkn_pass != nullptr, false);

    return $pos == $end;
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $pos
#undef $end

//================================================================================================================================
// VAR_INFO
//================================================================================================================================

#define $tkn_name   (tkn->value.name)
#define $tkn_imm    (tkn->value.imm_int)
#define $tkn_key    (tkn->value.key)
#define $tkn_op     (tkn->value.op)

#define $name   (var->name)
#define $size   (var->size)
#define $scope  (var->scope)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_ctor(var_info *const var, const char *name, const size_t size, const size_t scope)
{
    log_verify(var  != nullptr, false);
    log_verify(name != nullptr, false);

    $name  = name;
    $size  = size;
    $scope = stack_new(sizeof(size_t));

    if ($scope == nullptr) return false;
    bool   result = stack_push($scope, &scope);
    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static var_info *var_info_new(const char *name, const size_t size, const size_t scope)
{
    log_verify(name != nullptr, nullptr);

    var_info  *var_new = (var_info *) log_calloc(1, sizeof(var_info));
    log_verify(var_new != nullptr, nullptr);

    if (!var_info_ctor(var_new, name, size, scope)) { log_free(var_new); return nullptr; }
    return var_new;
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void var_info_dtor(void *const _var)
{
    if (_var == nullptr) return;

    var_info *const var = (var_info *) _var;
    stack_free($scope);
}

//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static void var_info_delete(void *const _var)
{
    var_info_dtor(_var);
    log_free     (_var);
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------
// scope
//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_scope_push(var_info *const var, const size_t scope)
{
    log_verify(var != nullptr, false);

    bool   result = stack_push($scope, &scope);
    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_scope_pop(var_info *const var, const size_t scope)
{
    log_verify(var != nullptr, false);

    if (stack_is_empty($scope)) return true;

    size_t scope_top = 0;
    stack_front($scope, &scope_top);

    if (scope_top == scope) return stack_pop($scope);
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// is_...
//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_is_equal(const var_info *const var, const token *const tkn)
{
    log_assert(var != nullptr);
    log_assert(tkn != nullptr);

    log_assert(tkn->type == TOKEN_NAME);
    log_assert($tkn_name !=    nullptr);

    if (tkn->size == $size && strncmp($name, $tkn_name, $size) == 0) return true;
    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool var_info_is_exist_global(const var_info *const var)
{
    log_assert(var != nullptr);

    bool   result = !stack_is_empty($scope);
    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool var_info_is_exist_local(const var_info *const var, const size_t scope)
{
    log_assert(var != nullptr);

    if (!var_info_is_exist_global(var)) return false;

    size_t scope_top = 0;

    if (!stack_front($scope, &scope_top)) return false;
    return scope_top == scope;
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $name
#undef $size
#undef $scope

//================================================================================================================================
// FUNC_INFO
//================================================================================================================================

#define $name   (func->name)
#define $size   (func->size)
#define $args   (func->args)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

static bool func_info_ctor(func_info *const func, const char *name, const size_t size)
{
    log_verify(func != nullptr, false);
    log_verify(name != nullptr, false);

    $name = name;
    $size = size;
    $args = vector_new(sizeof(size_t));

    if ($args == nullptr) return false;
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static func_info *func_info_new(const char *name, const size_t size)
{
    log_verify(name != nullptr, nullptr);

    func_info *func_new = (func_info *) log_calloc(1, sizeof(func_info));
    log_verify(func_new != nullptr, nullptr);

    if (!func_info_ctor(func_new, name, size)) { log_free(func_new); return nullptr; }
    return func_new;
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void func_info_dtor(void *const _func)
{
    if (_func == nullptr) return;

    func_info *const func = (func_info *) _func;
    vector_free($args);
}

//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static void func_info_delete(void *const _func)
{
    func_info_dtor(_func);
    log_free      (_func);
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

static bool func_info_arg_push(func_info *const func, const size_t arg)
{
    log_verify(func != nullptr, false);

    bool   result = vector_push_back($args, &arg);
    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool func_info_is_equal(const func_info *const func, const token *const tkn)
{
    log_assert(func != nullptr);
    log_assert(tkn  != nullptr);

    log_assert(tkn->type == TOKEN_NAME);
    log_assert($tkn_name !=    nullptr);

    if (tkn->size == $size && strncmp($name, $tkn_name, $size) == 0) return true;
    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $name
#undef $size
#undef $args

//================================================================================================================================
// PROG_INFO
//================================================================================================================================

#define $v_store    (prog->var_storage)
#define $f_store    (prog->func_storage)
#define $scope      (prog->scope)
#define $main_id    (prog->main_func_id)
#define $is_ret     (prog->is_return_op)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_ctor(prog_info *const prog)
{
    log_verify(prog != nullptr, false);

    $v_store = vector_new(sizeof(var_info ), nullptr,  var_info_dtor);
    $f_store = vector_new(sizeof(func_info), nullptr, func_info_dtor);

    if ($v_store == nullptr) { vector_free($f_store); return false; }
    if ($f_store == nullptr) { vector_free($v_store); return false; }

    $scope   =     0;
    $main_id =  -1UL;
    $is_ret  = false;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static prog_info *prog_info_new()
{
    prog_info *prog_new = (prog_info *) log_calloc(1, sizeof(prog_info));
    log_verify(prog_new != nullptr, nullptr);

    if (!prog_info_ctor(prog_new)) { log_free(prog_new); return nullptr; }
    return prog_new;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void prog_info_dtor(prog_info *const prog)
{
    if (prog == nullptr) return;

    vector_free($v_store);
    vector_free($f_store);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void prog_info_delete(prog_info *const prog)
{
    prog_info_dtor(prog);
    log_free      (prog);
}

//--------------------------------------------------------------------------------------------------------------------------------
// is_exist
//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_is_var_exist_global(const prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, false);
    log_verify(tkn  != nullptr, false);

    log_verify(tkn->type == TOKEN_NAME, false);

    var_info *v_cur = (var_info *) vector_begin($v_store);
    var_info *v_end = (var_info *) vector_end  ($v_store);

    for (; v_cur != v_end; ++v_cur)
    {
        if (var_info_is_equal       (v_cur, tkn) &&
            var_info_is_exist_global(v_cur)) return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_is_var_exist_local(const prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, false);
    log_verify(tkn  != nullptr, false);

    log_verify(tkn->type == TOKEN_NAME, false);

    var_info *v_cur = (var_info *) vector_begin($v_store);
    var_info *v_end = (var_info *) vector_end  ($v_store);

    for (; v_cur != v_end; ++v_cur)
    {
        if (var_info_is_equal      (v_cur,    tkn) &&
            var_info_is_exist_local(v_cur, $scope)) return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_is_func_exist(const prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, false);
    log_verify(tkn  != nullptr, false);

    log_verify(tkn->type == TOKEN_NAME, false);

    func_info *f_cur = (func_info *) vector_begin($f_store);
    func_info *f_end = (func_info *) vector_end  ($f_store);

    for (; f_cur != f_end; ++f_cur)
        if (func_info_is_equal(f_cur, tkn)) return true;

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
// is_possible
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool prog_info_is_name_decl_possible(const prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, false);
    log_verify(tkn  != nullptr, false);

    log_verify(tkn->type == TOKEN_NAME, false);

    bool   result = !prog_info_is_var_exist_local(prog, tkn) && !prog_info_is_func_exist(prog, tkn);
    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------
// get_index
//--------------------------------------------------------------------------------------------------------------------------------

static size_t prog_info_get_var_index(const prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, -1UL);
    log_verify(tkn  != nullptr, -1UL);

    log_verify(tkn->type == TOKEN_NAME, -1UL);

    var_info *v_cur = (var_info *) vector_begin($v_store);
    var_info *v_end = (var_info *) vector_end  ($v_store);
    size_t    index = 0;

    for (; v_cur != v_end; ++v_cur)
    {
        if (var_info_is_equal(v_cur, tkn)) return index;
        ++index;
    }

    return -1UL;
}

//--------------------------------------------------------------------------------------------------------------------------------

static size_t prog_info_get_func_index(const prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, -1UL);
    log_verify(tkn  != nullptr, -1UL);

    log_verify(tkn->type == TOKEN_NAME, -1UL);

    func_info *f_cur = (func_info *) vector_begin($f_store);
    func_info *f_end = (func_info *) vector_end  ($f_store);
    size_t     index = 0;

    for (; f_cur != f_end; ++f_cur)
    {
        if (func_info_is_equal(f_cur, tkn)) return index;
    }

    return -1UL;
}

//--------------------------------------------------------------------------------------------------------------------------------
// push pop
//--------------------------------------------------------------------------------------------------------------------------------

static size_t prog_info_var_push(prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, -1UL);
    log_verify(tkn  != nullptr, -1UL);

    log_verify(tkn->type == TOKEN_NAME, -1UL);

    size_t var_index = prog_info_get_var_index(prog, tkn);
    if    (var_index != -1UL)
    {
        var_info_scope_push((var_info *) vector_begin($v_store) + var_index, $scope);
        return var_index;
    }

    var_index = $v_store->size;

    var_info       var_new = {};
    var_info_ctor(&var_new, $tkn_name, tkn->size, $scope);

    vector_push_back($v_store, &var_new);

    return var_index;
}

//--------------------------------------------------------------------------------------------------------------------------------

static size_t prog_info_func_push_back(prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, -1UL);
    log_verify(tkn  != nullptr, -1UL);

    log_verify(tkn->type == TOKEN_NAME, -1UL);

    size_t func_index = prog_info_get_func_index(prog, tkn);
    if    (func_index != -1UL) return func_index;

    func_index = $f_store->size;

    func_info       func_new = {};
    func_info_ctor(&func_new, $tkn_name, tkn->size);

    vector_push_back($f_store, &func_new);

    return func_index;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline void prog_info_func_pop_back(prog_info *const prog)
{
    log_verify(prog != nullptr, (void) 0);
    log_verify(!vector_is_empty($f_store), (void) 0);

    vector_pop_back($f_store);
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline void prog_info_func_arg_push_back(prog_info *const prog, const size_t func_index, const size_t arg_index)
{
    log_verify(prog != nullptr, (void) 0);
    log_verify(func_index < $f_store->size, (void) 0);
    log_verify(arg_index  < $v_store->size, (void) 0);

    func_info_arg_push((func_info *) vector_begin($f_store) + func_index, arg_index);
}

//--------------------------------------------------------------------------------------------------------------------------------
// scope handler
//--------------------------------------------------------------------------------------------------------------------------------

static inline void prog_info_scope_open(prog_info *const prog)
{
    log_verify(prog != nullptr, (void) 0);

    $scope++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void prog_info_scope_close(prog_info *const prog)
{
    log_verify(prog != nullptr, (void) 0);

    var_info *v_cur = (var_info *) vector_begin($v_store);
    var_info *v_end = (var_info *) vector_end  ($v_store);

    for (; v_cur != v_end; ++v_cur)
        var_info_scope_pop(v_cur, $scope);

    $scope--;
}

//--------------------------------------------------------------------------------------------------------------------------------
// other
//--------------------------------------------------------------------------------------------------------------------------------

static inline void prog_info_meet_return(prog_info *const prog)
{
    log_verify(prog != nullptr, (void) 0);

    if ($scope == 1) $is_ret = true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool prog_info_func_exit(prog_info *const prog, const token *const tkn, const size_t func_id)
{
    log_assert(prog != nullptr);
    log_assert(tkn  != nullptr);

    log_assert(tkn->type == TOKEN_NAME);

    static size_t MAIN_FUNC_NAME_len = strlen(MAIN_FUNC_NAME);

    $main_id = (tkn->size == MAIN_FUNC_NAME_len &&
                strncmp(tkn->value.name, MAIN_FUNC_NAME, tkn->size) == 0) ? func_id : -1UL;

    bool   result = $is_ret; $is_ret = false;
    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool prog_info_verify_func_param_quantity(prog_info *const prog, const size_t func_index, const size_t param_quantity)
{
    log_verify(prog != nullptr, false);
    log_verify(func_index < $f_store->size, false);

    bool   result = param_quantity == ((func_info *) vector_begin($f_store))[func_index].args->size;
    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $v_store
#undef $f_store
#undef $scope
#undef $main_id
#undef $is_ret
