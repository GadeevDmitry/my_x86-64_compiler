#ifndef PARSER_STATIC_H
#define PARSER_STATIC_H

#include "common.h"
#include "tokenizer.h"
#include "parser.h"

//================================================================================================================================
// TOKEN_ARR_PASS
//================================================================================================================================

struct token_arr_pass
{
    const token *arr_pos;
    const token *arr_end;
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor, dtor
//--------------------------------------------------------------------------------------------------------------------------------

static bool            token_arr_pass_ctor  (token_arr_pass *const tkn_pass, const vector *const tkn_arr);
static token_arr_pass *token_arr_pass_new   (                                const vector *const tkn_arr);
static void            token_arr_pass_delete(token_arr_pass *const tkn_pass);

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

static inline void token_arr_pass_next     (      token_arr_pass *const tkn_pass);
static inline void token_arr_pass_reset    (      token_arr_pass *const tkn_pass, const token *const reset_val);
static inline bool token_arr_pass_is_passed(const token_arr_pass *const tkn_pass);

//================================================================================================================================
// VAR_INFO
//================================================================================================================================

struct var_info
{
    const char *name;
    size_t      size;
    stack     *scope;
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

static bool      var_info_ctor(var_info *const var, const char *name, const size_t size, const size_t scope);
static var_info *var_info_new (                     const char *name, const size_t size, const size_t scope);

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void var_info_dtor  (void *const _var);
static void var_info_delete(void *const _var);

//--------------------------------------------------------------------------------------------------------------------------------
// scope
//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_scope_push(var_info *const var, const size_t scope);
static bool var_info_scope_pop (var_info *const var, const size_t scope);

//--------------------------------------------------------------------------------------------------------------------------------
// is_...
//--------------------------------------------------------------------------------------------------------------------------------

static bool        var_info_is_equal       (const var_info *const var, const token *const tkn);
static inline bool var_info_is_exist_global(const var_info *const var);
static inline bool var_info_is_exist_local (const var_info *const var, const size_t scope);

//================================================================================================================================
// FUNC_INFO
//================================================================================================================================

struct func_info
{
    const char *name;
    size_t      size;
    vector     *args;
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

static bool       func_info_ctor(func_info *const func, const char *name, const size_t size);
static func_info *func_info_new (                       const char *name, const size_t size);

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void func_info_dtor  (void *const _func);
static void func_info_delete(void *const _func);

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

static bool func_info_arg_push(      func_info *const func, const size_t       arg);
static bool func_info_is_equal(const func_info *const func, const token *const tkn);

//================================================================================================================================
// PROG_INFO
//================================================================================================================================

struct prog_info
{
    vector *var_storage;
    vector *func_storage;

    size_t scope;
    size_t main_func_id;
    bool   is_return_op;
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

static bool       prog_info_ctor(prog_info *const prog);
static prog_info *prog_info_new ();

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void prog_info_dtor  (prog_info *const prog);
static void prog_info_delete(prog_info *const prog);

//--------------------------------------------------------------------------------------------------------------------------------
// is_exist
//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_is_var_exist_global(const prog_info *const prog, const token *const tkn);
static bool prog_info_is_var_exist_local (const prog_info *const prog, const token *const tkn);
static bool prog_info_is_func_exist      (const prog_info *const prog, const token *const tkn);

//--------------------------------------------------------------------------------------------------------------------------------
// is_possible
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool prog_info_is_name_decl_possible(const prog_info *const prog, const token *const tkn);

//--------------------------------------------------------------------------------------------------------------------------------
// get_index
//--------------------------------------------------------------------------------------------------------------------------------

static size_t prog_info_get_var_index (const prog_info *const prog, const token *const tkn);
static size_t prog_info_get_func_index(const prog_info *const prog, const token *const tkn);

//--------------------------------------------------------------------------------------------------------------------------------
// push
//--------------------------------------------------------------------------------------------------------------------------------

static      size_t prog_info_var_push      (prog_info *const prog, const token *const tkn);
static      size_t prog_info_func_push_back(prog_info *const prog, const token *const tkn);
static inline void prog_info_func_pop_back (prog_info *const prog);

//--------------------------------------------------------------------------------------------------------------------------------
// scope
//--------------------------------------------------------------------------------------------------------------------------------

static inline void prog_info_scope_open (prog_info *const prog);
static        void prog_info_scope_close(prog_info *const prog);

//--------------------------------------------------------------------------------------------------------------------------------
// other
//--------------------------------------------------------------------------------------------------------------------------------

static inline void prog_info_meet_return(prog_info *const prog);
static inline bool prog_info_func_exit  (prog_info *const prog, const token *const tkn, const size_t func_id);

//================================================================================================================================
// PARSER
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// GLOBAL
//--------------------------------------------------------------------------------------------------------------------------------

static const char MAIN_FUNC_NAME[] = "CAMP_NOU";

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_general           (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_var_decl          (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_func_decl         (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_args              (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
//--------------------------------------------------------------------------------------------------------------------------------
static bool parse_first_arg         (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_other_arg         (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
//--------------------------------------------------------------------------------------------------------------------------------
static bool parse_operators         (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_assignment_op     (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_input             (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_output            (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_if                (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
//--------------------------------------------------------------------------------------------------------------------------------
static bool parse_then_else         (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_else              (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
//--------------------------------------------------------------------------------------------------------------------------------
static bool parse_while             (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_return            (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_scope             (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_rvalue            (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_assignment        (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
//--------------------------------------------------------------------------------------------------------------------------------
static bool parse_single_assignment (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
//--------------------------------------------------------------------------------------------------------------------------------
static bool   parse_op_pattern      (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree ,
       bool (*parse_op_lower)       (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree),
       bool (*parse_op_token)                                 (token_arr_pass *const tkn_pass, AST_node **const subtree));
//--------------------------------------------------------------------------------------------------------------------------------
static bool parse_log_or            (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_log_or_token                                (token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_log_and           (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_log_and_token                               (token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_equal             (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_equal_token                                 (token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_cmp               (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_cmp_token                                   (token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_add_sub           (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_add_sub_token                               (token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_mul_div           (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_mul_div_token                               (token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_pow               (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_pow_token                                   (token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_not               (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_operand           (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_rvalue_scope      (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_func_call_op      (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_func_call         (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_params            (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
//--------------------------------------------------------------------------------------------------------------------------------
static bool parse_first_param       (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree) __attribute__((always_inline));
static bool parse_other_param       (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
//--------------------------------------------------------------------------------------------------------------------------------
static bool parse_rvalue_elem       (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_lvalue            (prog_info *const program, token_arr_pass *const tkn_pass, AST_node **const subtree);

#endif //PARSER_STATIC_H
