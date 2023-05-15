#ifndef PARSER_STATIC_H
#define PARSER_STATIC_H

#include "common.h"
#include "tokenizer.h"
#include "parser.h"

//================================================================================================================================
// PARSER
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// GLOBAL
//--------------------------------------------------------------------------------------------------------------------------------

enum PARSE_VERDICT
{
    PARSE_VERDICT_SUCCESS,
    PARSE_VERDICT_FAIL   ,
    PARSE_VERDICT_ERROR  ,
};

//================================================================================================================================
// TOKEN_ARR_PASS
//================================================================================================================================

struct token_arr_pass
{
    const token *arr_beg;
    const token *arr_pos;
    const token *arr_end;
};

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION
//--------------------------------------------------------------------------------------------------------------------------------

static bool            token_arr_pass_ctor(token_arr_pass *const tkn_pass, const vector *const tkn_arr);
static token_arr_pass *token_arr_pass_new (                                const vector *const tkn_arr);

static void token_arr_pass_forward (token_arr_pass *const tkn_pass);
static void token_arr_pass_backward(token_arr_pass *const tkn_pass);

static bool token_arr_pass_is_passed(const token_arr_pass *const tkn_pass);

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
// FUNCTION
//--------------------------------------------------------------------------------------------------------------------------------

static bool      var_info_ctor      (var_info *const var, const char *name, const size_t size, const size_t scope);
static var_info *var_info_new       (                     const char *name, const size_t size, const size_t scope);

static bool var_info_scope_push(var_info *const var, const size_t scope);
static bool var_info_scope_pop (var_info *const var, const size_t scope);

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
// FUNCTION
//--------------------------------------------------------------------------------------------------------------------------------

static bool       func_info_ctor    (func_info *const func, const char *name, const size_t size);
static func_info *func_info_new     (                       const char *name, const size_t size);
static bool       func_info_arg_push(func_info *const func,                   const size_t  arg);

static bool func_info_is_equal(const func_info *const func, const token *const tkn);

//================================================================================================================================
// PROG_INFO
//================================================================================================================================

struct prog_info
{
    vector *var_storage;
    vector *func_storage;

    size_t scope;
    bool is_main_func;
    bool is_return_op;
};

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION
//--------------------------------------------------------------------------------------------------------------------------------

static bool       prog_info_ctor(prog_info *const prog);
static prog_info *prog_info_new ();

static bool prog_info_is_var_exist_global(const prog_info *const prog, const token *const tkn);
static bool prog_info_is_var_exist_local (const prog_info *const prog, const token *const tkn);
static bool prog_info_is_func_exist      (const prog_info *const prog, const token *const tkn);

static size_t prog_info_get_var_index (const prog_info *const prog, const token *const tkn);
static size_t prog_info_get_func_index(const prog_info *const prog, const token *const tkn);

static void prog_info_var_push_forced (prog_info *const prog, const token *const tkn);
static void prog_info_func_push_forced(prog_info *const prog, const token *const tkn);

static inline void prog_info_scope_open (prog_info *const prog);
static        void prog_info_scope_close(prog_info *const prog);
static inline void prog_info_meet_return(prog_info *const prog);

#endif //PARSER_STATIC_H
