#include "parser_static.h"

//================================================================================================================================
// PARSER
//================================================================================================================================

AST_node *parser(vector *token_arr)
{
$i
$   vec_verify(token_arr, nullptr);

$   prog_info      *program  = prog_info_new();
$   token_arr_pass *tkn_pass = token_arr_pass_new(token_arr);
$   AST_node       *result   = nullptr;

$   parse_general(program, tkn_pass, &result);
$   AST_tree_graphviz_dump           (result);

$         AST_tree_delete(result);
$        prog_info_delete(program);
$   token_arr_pass_delete(tkn_pass);

$o  return nullptr;
}

//================================================================================================================================

#define $tkn_pos (tkn_pass->arr_pos)
#define $tkn_end (tkn_pass->arr_end)

#define next()      { $ token_arr_pass_next     (tkn_pass);              }
#define reset()     { $ token_arr_pass_reset    (tkn_pass, tkn_entry);   }
#define is_passed()     token_arr_pass_is_passed(tkn_pass)

#define parse_verify()                                                                                                      \
          log_verify(tkn_pass != nullptr, false);                                                                           \
          log_verify( program != nullptr, false);                                                                           \
          log_verify( subtree != nullptr, false);                                                                           \
          *subtree = nullptr;


#define parse_fail_cmd
#define parse_fail                                                                                                          \
        {   $                                                                                                               \
            parse_fail_cmd                                                                                                  \
            AST_tree_delete(*subtree); *subtree = nullptr;                                                                  \
            reset();                                                                                                        \
        $o  return false;                                                                                                   \
        }

#define parse_success { $o return true; }

/**
*   Параметр op_type должен быть равен соответствующему параметру op_type в макросе token_op_is_smth в файле tokenizer.h.
*/
#define try_token_op(op_type)                                                                                               \
    $   if (!is_passed() && token_op_is_##op_type($tkn_pos)) next()                                                         \
        else parse_fail

/**
*   Параметр token_key должен быть равен соответствующему параметру token_key в макросе token_key_is_smth в файле tokenizer.h.
*/
#define try_token_key(token_key)                                                                                            \
    $   if (!is_passed() && token_key_is_##token_key($tkn_pos)) next()                                                      \
        else parse_fail

#define try_name_decl                                                                                                       \
    $   if (!is_passed() && token_type_is_name($tkn_pos) && prog_info_is_name_decl_possible(program, $tkn_pos))             \
        {                                                                                                                   \
            name = $tkn_pos;                                                                                                \
            next()                                                                                                          \
        }                                                                                                                   \
        else parse_fail

#define try_func_access                                                                                                     \
    $   if (!is_passed() && token_type_is_name($tkn_pos) && prog_info_is_func_exist(program, $tkn_pos))                     \
        {                                                                                                                   \
            func_id = prog_info_get_func_index(program, $tkn_pos);                                                          \
            next()                                                                                                          \
        }                                                                                                                   \
        else parse_fail

#define try_var_access                                                                                                      \
    $   if (!is_passed() && token_type_is_name($tkn_pos) && prog_info_is_var_exist_global(program, $tkn_pos))               \
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
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;
    AST_node    *adapter   =  nullptr;

$  *subtree = AST_node_new(AST_NODE_FICTIONAL);
    adapter = *subtree;

$   while (!is_passed())
    {
        try_parse_adapter(parse_var_decl )
        try_parse_adapter(parse_func_decl)

        parse_fail
    }
    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_var_decl
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_var_decl(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    const token *name      =  nullptr;

    try_token_key(int)
    try_name_decl
    try_token_op(comma_point)

$   *subtree = AST_node_new(AST_NODE_DECL_VAR, prog_info_var_push(program, name));
    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_func_decl
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_func_decl(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    const token *name      =  nullptr;
    AST_node    *child     =  nullptr;
    size_t       func_id   =        0;

    try_token_key(int)
    try_name_decl
    try_token_op(l_scope_circle)

$   func_id  = prog_info_func_push_back(program, name);
$   *subtree = AST_node_new(AST_NODE_DECL_FUNC, func_id);
$   prog_info_scope_open(program);

    #undef  parse_fail_cmd
    #define parse_fail_cmd prog_info_func_pop_back(program); prog_info_scope_close(program);

$   parse_args(program, tkn_pass, &child);
$   AST_node_hang_left (*subtree,  child);

    try_token_op(r_scope_circle)
    try_token_op(l_scope_figure)

$   parse_operators(program, tkn_pass, &child);
$   AST_node_hang_right     (*subtree,  child);

    try_token_op(r_scope_figure)

$   if (!prog_info_func_exit(program, name, func_id)) parse_fail
$   prog_info_scope_close   (program);

    parse_success
}

#undef  parse_fail_cmd
#define parse_fail_cmd

//--------------------------------------------------------------------------------------------------------------------------------
// parse_args
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_args(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *arg_cur   =  nullptr;
    AST_node    *arg_next  =  nullptr;

$   if (!parse_first_arg(program, tkn_pass, &arg_cur)) parse_fail
    *subtree = arg_cur;

$   while (true)
    {
        if (!parse_other_arg(program, tkn_pass, &arg_next)) break;
        AST_node_hang_left(arg_cur, arg_next);

        arg_cur = arg_next;
    }

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_first_arg(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    const token *name      =  nullptr;

    try_token_key(int)
    try_name_decl

$   *subtree = AST_node_new(AST_NODE_DECL_VAR, prog_info_var_push(program, name));
    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_other_arg(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;

    try_token_op(comma)

$   if (parse_first_arg(program, tkn_pass, subtree)) parse_success
    /* else */                                       parse_fail
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_operators
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_operators(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;
    AST_node    *adapter   =  nullptr;

$  *subtree = AST_node_new(AST_NODE_FICTIONAL);
    adapter = *subtree;

$   while (!is_passed())
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
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

$   *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ASSIGNMENT);

$   if (!parse_lvalue(program, tkn_pass, &child)) parse_fail
$   AST_node_hang_left(*subtree, child);

    try_token_op(assignment)

$   if (!parse_rvalue(program, tkn_pass, &child)) parse_fail
$   AST_node_hang_right(*subtree, child);

    try_token_op(comma_point)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_input
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_input(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    try_token_key(input)

$   *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_INPUT);

$   if (!parse_lvalue(program, tkn_pass, &child)) parse_fail
$   AST_node_hang_left(*subtree, child);

    try_token_op(comma_point)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_output
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_output(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    try_token_key(output)

$   *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_OUTPUT);

$   if (!parse_rvalue(program, tkn_pass, &child)) parse_fail
$   AST_node_hang_left(*subtree, child);

    try_token_op(comma_point)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_scope
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_scope(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *const tkn_entry = $tkn_pos;

    try_token_op(l_scope_figure)

$   prog_info_scope_open(program);

    #undef  parse_fail_cmd
    #define parse_fail_cmd prog_info_scope_close(program);

$   if (!parse_operators(program, tkn_pass, subtree)) parse_fail

    try_token_op(r_scope_figure)

$   prog_info_scope_close(program);

    parse_success
}

#undef  parse_fail_cmd
#define parse_fail_cmd

//--------------------------------------------------------------------------------------------------------------------------------
// parse_if
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_if(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    try_token_key(if)
    try_token_op(l_scope_circle)

$   *subtree = AST_node_new(AST_NODE_OPERATOR_IF);

$   if (!parse_rvalue(program, tkn_pass, &child)) parse_fail
$   AST_node_hang_left(*subtree, child);

    try_token_op(r_scope_circle)

$   if (!parse_then_else(program, tkn_pass, &child)) parse_fail
$   AST_node_hang_right(*subtree, child);

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_then_else(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

$   *subtree = AST_node_new(AST_NODE_OPERATOR_THEN_ELSE);

$   if (!parse_scope(program, tkn_pass, &child)) parse_fail
$   AST_node_hang_left(*subtree, child);

$   parse_else(program, tkn_pass, &child);
$   AST_node_hang_right(*subtree,  child);

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_else(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;

    try_token_key(else)

$   if (parse_scope(program, tkn_pass, subtree)) parse_success
    /* else */                                   parse_fail
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_while
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_while(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    try_token_key(while)
    try_token_op(l_scope_circle)

$   *subtree = AST_node_new(AST_NODE_OPERATOR_WHILE);

$   if (!parse_rvalue(program, tkn_pass, &child)) parse_fail
$   AST_node_hang_left(*subtree, child);

    try_token_op(r_scope_circle)

$   if (!parse_scope(program, tkn_pass, &child)) parse_fail
$   AST_node_hang_right(*subtree, child);

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_return
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_return(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

    try_token_key(return)

$   *subtree = AST_node_new(AST_NODE_OPERATOR_RETURN);

$   if (!parse_rvalue(program, tkn_pass, &child)) parse_fail
$   AST_node_hang_left(*subtree, child);

    try_token_op(comma_point)

$   prog_info_meet_return(program);

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_rvalue
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_rvalue(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

$   parse_assignment(program, tkn_pass, subtree);

$   if (parse_log_or(program, tkn_pass, &child))
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
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *assign    =  nullptr;
    AST_node    *child     =  nullptr;

$   if (!parse_single_assignment(program, tkn_pass, subtree)) parse_fail
    assign = *subtree;

$   while (true)
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
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

$   *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ASSIGNMENT);

$   if (!parse_lvalue(program, tkn_pass, &child)) parse_fail
$   AST_node_hang_left(*subtree, child);

    try_token_op(assignment)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_op macro
//--------------------------------------------------------------------------------------------------------------------------------

#define parse_op_prepare                                                    \
        const token *tkn_entry     = $tkn_pos;                              \
        AST_node    *node_op       =  nullptr;                              \
        AST_node    *node_op_lower =  nullptr;

#define try_op_lower_first(parse_op_lower)                                  \
    $   if (!parse_op_lower(program, tkn_pass, &node_op_lower)) parse_fail  \
        *subtree = node_op_lower;

#define create_op_node_first()                                              \
    $   *subtree = node_op;                                                 \
        AST_node_hang_left(node_op, node_op_lower);

#define try_op_lower_cycle(parse_op_lower)                                  \
    $   if (!parse_op_lower(program, tkn_pass, &node_op_lower)) parse_fail  \
        AST_node_hang_right(node_op, node_op_lower);                        \

#define op_nodes_rehang()                                                   \
    $   AST_node_hang_right(node_op_lower->prev, node_op);                  \
        AST_node_hang_left (node_op,       node_op_lower);

#define try_op

#define parse_op_body(parse_op_lower)                                       \
        parse_verify()                                                      \
        parse_op_prepare                                                    \
                                                                            \
        try_op_lower_first(parse_op_lower)                                  \
        try_op                                                              \
        create_op_node_first()                                              \
                                                                            \
        while (true)                                                        \
        {                                                                   \
            try_op_lower_cycle(parse_op_lower)                              \
            try_op                                                          \
            op_nodes_rehang()                                               \
        }                                                                   \
        parse_success

//--------------------------------------------------------------------------------------------------------------------------------
// parse_log_or
//--------------------------------------------------------------------------------------------------------------------------------

#undef  try_op
#define try_op                                                                                                          \
    $   if (is_passed() || !token_op_is_log_or($tkn_pos)) parse_success                                                 \
        next()                                                                                                          \
        node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LOG_OR);

static bool parse_log_or(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_op_body(parse_log_and)
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_log_and
//--------------------------------------------------------------------------------------------------------------------------------

#undef  try_op
#define try_op                                                                                                          \
    $   if (is_passed() || !token_op_is_log_and($tkn_pos)) parse_success                                                \
        next()                                                                                                          \
        node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LOG_AND);


static bool parse_log_and(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_op_body(parse_equal)
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_equal
//--------------------------------------------------------------------------------------------------------------------------------

#undef  try_op
#define try_op                                                                                                          \
    $   if      (is_passed()) parse_success                                                                             \
        if      (token_op_is_are_equal($tkn_pos)) node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ARE_EQUAL);    \
        else if (token_op_is_not_equal($tkn_pos)) node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_NOT_EQUAL);    \
        else    parse_success                                                                                           \
        next()

static bool parse_equal(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_op_body(parse_cmp)
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_cmp
//--------------------------------------------------------------------------------------------------------------------------------

#undef  try_op
#define try_op                                                                                                          \
    $   if     (is_passed() || !token_type_is_op($tkn_pos)) parse_success                                               \
        switch ($tkn_pos->value.op)                                                                                     \
        {                                                                                                               \
            case AST_OPERATOR_LESS      : node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LESS);       break;    \
            case AST_OPERATOR_MORE      : node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_MORE);       break;    \
            case AST_OPERATOR_LESS_EQUAL: node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_LESS_EQUAL); break;    \
            case AST_OPERATOR_MORE_EQUAL: node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_MORE_EQUAL); break;    \
            default                     : parse_success                                                                 \
        }                                                                                                               \
        next()

static bool parse_cmp(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"
    parse_op_body(parse_add_sub)
    #pragma GCC diagnostic pop
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_add_sub
//--------------------------------------------------------------------------------------------------------------------------------

#undef  try_op
#define try_op                                                                                                          \
    $   if      (is_passed()) parse_success                                                                             \
        else if (token_op_is_add($tkn_pos)) node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_ADD);                \
        else if (token_op_is_sub($tkn_pos)) node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_SUB);                \
        else    parse_success                                                                                           \
        next()

static bool parse_add_sub(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_op_body(parse_mul_div)
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_mul_div
//--------------------------------------------------------------------------------------------------------------------------------

#undef  try_op
#define try_op                                                                                                          \
    $   if      (is_passed()) parse_success                                                                             \
        else if (token_op_is_mul($tkn_pos)) node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_MUL);                \
        else if (token_op_is_div($tkn_pos)) node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_DIV);                \
        else    parse_success                                                                                           \
        next()

static bool parse_mul_div(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_op_body(parse_pow)
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_pow
//--------------------------------------------------------------------------------------------------------------------------------

#undef  try_op
#define try_op                                                                                                          \
    $   if (is_passed() || !token_op_is_pow($tkn_pos)) parse_success                                                    \
        next()                                                                                                          \
        node_op = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_POW);

static bool parse_pow(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_op_body(parse_not)
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef parse_op_prepare
#undef try_op_lower_first
#undef create_op_node_first
#undef try_op_lower_cycle
#undef op_nodes_rehang
#undef try_op
#undef parse_op_body

//--------------------------------------------------------------------------------------------------------------------------------
// parse_not
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_not(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;

$   if (token_op_is_not($tkn_pos)) { *subtree = AST_node_new(AST_NODE_OPERATOR, AST_OPERATOR_NOT); next() }

$   if (parse_operand(program, tkn_pass, &child))
    {
$       if (*subtree != nullptr) AST_node_hang_left(*subtree, child);
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
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;

$   if (parse_rvalue_scope(program, tkn_pass, subtree)) parse_success
$   if (parse_func_call   (program, tkn_pass, subtree)) parse_success
$   if (parse_rvalue_elem (program, tkn_pass, subtree)) parse_success

    parse_fail
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_rvalue_scope
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_rvalue_scope(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;

    try_token_op(l_scope_circle)
$   if (!parse_rvalue(program, tkn_pass, subtree)) parse_fail
    try_token_op(r_scope_circle)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_func_call_op
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_func_call_op(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;

$   if (!parse_func_call(program, tkn_pass, subtree)) parse_fail

    try_token_op(comma_point)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_func_call
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_func_call(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    AST_node    *child     =  nullptr;
    size_t func_id = 0;

    try_func_access
    try_token_op(l_scope_circle)

$   *subtree = AST_node_new(AST_NODE_CALL_FUNC, func_id);

$   parse_params(program, tkn_pass, &child);
$   AST_node_hang_left(*subtree   ,  child);

    try_token_op(r_scope_circle)

    parse_success
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_params(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry  = $tkn_pos;
    AST_node    *param_cur  =  nullptr;
    AST_node    *param_next =  nullptr;

$   if (parse_first_param(program, tkn_pass, &param_cur)) *subtree = param_cur;
    else parse_fail

$   while (true)
    {
        if (parse_other_param(program, tkn_pass, &param_next)) AST_node_hang_left(param_cur, param_next);
        else parse_success

        param_cur = param_next;
    }
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
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;

    try_token_op(comma)

$   if (parse_first_param(program, tkn_pass, subtree)) parse_success
    /* else */                                         parse_fail
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_rvalue_elem
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_rvalue_elem(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;

$   if (token_type_is_int($tkn_pos)) { *subtree = AST_node_new(AST_NODE_IMM_INT, $tkn_pos->value.imm_int); next() parse_success }
$   if (parse_lvalue(program, tkn_pass, subtree)) parse_success

    parse_fail
}

//--------------------------------------------------------------------------------------------------------------------------------
// parse_lvalue
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_lvalue(prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree)
{
$i
    parse_verify()

    const token *tkn_entry = $tkn_pos;
    size_t var_id = 0;

    try_var_access

$   *subtree = AST_node_new(AST_NODE_VARIABLE, var_id);

    parse_success
}

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
$i
    log_verify(tkn_pass != nullptr, false);
    vec_verify(tkn_arr            , false);

$   $pos = (token *) vector_begin(tkn_arr);
$   $end = (token *) vector_end  (tkn_arr) - 1; // последний токен - фиктивный

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static token_arr_pass *token_arr_pass_new(const vector *const tkn_arr)
{
$i
    token_arr_pass *tkn_pass_new = (token_arr_pass *) log_calloc(1, sizeof(token_arr_pass));
    log_verify     (tkn_pass_new != nullptr, nullptr);

$   if (!token_arr_pass_ctor(tkn_pass_new, tkn_arr)) { log_free(tkn_pass_new); $o return nullptr; }
$o  return tkn_pass_new;
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
$i
    log_verify(var  != nullptr, false);
    log_verify(name != nullptr, false);

    $name  = name;
    $size  = size;
$   $scope = stack_new(sizeof(size_t));

    if ($scope == nullptr) { $o return false; }

$   bool   result = stack_push($scope, &scope);
$o  return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static var_info *var_info_new(const char *name, const size_t size, const size_t scope)
{
$i
    log_verify(name != nullptr, nullptr);

    var_info  *var_new = (var_info *) log_calloc(1, sizeof(var_info));
    log_verify(var_new != nullptr, nullptr);

$   if (!var_info_ctor(var_new, name, size, scope)) { log_free(var_new); $o return nullptr; }
$o  return var_new;
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void var_info_dtor(void *const _var)
{
$i
    if (_var == nullptr) { $o return; }

    var_info *const var = (var_info *) _var;
$   stack_free($scope);
$o
}

//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static void var_info_delete(void *const _var)
{
$i
$   var_info_dtor(_var);
    log_free     (_var);
$o
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------
// scope
//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_scope_push(var_info *const var, const size_t scope)
{
$o
    log_verify(var != nullptr, false);

$   bool   result = stack_push($scope, &scope);
$o  return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_scope_pop(var_info *const var, const size_t scope)
{
$i
    log_verify(var != nullptr, false);

$   if (stack_is_empty($scope)) { $o return true; }

    size_t scope_top = 0;
    stack_front($scope, &scope_top);

$   if (scope_top == scope) { $o return stack_pop($scope); }
$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// is_...
//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_is_equal(const var_info *const var, const token *const tkn)
{
$i
    log_assert(var != nullptr);
    log_assert(tkn != nullptr);

    log_assert(tkn->type == TOKEN_NAME);
    log_assert($tkn_name !=    nullptr);

$   if (tkn->size == $size && strncmp($name, $tkn_name, $size) == 0) { $o return true; }
$o  return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool var_info_is_exist_global(const var_info *const var)
{
$i
    log_assert(var != nullptr);

$   bool   result = !stack_is_empty($scope);
$o  return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool var_info_is_exist_local(const var_info *const var, const size_t scope)
{
$i
    log_assert(var != nullptr);

$   if (!var_info_is_exist_global(var)) { $o return false; }

    size_t scope_top = 0;

$   if (!stack_front($scope, &scope_top)) { $o return false; }
$o  return scope_top == scope;
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
$i
    log_verify(func != nullptr, false);
    log_verify(name != nullptr, false);

    $name = name;
    $size = size;
$   $args = vector_new(sizeof(size_t));

    if ($args == nullptr) { $o return false; }
$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static func_info *func_info_new(const char *name, const size_t size)
{
$i
    log_verify(name != nullptr, nullptr);

    func_info *func_new = (func_info *) log_calloc(1, sizeof(func_info));
    log_verify(func_new != nullptr, nullptr);

$   if (!func_info_ctor(func_new, name, size)) { log_free(func_new); $o return nullptr; }
$o  return func_new;
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void func_info_dtor(void *const _func)
{
$i
    if (_func == nullptr) { $o return; }

    func_info *const func = (func_info *) _func;
$   vector_free($args);
$o
}

//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static void func_info_delete(void *const _func)
{
$i
$   func_info_dtor(_func);
    log_free      (_func);
$o
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

static bool func_info_arg_push(func_info *const func, const size_t arg)
{
$i
    log_verify(func != nullptr, false);

$   bool   result = vector_push_back($args, &arg);
$o  return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool func_info_is_equal(const func_info *const func, const token *const tkn)
{
$i
    log_assert(func != nullptr);
    log_assert(tkn  != nullptr);

    log_assert(tkn->type == TOKEN_NAME);
    log_assert($tkn_name !=    nullptr);

$   if (tkn->size == $size && strncmp($name, $tkn_name, $size) == 0) { $o return true; }
$o  return false;
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
$i
    log_verify(prog != nullptr, false);

$   $v_store = vector_new(sizeof(var_info ), nullptr,  var_info_dtor);
$   $f_store = vector_new(sizeof(func_info), nullptr, func_info_dtor);

$   if ($v_store == nullptr) { vector_free($f_store); $o return false; }
$   if ($f_store == nullptr) { vector_free($v_store); $o return false; }

    $scope   =     0;
    $main_id =  -1UL;
    $is_ret  = false;

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static prog_info *prog_info_new()
{
$i
    prog_info *prog_new = (prog_info *) log_calloc(1, sizeof(prog_info));
    log_verify(prog_new != nullptr, nullptr);

$   if (!prog_info_ctor(prog_new)) { log_free(prog_new); $o return nullptr; }
$o  return prog_new;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void prog_info_dtor(prog_info *const prog)
{
$i
    if (prog == nullptr) { $o return; }

$   vector_free($v_store);
$   vector_free($f_store);
$o
}

//--------------------------------------------------------------------------------------------------------------------------------

static void prog_info_delete(prog_info *const prog)
{
$i
$   prog_info_dtor(prog);
    log_free      (prog);
$o
}

//--------------------------------------------------------------------------------------------------------------------------------
// is_exist
//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_is_var_exist_global(const prog_info *const prog, const token *const tkn)
{
$i
    log_verify(prog != nullptr, false);
    log_verify(tkn  != nullptr, false);

    log_verify(tkn->type == TOKEN_NAME, false);

$   var_info *v_cur = (var_info *) vector_begin($v_store);
$   var_info *v_end = (var_info *) vector_end  ($v_store);

$   for (; v_cur != v_end; ++v_cur)
    {
        if (var_info_is_equal       (v_cur, tkn) &&
            var_info_is_exist_global(v_cur)) { $o return true; }
    }

$o  return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_is_var_exist_local(const prog_info *const prog, const token *const tkn)
{
$i
    log_verify(prog != nullptr, false);
    log_verify(tkn  != nullptr, false);

    log_verify(tkn->type == TOKEN_NAME, false);

$   var_info *v_cur = (var_info *) vector_begin($v_store);
$   var_info *v_end = (var_info *) vector_end  ($v_store);

$   for (; v_cur != v_end; ++v_cur)
    {
        if (var_info_is_equal      (v_cur,    tkn) &&
            var_info_is_exist_local(v_cur, $scope)) { $o return true; }
    }

$o  return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_is_func_exist(const prog_info *const prog, const token *const tkn)
{
$i
    log_verify(prog != nullptr, false);
    log_verify(tkn  != nullptr, false);

    log_verify(tkn->type == TOKEN_NAME, false);

$   func_info *f_cur = (func_info *) vector_begin($f_store);
$   func_info *f_end = (func_info *) vector_end  ($f_store);

$   for (; f_cur != f_end; ++f_cur)
        if (func_info_is_equal(f_cur, tkn)) { $o return true; }

$o  return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
// is_possible
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool prog_info_is_name_decl_possible(const prog_info *const prog, const token *const tkn)
{
$i
    log_verify(prog != nullptr, false);
    log_verify(tkn  != nullptr, false);

    log_verify(tkn->type == TOKEN_NAME, false);

$   bool   result = !prog_info_is_var_exist_local(prog, tkn) && !prog_info_is_func_exist(prog, tkn);
$o  return result;
}

//--------------------------------------------------------------------------------------------------------------------------------
// get_index
//--------------------------------------------------------------------------------------------------------------------------------

static size_t prog_info_get_var_index(const prog_info *const prog, const token *const tkn)
{
$i
    log_verify(prog != nullptr, -1UL);
    log_verify(tkn  != nullptr, -1UL);

    log_verify(tkn->type == TOKEN_NAME, -1UL);

$   var_info *v_cur = (var_info *) vector_begin($v_store);
$   var_info *v_end = (var_info *) vector_end  ($v_store);
    size_t    index = 0;

$   for (; v_cur != v_end; ++v_cur)
    {
        if (var_info_is_equal(v_cur, tkn)) { $o return index; }
        ++index;
    }

$o  return -1UL;
}

//--------------------------------------------------------------------------------------------------------------------------------

static size_t prog_info_get_func_index(const prog_info *const prog, const token *const tkn)
{
$i
    log_verify(prog != nullptr, -1UL);
    log_verify(tkn  != nullptr, -1UL);

    log_verify(tkn->type == TOKEN_NAME, -1UL);

$   func_info *f_cur = (func_info *) vector_begin($f_store);
$   func_info *f_end = (func_info *) vector_end  ($f_store);
    size_t     index = 0;

$   for (; f_cur != f_end; ++f_cur)
    {
        if (func_info_is_equal(f_cur, tkn)) { $o return index; }
    }

$o  return -1UL;
}

//--------------------------------------------------------------------------------------------------------------------------------
// push pop
//--------------------------------------------------------------------------------------------------------------------------------

static size_t prog_info_var_push(prog_info *const prog, const token *const tkn)
{
$i
    log_verify(prog != nullptr, -1UL);
    log_verify(tkn  != nullptr, -1UL);

    log_verify(tkn->type == TOKEN_NAME, -1UL);

$   size_t var_index = prog_info_get_var_index(prog, tkn);
    if    (var_index != -1UL)
    {
        var_info_scope_push((var_info *) vector_begin($v_store) + var_index, $scope);
$o      return var_index;
    }

    var_index = $v_store->size;

    var_info       var_new = {};
$   var_info_ctor(&var_new, $tkn_name, tkn->size, $scope);

$   vector_push_back($v_store, &var_new);

$o  return var_index;
}

//--------------------------------------------------------------------------------------------------------------------------------

static size_t prog_info_func_push_back(prog_info *const prog, const token *const tkn)
{
$i
    log_verify(prog != nullptr, -1UL);
    log_verify(tkn  != nullptr, -1UL);

    log_verify(tkn->type == TOKEN_NAME, -1UL);

$   size_t func_index = prog_info_get_func_index(prog, tkn);
    if    (func_index != -1UL) { $o return func_index; }

    func_index = $f_store->size;

    func_info       func_new = {};
$   func_info_ctor(&func_new, $tkn_name, tkn->size);

$   vector_push_back($f_store, &func_new);

$o  return func_index;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline void prog_info_func_pop_back(prog_info *const prog)
{
$i
    log_verify(prog != nullptr, (void) 0);
$   log_verify(!vector_is_empty($f_store), (void) 0);

$   vector_pop_back($f_store);
$o
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
$i
    log_verify(prog != nullptr, (void) 0);

$   var_info *v_cur = (var_info *) vector_begin($v_store);
$   var_info *v_end = (var_info *) vector_end  ($v_store);

$   for (; v_cur != v_end; ++v_cur)
        var_info_scope_pop(v_cur, $scope);

    $scope--;
$o
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
$i
    log_assert(prog != nullptr);
    log_assert(tkn  != nullptr);

    log_assert(tkn->type == TOKEN_NAME);

$   $main_id = (strcmp(tkn->value.name, MAIN_FUNC_NAME) == 0) ? func_id : -1UL;

    bool   result = $is_ret; $is_ret = false;
$o  return result;
}
