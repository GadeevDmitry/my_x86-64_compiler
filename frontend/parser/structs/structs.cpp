#include <stdio.h>
#include <string.h>

#include "structs.h"

//================================================================================================================================
// TOKEN_ARR_PASS
//================================================================================================================================

#define $pos (tkn_pass->arr_pos)
#define $end (tkn_pass->arr_end)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

bool token_arr_pass_ctor(token_arr_pass *const tkn_pass, const vector *const tkn_arr)
{
    LOG_VERIFY   (tkn_pass != nullptr, false);
    VECTOR_VERIFY(tkn_arr            , false);

    $pos = (token *) vector_begin(tkn_arr);
    $end = (token *) vector_end  (tkn_arr) - 1; // последний токен - фиктивный

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

token_arr_pass *token_arr_pass_new(const vector *const tkn_arr)
{
    token_arr_pass *tkn_pass_new = (token_arr_pass *) LOG_CALLOC(1, sizeof(token_arr_pass));
    LOG_VERIFY     (tkn_pass_new != nullptr, nullptr);

    if (!token_arr_pass_ctor(tkn_pass_new, tkn_arr)) { LOG_FREE(tkn_pass_new); return nullptr; }
    return tkn_pass_new;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

void token_arr_pass_delete(token_arr_pass *const tkn_pass)
{
    if (tkn_pass == nullptr) return;
    LOG_FREE(tkn_pass);
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $pos
#undef $end

#define $tkn_type   (tkn->type)
#define $tkn_size   (tkn->size)
#define $tkn_name   (tkn->value.name)

//================================================================================================================================
// VAR_INFO
//================================================================================================================================

#define $name       (var->name)
#define $name_size  (var->name_size)
#define $frame      (var->frame)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

bool var_info_ctor(var_info *const var, const char  *name,
                                        const size_t name_size,
                                        const size_t frame)
{
    LOG_VERIFY(var  != nullptr, false);
    LOG_VERIFY(name != nullptr, false);

    $name      = name;
    $name_size = name_size;
    $frame     = stack_new(sizeof(size_t));

    if ($frame == nullptr) return false;

    return stack_push($frame, &frame);
}

//--------------------------------------------------------------------------------------------------------------------------------

var_info *var_info_new(const char  *name,
                       const size_t name_size,
                       const size_t frame)
{
    LOG_VERIFY(name != nullptr, nullptr);

    var_info  *var_new = (var_info *) LOG_CALLOC(1, sizeof(var_info));
    LOG_VERIFY(var_new != nullptr, nullptr);

    if (!var_info_ctor(var_new, name, name_size, frame)) { LOG_FREE(var_new); return nullptr; }
    return var_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool var_info_ctor(var_info *const var, const token *const tkn, const size_t frame)
{
    LOG_VERIFY(var != nullptr, false);
    LOG_VERIFY(tkn != nullptr, false);
    LOG_VERIFY($tkn_type == TOKEN_NAME, false);

    return var_info_ctor(var, $tkn_name, $tkn_size, frame);
}

//--------------------------------------------------------------------------------------------------------------------------------

var_info *var_info_new(const token *const tkn, const size_t frame)
{
    LOG_VERIFY(tkn != nullptr, nullptr);
    LOG_VERIFY($tkn_type == TOKEN_NAME, nullptr);

    return var_info_new($tkn_name, $tkn_size, frame);
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

void var_info_dtor(void *const _var)
{
    if (_var == nullptr) return;

    var_info *const var = (var_info *) _var;
    stack_delete($frame);
}

//--------------------------------------------------------------------------------------------------------------------------------

void var_info_delete(void *const _var)
{
    var_info_dtor(_var);
    LOG_FREE     (_var);
}

//--------------------------------------------------------------------------------------------------------------------------------
// frame
//--------------------------------------------------------------------------------------------------------------------------------

bool var_info_frame_delete(var_info *const var, const size_t frame)
{
    LOG_VERIFY(var != nullptr, false);

    if (stack_is_empty($frame)) return true;

    size_t top_frame = 0;
    stack_front($frame, &top_frame);

    if (top_frame == frame) return stack_pop($frame);
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// is_...
//--------------------------------------------------------------------------------------------------------------------------------

bool var_info_is_name_equal(const var_info *const var, const token *const tkn)
{
    LOG_VERIFY(var != nullptr, false);
    LOG_VERIFY(tkn != nullptr, false);
    LOG_VERIFY($tkn_type == TOKEN_NAME, false);

    if ($tkn_size == $name_size && strncmp($name, $tkn_name, $name_size) == 0) return true;
    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool var_info_is_exist_local(const var_info *const var, const size_t loc_frame)
{
    LOG_VERIFY(var != nullptr, false);

    if (!var_info_is_exist(var)) return false;

    size_t top_frame = 0UL;
    stack_front($frame, &top_frame);

    return top_frame == loc_frame;
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $name
#undef $name_size
#undef $frame

//================================================================================================================================
// FUNC_INFO
//================================================================================================================================

#define $name       (func->name)
#define $name_size  (func->name_size)
#define $args       (func->args_quantity)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

bool func_info_ctor(func_info *const func, const char  *name,
                                           const size_t name_size,
                                           const size_t args_quantity /* = 0UL */)
{
    LOG_VERIFY(func != nullptr, false);
    LOG_VERIFY(name != nullptr, false);

    $name      = name;
    $name_size = name_size;
    $args      = args_quantity;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

func_info *func_info_new(const char  *name,
                         const size_t name_size,
                         const size_t args_quantity /* = 0UL */)
{
    LOG_VERIFY(name != nullptr, nullptr);

    func_info *func_new = (func_info *) LOG_CALLOC(1, sizeof(func_info));
    LOG_VERIFY(func_new != nullptr, nullptr);

    if (!func_info_ctor(func_new, name, name_size, args_quantity)) { LOG_FREE(func_new); return nullptr; }
    return func_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool func_info_ctor(func_info *const func, const token *const tkn)
{
    LOG_VERIFY(func != nullptr, false);
    LOG_VERIFY(tkn  != nullptr, false);

    return func_info_ctor(func, $tkn_name, $tkn_size);
}

//--------------------------------------------------------------------------------------------------------------------------------

func_info *func_info_new(const token *const tkn)
{
    LOG_VERIFY(tkn != nullptr, nullptr);

    return func_info_new($tkn_name, $tkn_size);
}

//--------------------------------------------------------------------------------------------------------------------------------

void func_info_delete(void *const _func)
{
    LOG_FREE(_func);
}

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

bool func_info_is_equal(const func_info *const func, const func_info *const pattern)
{
    LOG_VERIFY(func    != nullptr, false);
    LOG_VERIFY(pattern != nullptr, false);

    if (pattern->args_quantity == $args      &&
        pattern->name_size     == $name_size &&
        strncmp(pattern->name, $name, $name_size) == 0) return true;

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool func_info_is_name_equal(const func_info *const func, const token *const tkn)
{
    LOG_VERIFY(func != nullptr, false);
    LOG_VERIFY(tkn  != nullptr, false);

    if ($tkn_size == $name_size && strncmp($name, $tkn_name, $name_size) == 0) return true;
    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $name
#undef $name_size
#undef $args

//================================================================================================================================
// PROG_INFO
//================================================================================================================================

#define $v_store    (prog->var_storage)
#define $f_store    (prog->func_storage)

#define $frame      (prog->frame)
#define $main_info  (prog->main_func_info)
#define $main_id    (prog->main_func_id)
#define $is_ret     (prog->is_return_op)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_ctor(prog_info *const prog, const char *const main_func_name)
{
    LOG_VERIFY(prog != nullptr, false);
    LOG_VERIFY(main_func_name != nullptr, false);

    $v_store = vector_new(sizeof(var_info ), nullptr,  var_info_dtor);
    $f_store = vector_new(sizeof(func_info), nullptr);

    if ($v_store == nullptr) { vector_delete($f_store); return false; }
    if ($f_store == nullptr) { vector_delete($v_store); return false; }

    var_info_ctor(&$main_info, main_func_name, strlen(main_func_name), 0UL);

    $main_id =  -1UL;
    $frame   =     0;
    $is_ret  = false;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

prog_info *prog_info_new(const char *const main_func_name)
{
    prog_info *prog_new = (prog_info *) LOG_CALLOC(1, sizeof(prog_info));
    LOG_VERIFY(prog_new != nullptr, nullptr);

    if (!prog_info_ctor(prog_new, main_func_name)) { LOG_FREE(prog_new); return nullptr; }
    return prog_new;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

void prog_info_dtor(prog_info *const prog)
{
    if (prog == nullptr) return;

    vector_delete  ($v_store);
    vector_delete  ($f_store);

    var_info_dtor(&$main_info);
}

//--------------------------------------------------------------------------------------------------------------------------------

void prog_info_delete(prog_info *const prog)
{
    prog_info_dtor(prog);
    LOG_FREE      (prog);
}

//--------------------------------------------------------------------------------------------------------------------------------
// is_exist
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_is_var_exist(const prog_info *const prog, const token *const tkn)
{
    LOG_VERIFY(prog != nullptr, false);
    LOG_VERIFY(tkn  != nullptr, false);
    LOG_VERIFY($tkn_type == TOKEN_NAME, false);

    var_info *v_cur = (var_info *) vector_begin($v_store);
    var_info *v_end = (var_info *) vector_end  ($v_store);

    for (; v_cur != v_end; ++v_cur)
    {
        if (var_info_is_name_equal(v_cur, tkn) &&
            var_info_is_exist     (v_cur)) return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_is_var_exist_local(const prog_info *const prog, const token *const tkn)
{
    LOG_VERIFY(prog != nullptr, false);
    LOG_VERIFY(tkn  != nullptr, false);
    LOG_VERIFY($tkn_type == TOKEN_NAME, false);

    var_info *v_cur = (var_info *) vector_begin($v_store);
    var_info *v_end = (var_info *) vector_end  ($v_store);

    for (; v_cur != v_end; ++v_cur)
    {
        if (var_info_is_name_equal (v_cur,    tkn) &&
            var_info_is_exist_local(v_cur, $frame)) return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_is_func_exist(const prog_info *const prog, const func_info *const func)
{
    LOG_VERIFY(prog != nullptr, false);
    LOG_VERIFY(func != nullptr, false);

    func_info *f_cur = (func_info *) vector_begin($f_store);
    func_info *f_end = (func_info *) vector_end  ($f_store);

    for (; f_cur != f_end; ++f_cur)
        if (func_info_is_equal(f_cur, func)) return true;

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_is_func_name_exist(const prog_info *const prog, const token *const tkn)
{
    LOG_VERIFY(prog != nullptr, false);
    LOG_VERIFY(tkn  != nullptr, false);

    func_info *f_cur = (func_info *) vector_begin($f_store);
    func_info *f_end = (func_info *) vector_end  ($f_store);

    for (; f_cur != f_end; ++f_cur)
        if (func_info_is_name_equal(f_cur, tkn)) return true;

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
// get_index
//--------------------------------------------------------------------------------------------------------------------------------

size_t prog_info_get_var_index(const prog_info *const prog, const token *const tkn)
{
    LOG_VERIFY(prog != nullptr, -1UL);
    LOG_VERIFY(tkn  != nullptr, -1UL);
    LOG_VERIFY(tkn->type == TOKEN_NAME, -1UL);

    var_info *v_cur = (var_info *) vector_begin($v_store);
    var_info *v_end = (var_info *) vector_end  ($v_store);
    size_t    index = 0;

    for (; v_cur != v_end; ++v_cur)
    {
        if (var_info_is_name_equal(v_cur, tkn)) return index;
        ++index;
    }

    return -1UL;
}

//--------------------------------------------------------------------------------------------------------------------------------

size_t prog_info_get_func_index(const prog_info *const prog, const func_info *const func)
{
    LOG_VERIFY(prog != nullptr, -1UL);
    LOG_VERIFY(func != nullptr, -1UL);

    func_info *f_cur = (func_info *) vector_begin($f_store);
    func_info *f_end = (func_info *) vector_end  ($f_store);
    size_t     index = 0;

    for (; f_cur != f_end; ++f_cur)
    {
        if (func_info_is_equal(f_cur, func)) return index;
        ++index;
    }

    return -1UL;
}

//--------------------------------------------------------------------------------------------------------------------------------
// push
//--------------------------------------------------------------------------------------------------------------------------------

size_t prog_info_var_push(prog_info *const prog, const token *const tkn)
{
    LOG_VERIFY(prog != nullptr, -1UL);
    LOG_VERIFY(tkn  != nullptr, -1UL);
    LOG_VERIFY(tkn->type == TOKEN_NAME, -1UL);

    size_t var_index = prog_info_get_var_index(prog, tkn);
    if    (var_index != -1UL)
    {
        var_info_frame_new((var_info *) vector_begin($v_store) + var_index, $frame);
        return var_index;
    }

    var_index = $v_store->size;

    var_info var_new = {};
    var_info_ctor(&var_new, tkn, $frame);
    vector_push_back($v_store, &var_new);

    return var_index;
}

//--------------------------------------------------------------------------------------------------------------------------------

size_t prog_info_func_push(prog_info *const prog, const func_info *const func)
{
    LOG_VERIFY(prog != nullptr, -1UL);
    LOG_VERIFY(func != nullptr, -1UL);

    size_t func_index = prog_info_get_func_index(prog, func);
    if    (func_index != -1UL) return func_index;

    func_index = $f_store->size;
    vector_push_back($f_store, func);

    return func_index;
}

//--------------------------------------------------------------------------------------------------------------------------------
// scope handler
//--------------------------------------------------------------------------------------------------------------------------------

void prog_info_scope_close(prog_info *const prog)
{
    LOG_VERIFY(prog != nullptr, (void) 0);

    var_info *v_cur = (var_info *) vector_begin($v_store);
    var_info *v_end = (var_info *) vector_end  ($v_store);

    for (; v_cur != v_end; ++v_cur)
        var_info_frame_delete(v_cur, $frame);

    $frame--;
}

//--------------------------------------------------------------------------------------------------------------------------------
// other
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_func_exit(prog_info *const prog)
{
    LOG_ASSERT(prog != nullptr);

    const func_info *func    = (const func_info *) vector_end($f_store) - 1;
    const size_t     func_id = $f_store->size - 1;

    $main_id = (func->name_size ==  $main_info.name_size &&
                strncmp(func->name, $main_info.name, func->name_size) == 0) ? func_id : -1UL;

    bool result = $is_ret;
    $is_ret     =   false;

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $v_store
#undef $f_store
#undef $scope
#undef $main_id
#undef $is_ret
