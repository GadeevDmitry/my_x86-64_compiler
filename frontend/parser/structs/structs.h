#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>

#include "../../tokenizer/tokenizer.h"

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "../../../lib/logs/log.h"
#include "../../../lib/vector/vector.h"
#include "../../../lib/stack/stack.h"

//================================================================================================================================
// TOKEN_ARR_PASS
//================================================================================================================================

struct token_arr_pass
{
    const token *arr_pos;
    const token *arr_end;
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

bool            token_arr_pass_ctor  (token_arr_pass *const tkn_pass, const vector *const tkn_arr);
token_arr_pass *token_arr_pass_new   (                                const vector *const tkn_arr);
void            token_arr_pass_delete(token_arr_pass *const tkn_pass);

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

static inline void token_arr_pass_next(token_arr_pass *const tkn_pass);
static inline void token_arr_pass_next(token_arr_pass *const tkn_pass)
{
    log_verify(tkn_pass != nullptr, (void) 0);

    tkn_pass->arr_pos++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline void token_arr_pass_reset(token_arr_pass *const tkn_pass, const token *const reset_val);
static inline void token_arr_pass_reset(token_arr_pass *const tkn_pass, const token *const reset_val)
{
    log_verify(tkn_pass != nullptr, (void) 0);

    tkn_pass->arr_pos = reset_val;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_arr_pass_is_passed(const token_arr_pass *const tkn_pass);
static inline bool token_arr_pass_is_passed(const token_arr_pass *const tkn_pass)
{
    log_verify(tkn_pass != nullptr, false);

    return tkn_pass->arr_pos == tkn_pass->arr_end;
}

//================================================================================================================================
// { VAR_INFO
//================================================================================================================================

struct var_info
{
    const char *name;
    size_t      name_size;
    stack      *frame;
};

//================================================================================================================================
// HEADERS
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

bool      var_info_ctor(var_info *const var, const char *name, const size_t name_size, const size_t frame);
var_info *var_info_new (                     const char *name, const size_t name_size, const size_t frame);

bool      var_info_ctor(var_info *const var, const token *const tkn, const size_t frame);
var_info *var_info_new (                     const token *const tkn, const size_t frame);

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

void var_info_dtor  (void *const _var);
void var_info_delete(void *const _var);

//--------------------------------------------------------------------------------------------------------------------------------
// frame
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool var_info_frame_new   (var_info *const var, const size_t frame);
              bool var_info_frame_delete(var_info *const var, const size_t frame);

//--------------------------------------------------------------------------------------------------------------------------------
// is_...
//--------------------------------------------------------------------------------------------------------------------------------

              bool var_info_is_name_equal (const var_info *const var, const token *const tkn);
              bool var_info_is_exist_local(const var_info *const var, const size_t frame);
static inline bool var_info_is_exist      (const var_info *const var);

//================================================================================================================================
// BODIES
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// frame
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool var_info_frame_new(var_info *const var, const size_t frame)
{
    log_verify(var != nullptr, false);

    return stack_push(var->frame, &frame);
}

//--------------------------------------------------------------------------------------------------------------------------------
// is_...
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool var_info_is_exist(const var_info *const var)
{
    log_verify(var != nullptr, false);

    return !stack_is_empty(var->frame);
}

// } VAR_INFO
//================================================================================================================================

//================================================================================================================================
// { FUNC_INFO
//================================================================================================================================

struct func_info
{
    const char *name;
    size_t      name_size;
    size_t      args_quantity;
};

//================================================================================================================================
// HEADERS
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

bool       func_info_ctor  (func_info *const func, const char *name, const size_t name_size, const size_t args_quantity = 0UL);
func_info *func_info_new   (                       const char *name, const size_t name_size, const size_t args_quantity = 0UL);

bool       func_info_ctor  (func_info *const func, const token *const tkn);
func_info *func_info_new   (                       const token *const tkn);

void       func_info_delete(void *const _func);

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool func_info_add_arg      (      func_info *const func);
static inline bool func_info_set_arg      (      func_info *const func, const size_t args_quantity);
              bool func_info_is_name_equal(const func_info *const func, const token *const tkn);
              bool func_info_is_equal     (const func_info *const func, const func_info *const pattern);

//================================================================================================================================
// BODIES
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool func_info_add_arg(func_info *const func)
{
    log_verify(func != nullptr, false);

    func->args_quantity++;
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool func_info_set_arg(func_info *const func, const size_t args_quantity)
{
    log_verify(func != nullptr, false);

    func->args_quantity = args_quantity;
    return true;
}

// } FUNC_INFO
//================================================================================================================================

//================================================================================================================================
// { PROG_INFO
//================================================================================================================================

struct prog_info
{
    vector * var_storage;
    vector *func_storage;

    size_t      main_func_id;
    var_info    main_func_info;

    size_t      frame;
    bool        is_return_op;
};

//================================================================================================================================
// HEADERS
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

bool       prog_info_ctor  (prog_info *const prog, const char *const main_func_name);
prog_info *prog_info_new   (                       const char *const main_func_name);
void       prog_info_dtor  (prog_info *const prog);
void       prog_info_delete(prog_info *const prog);

//--------------------------------------------------------------------------------------------------------------------------------
// is_exist
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_is_var_exist      (const prog_info *const prog, const token *const tkn);
bool prog_info_is_var_exist_local(const prog_info *const prog, const token *const tkn);

bool prog_info_is_func_exist     (const prog_info *const prog, const func_info *const func);
bool prog_info_is_func_name_exist(const prog_info *const prog, const token     *const  tkn);

//--------------------------------------------------------------------------------------------------------------------------------
// is_possible
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool prog_info_is_name_decl_possible  (const prog_info *const prog, const token *const tkn);
static inline bool prog_info_is_name_access_possible(const prog_info *const prog, const token *const tkn);

//--------------------------------------------------------------------------------------------------------------------------------
// get_index
//--------------------------------------------------------------------------------------------------------------------------------

size_t prog_info_get_var_index (const prog_info *const prog, const token     *const  tkn);
size_t prog_info_get_func_index(const prog_info *const prog, const func_info *const func);

//--------------------------------------------------------------------------------------------------------------------------------
// push
//--------------------------------------------------------------------------------------------------------------------------------

size_t prog_info_var_push (prog_info *const prog, const token     *const  tkn);
size_t prog_info_func_push(prog_info *const prog, const func_info *const func);

//--------------------------------------------------------------------------------------------------------------------------------
// scope handler
//--------------------------------------------------------------------------------------------------------------------------------

static inline void prog_info_scope_open (prog_info *const prog);
              void prog_info_scope_close(prog_info *const prog);

//--------------------------------------------------------------------------------------------------------------------------------
// other
//--------------------------------------------------------------------------------------------------------------------------------

static inline void prog_info_meet_return(prog_info *const prog);
              bool prog_info_func_exit  (prog_info *const prog);

//================================================================================================================================
// BODIES
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// is_possible
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool prog_info_is_name_decl_possible(const prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, false);
    log_verify(tkn  != nullptr, false);
    log_verify(tkn->type == TOKEN_NAME, false);

    return !prog_info_is_var_exist_local(prog, tkn) && !prog_info_is_func_name_exist(prog, tkn);
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool prog_info_is_name_access_possible(const prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, false);
    log_verify(tkn  != nullptr, false);
    log_verify(tkn->type == TOKEN_NAME, false);

    return prog_info_is_var_exist(prog, tkn) || prog_info_is_func_name_exist(prog, tkn);
}

//--------------------------------------------------------------------------------------------------------------------------------
// scope handler
//--------------------------------------------------------------------------------------------------------------------------------

static inline void prog_info_scope_open(prog_info *const prog)
{
    log_verify(prog != nullptr, (void) 0);

    prog->frame++;
}

//--------------------------------------------------------------------------------------------------------------------------------
// other
//--------------------------------------------------------------------------------------------------------------------------------

static inline void prog_info_meet_return(prog_info *const prog)
{
    log_verify(prog != nullptr, (void) 0);

    if (prog->frame == 1) prog->is_return_op = true;
}

// } PROG_INFO
//================================================================================================================================

#endif //STRUCTS_H
