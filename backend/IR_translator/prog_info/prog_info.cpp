#include "prog_info_static.h"

//================================================================================================================================
// PROG_INFO
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

prog_info *prog_info_new(const size_t var_quantity, const size_t func_quantity)
{
    prog_info *prog_new = (prog_info *) log_calloc(1, sizeof(prog_info));
    if (prog_new == nullptr)
    {
        log_error("log_calloc(1, sizeof(prog_info) = %lu) returns nullptr\n", sizeof(prog_info));
        return nullptr;
    }

    if (!prog_info_ctor(prog_new, var_quantity, func_quantity)) { log_free(prog_new); return nullptr; }
    return prog_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_ctor(prog_info *const prog, const size_t  var_quantity,
                                           const size_t func_quantity)
{
    log_verify(prog != nullptr, false);

    $IR = vector_new(sizeof(IR_node), nullptr, nullptr, IR_node_dump);

    size_t poison_val = -1UL;

    $func  = array_new(func_quantity, sizeof(size_t)); array_init($func, &poison_val);
    $glob  = array_new( var_quantity, sizeof(size_t)); array_init($glob, &poison_val);
    $loc   = array_new( var_quantity, sizeof(stack), stack_dtor, stack_dump); loc_addr_ctor($loc);

    $scope = stack_new(sizeof(size_t));
    $rel   = 0UL;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool loc_addr_ctor(array *const loc_addr)
{
    log_assert(loc_addr != nullptr);

    stack *const loc_addr_begin = (stack *) array_begin(loc_addr);
    stack *const loc_addr_end   = (stack *) array_end  (loc_addr);

    for (stack *loc_cur = loc_addr_begin; loc_cur != loc_addr_end; ++loc_cur)
    {
        stack_ctor(loc_cur, sizeof(size_t));
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

void prog_info_dtor(void *const _prog)
{
    if (_prog == nullptr) return;

    prog_info *const prog = (prog_info *) _prog;

    vector_free($IR);

    array_free($func);
    array_free($glob);
    array_free($loc);

    stack_free($scope);
}

//--------------------------------------------------------------------------------------------------------------------------------

void prog_info_delete(void *const _prog)
{
    prog_info_dtor(_prog);
    log_free(_prog);
}

//--------------------------------------------------------------------------------------------------------------------------------
// var declaration
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_set_global_var_addr(prog_info *const prog, const size_t var_ind)
{
    log_verify(prog   != nullptr    , false);
    log_verify(var_ind < $glob->size, false);

    size_t *var_cell = ((size_t *) array_begin($glob)) + var_ind;
    log_verify_verbose(*var_cell == -1UL, "global variable is already declared", false);

    *var_cell = $rel;
    $rel++;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_add_local_var_addr(prog_info *const prog, const size_t var_ind)
{
    log_verify(prog != nullptr     , false);
    log_verify(var_ind < $loc->size, false);
    log_verify_verbose(!prog_info_is_local_var_redeclared(prog, var_ind),
                       "local variable is already declared in this scope", false);

    stack *var_cell = ((stack *) array_begin($loc)) + var_ind;
    stack_push(var_cell, &$rel);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_is_local_var_redeclared(prog_info *const prog, const size_t var_ind)
{
    log_assert(prog != nullptr);
    log_assert(var_ind < $loc->size);
    log_verify(!stack_is_empty($scope), false);

    stack *var_cell = ((stack *) array_begin($loc)) + var_ind;

    if (stack_is_empty(var_cell)) return false;

    size_t top_addr = 0UL, scope_addr = 0UL;
    stack_front(var_cell, &  top_addr);
    stack_front($scope  , &scope_addr);

    return top_addr >= scope_addr;
}

//--------------------------------------------------------------------------------------------------------------------------------
// var access
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_get_global_var_addr(prog_info *const prog, const size_t var_ind, size_t *const var_addr)
{
    log_verify(prog     != nullptr  , false);
    log_verify(var_ind < $glob->size, false);
    log_verify(var_addr != nullptr  , false);

    *var_addr = ((size_t *) array_begin($glob))[var_ind];
    return *var_addr != -1UL;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_get_local_var_addr(prog_info *const prog, const size_t var_ind, size_t *const var_addr)
{
    log_verify(prog     != nullptr , false);
    log_verify(var_ind < $loc->size, false);
    log_verify(var_addr != nullptr , false);

    stack *var_cell = ((stack *) array_begin($loc)) + var_ind;

    if (stack_is_empty(var_cell)) return false;

    return stack_front(var_cell, var_addr);
}

//--------------------------------------------------------------------------------------------------------------------------------
// func declaration
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_func_begin(prog_info *const prog, const size_t func_ind)
{
    log_verify(prog != nullptr       , false);
    log_verify(func_ind < $func->size, false);

    size_t *func_cell = ((size_t *) array_begin($func)) + func_ind;
    log_verify_verbose(*func_cell == -1UL, "function is already declared", false);

    *func_cell = $IR->size;

    stack_push($scope, &$rel);  // сохранение номера свободной ячейки для следующей глобальной переменной перед обработкой функции
    $rel = 0;
    stack_push($scope, &$rel);  // сохранение адреса начала области видимости (относительно фрейма функции)

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_func_end(prog_info *const prog)
{
    log_verify(prog != nullptr, false);

    log_verify(!stack_is_empty($scope), false);
    stack_pop($scope);          // удаление из стека адреса начала области видимости
    log_verify(!stack_is_empty($scope), false);
    stack_pop($scope, &$rel);   // удаление номера свободной ячейки для следующей глобальной переменной (see "prog_info_func_begin")

    stack *const loc_begin = (stack *) array_begin($loc);
    stack *const loc_end   = (stack *) array_end  ($loc);

    for (stack *var = loc_begin; var != loc_end; ++var)
    {
        if (stack_is_empty(var)) continue;

        stack_pop(var);
        log_verify(stack_is_empty(var), false);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// func access
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_get_func_addr(prog_info *const prog, const size_t func_ind, size_t *const func_addr)
{
    log_verify(prog      != nullptr  , false);
    log_verify(func_ind < $func->size, false);
    log_verify(func_addr != nullptr  , false);

    *func_addr = ((size_t *) array_begin($func))[func_ind];
    return *func_addr != -1UL;
}

//--------------------------------------------------------------------------------------------------------------------------------
// scope
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_scope_in(prog_info *const prog)
{
    log_verify(prog != nullptr, false);

    stack_push($scope, &$rel);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_scope_out(prog_info *const prog)
{
    log_verify(prog != nullptr        , false);
    log_verify(!stack_is_empty($scope), false);

    stack_pop($scope, &$rel);

    stack *const loc_begin = (stack *) array_begin($loc);
    stack *const loc_end   = (stack *) array_end  ($loc);

    for (stack *var = loc_begin; var != loc_end; ++var)
    {
        if (stack_is_empty(var)) continue;

        size_t top_addr = 0UL;
        stack_front(var, &top_addr);

        if (top_addr >= $rel) stack_pop(var);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// other
//--------------------------------------------------------------------------------------------------------------------------------

size_t prog_info_create_command(prog_info *const prog, const IR_node *const cmd)
{
    log_verify(prog != nullptr, -1UL);

    size_t result =  $IR->size;
    vector_push_back($IR, cmd);

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

size_t prog_info_get_next_command_num(const prog_info *const prog)
{
    log_verify(prog != nullptr, -1UL);

    return $IR->size;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_fixup_jmp_addr(prog_info *const prog, const size_t jmp_cmd_num, const size_t label_num)
{
    log_verify(prog != nullptr, false);
    log_verify(jmp_cmd_num <= $IR->size, false);
    log_verify(label_num   <= $IR->size, false);

    IR_node *jmp_cmd = ((IR_node *) vector_begin($IR)) + jmp_cmd_num;
    IR_node_set_imm_val(jmp_cmd, label_num);

    return true;
}
