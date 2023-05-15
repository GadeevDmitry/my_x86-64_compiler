#include "parser_static.h"

//================================================================================================================================
// { PROG, FUNC, VAR }_INFO
//================================================================================================================================

#define $tkn_name   (tkn->value.name)
#define $tkn_imm    (tkn->value.imm_int)
#define $tkn_key    (tkn->value.key)
#define $tkn_op     (tkn->value.op)

//--------------------------------------------------------------------------------------------------------------------------------
// VAR_INFO
//--------------------------------------------------------------------------------------------------------------------------------

#define $name   (var->name)
#define $size   (var->size)
#define $scope  (var->scope)

//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_ctor(var_info *const var, const char *name, const size_t size, const size_t scope)
{
    log_verify(var  != nullptr, false);
    log_verify(name != nullptr, false);

    $name  = name;
    $size  = size;
    $scope = stack_new(sizeof(size_t));

    if ($scope == nullptr) return false;
    return stack_push($scope, &scope);
}

//--------------------------------------------------------------------------------------------------------------------------------

static var_info *var_info_new(const char *name, const size_t size, const size_t scope)
{
    log_verify(name != nullptr, nullptr);

    var_info  *var_new = (var_info *) log_calloc(1, sizeof(var_info));
    log_verify(var_new != nullptr, nullptr);

    if (!var_info_ctor(var_new, name, size, scope)) { log_free(var_new); return nullptr; }
    return var_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_scope_push(var_info *const var, const size_t scope)
{
    log_verify(var != nullptr, false);

    return stack_push($scope, &scope);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_scope_pop(var_info *const var, const size_t scope)
{
    log_verify(var != nullptr, false);

    size_t scope_top = 0;

    if (!stack_front($scope, &scope_top)) return false;

    if (scope_top == scope) return stack_pop($scope);
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool var_info_is_equal(const var_info *const var, const token *const tkn)
{
    log_assert(var != nullptr);
    log_assert(tkn != nullptr);

    log_assert(tkn->type == TOKEN_NAME);
    log_verify($tkn_name !=    nullptr, false);

    if (tkn->size == $size && strncmp($name, $tkn_name, $size) == 0) return true;
    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool var_info_is_exist_global(const var_info *const var)
{
    log_assert(var != nullptr);

    return !stack_is_empty($scope);
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

//--------------------------------------------------------------------------------------------------------------------------------
// FUNC_INFO
//--------------------------------------------------------------------------------------------------------------------------------

#define $name   (func->name)
#define $size   (func->size)
#define $args   (func->args)

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

static func_info *func_info_new(const char *name, const size_t size)
{
    log_verify(name != nullptr, nullptr);

    func_info *func_new = (func_info *) log_calloc(1, sizeof(func_info));
    log_verify(func_new != nullptr, nullptr);

    if (!func_info_ctor(func_new, name, size)) { log_free(func_new); return nullptr; }
    return func_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool func_info_arg_push(func_info *const func, const size_t arg)
{
    log_verify(func != nullptr, false);

    return vector_push_back($args, &arg);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool func_info_is_equal(const func_info *const func, const token *const tkn)
{
    log_assert(func != nullptr);
    log_assert(tkn  != nullptr);

    log_assert(tkn->type == TOKEN_NAME);
    log_verify($tkn_name !=     nullptr, false);

    if (tkn->size == $size && strncmp($name, $tkn_name, $size) == 0) return true;
    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $name
#undef $size
#undef $args

//--------------------------------------------------------------------------------------------------------------------------------
// PROG_INFO
//--------------------------------------------------------------------------------------------------------------------------------

#define $v_store    (prog->var_storage)
#define $f_store    (prog->func_storage)
#define $scope      (prog->scope)
#define $is_main    (prog->is_main_func)
#define $is_ret     (prog->is_return_op)

//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_ctor(prog_info *const prog)
{
    log_verify(prog != nullptr, false);

    $v_store = vector_new(sizeof(var_info ));
    $f_store = vector_new(sizeof(func_info));

    if ($v_store == nullptr) { log_free($f_store); return false; }
    if ($f_store == nullptr) { log_free($v_store); return false; }

    $scope   = 0;
    $is_main = false;
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

static inline void prog_info_meet_return(prog_info *const prog)
{
    log_verify(prog != nullptr, (void) 0);

    if ($scope == 1) $is_ret = true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void prog_info_var_push_forced(prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, (void) 0);
    log_verify(tkn  != nullptr, (void) 0);

    log_verify(tkn->type == TOKEN_NAME, (void) 0);

    var_info  *var_new = var_info_new($tkn_name, tkn->size, $scope);
    log_verify(var_new != nullptr, (void) 0);

    vector_push_back($v_store, var_new);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void prog_info_func_push_forced(prog_info *const prog, const token *const tkn)
{
    log_verify(prog != nullptr, (void) 0);
    log_verify(tkn  != nullptr, (void) 0);

    log_verify(tkn->type == TOKEN_NAME, (void) 0);

    func_info *func_new = func_info_new($tkn_name, tkn->size);
    log_verify(func_new != nullptr, (void) 0);

    vector_push_back($f_store, func_new);
}
