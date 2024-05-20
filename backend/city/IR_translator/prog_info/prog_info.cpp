#include "prog_info_static.h"

//================================================================================================================================
// PROG_INFO
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

prog_info *prog_info_new(const size_t var_quantity, const size_t func_quantity)
{
    prog_info *prog_new = (prog_info *) LOG_CALLOC(1, sizeof(prog_info));
    if (prog_new == nullptr)
    {
        LOG_ERROR("LOG_CALLOC(1, sizeof(prog_info) = %lu) returns nullptr\n", sizeof(prog_info));
        return nullptr;
    }

    if (!prog_info_ctor(prog_new, var_quantity, func_quantity)) { LOG_FREE(prog_new); return nullptr; }
    return prog_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_ctor(prog_info *const prog, const size_t  var_quantity,
                                           const size_t func_quantity)
{
    LOG_VERIFY(prog != nullptr, false);

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
    LOG_ASSERT(loc_addr != nullptr);

    stack *const loc_addr_begin = (stack *) array_begin(loc_addr);
    stack *const loc_addr_end   = (stack *) array_end  (loc_addr);

    for (stack *loc_cur = loc_addr_begin; loc_cur != loc_addr_end; ++loc_cur)
    {
        stack_ctor(loc_cur, sizeof(size_t));
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

vector *prog_info_dtor_no_IR(void *const _prog)
{
    if (_prog == nullptr) return nullptr;

    prog_info *const prog = (prog_info *) _prog;

    array_delete($func);
    array_delete($glob);
    array_delete($loc);

    stack_delete($scope);

    return $IR;
}

//--------------------------------------------------------------------------------------------------------------------------------

void prog_info_dtor(void *const _prog)
{
    if (_prog == nullptr) return;

    prog_info_dtor_no_IR(_prog);
    prog_info *const prog = (prog_info *) _prog;

    vector_delete($IR);
}

//--------------------------------------------------------------------------------------------------------------------------------

vector *prog_info_delete_no_IR(void *const _prog)
{
    vector *IR = prog_info_dtor_no_IR(_prog);
    LOG_FREE(_prog);

    return IR;
}

//--------------------------------------------------------------------------------------------------------------------------------

void prog_info_delete(void *const _prog)
{
    prog_info_dtor(_prog);
    LOG_FREE(_prog);
}

//--------------------------------------------------------------------------------------------------------------------------------
// var declaration
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_set_global_var_addr(prog_info *const prog, const size_t var_ind)
{
    LOG_VERIFY(prog   != nullptr    , false);
    LOG_VERIFY(var_ind < $glob->size, false);

    size_t *var_cell = ((size_t *) array_begin($glob)) + var_ind;
    LOG_VERIFY_VERBOSE(*var_cell == -1UL, "global variable is already declared", false);

    *var_cell = $rel;
    $rel++;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_add_local_var_addr(prog_info *const prog, const size_t var_ind)
{
    LOG_VERIFY(prog != nullptr     , false);
    LOG_VERIFY(var_ind < $loc->size, false);
    LOG_VERIFY_VERBOSE(!prog_info_is_local_var_redeclared(prog, var_ind),
                       "local variable is already declared in this scope", false);

    stack *var_cell = ((stack *) array_begin($loc)) + var_ind;
    stack_push(var_cell, &$rel);

    $rel++;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_is_local_var_redeclared(prog_info *const prog, const size_t var_ind)
{
    LOG_ASSERT(prog != nullptr);
    LOG_ASSERT(var_ind < $loc->size);
    LOG_VERIFY(!stack_is_empty($scope), false);

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
    LOG_VERIFY(prog     != nullptr  , false);
    LOG_VERIFY(var_ind < $glob->size, false);
    LOG_VERIFY(var_addr != nullptr  , false);

    *var_addr = ((size_t *) array_begin($glob))[var_ind];
    return *var_addr != -1UL;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_get_local_var_addr(prog_info *const prog, const size_t var_ind, size_t *const var_addr)
{
    LOG_VERIFY(prog     != nullptr , false);
    LOG_VERIFY(var_ind < $loc->size, false);
    LOG_VERIFY(var_addr != nullptr , false);

    stack *var_cell = ((stack *) array_begin($loc)) + var_ind;

    if (stack_is_empty(var_cell)) return false;
    return stack_front(var_cell, var_addr);
}

//--------------------------------------------------------------------------------------------------------------------------------
// func declaration
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_func_begin(prog_info *const prog, const size_t func_ind)
{
    LOG_VERIFY(prog != nullptr       , false);
    LOG_VERIFY(func_ind < $func->size, false);

    size_t *func_cell = ((size_t *) array_begin($func)) + func_ind;
    LOG_VERIFY_VERBOSE(*func_cell == -1UL, "function is already declared", false);

    *func_cell = $IR->size;

    stack_push($scope, &$rel);  // сохранение номера свободной ячейки для следующей глобальной переменной перед обработкой функции
    $rel = 0;
    stack_push($scope, &$rel);  // сохранение адреса начала области видимости (относительно фрейма функции)

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_func_end(prog_info *const prog)
{
    LOG_VERIFY(prog != nullptr, false);

    LOG_VERIFY(!stack_is_empty($scope), false);
    stack_pop($scope);          // удаление из стека адреса начала области видимости
    LOG_VERIFY(!stack_is_empty($scope), false);
    stack_pop($scope, &$rel);   // удаление номера свободной ячейки для следующей глобальной переменной (see "prog_info_func_begin")

    stack *const loc_begin = (stack *) array_begin($loc);
    stack *const loc_end   = (stack *) array_end  ($loc);

    for (stack *var = loc_begin; var != loc_end; ++var)
    {
        if (stack_is_empty(var)) continue;

        stack_pop(var);
        LOG_ASSERT_VERBOSE(stack_is_empty(var), "local variable is redeclared somewhere in the function");
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// func access
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_get_func_addr(prog_info *const prog, const size_t func_ind, size_t *const func_addr)
{
    LOG_VERIFY(prog      != nullptr  , false);
    LOG_VERIFY(func_ind < $func->size, false);
    LOG_VERIFY(func_addr != nullptr  , false);

    *func_addr = ((size_t *) array_begin($func))[func_ind];
    return *func_addr != -1UL;
}

//--------------------------------------------------------------------------------------------------------------------------------
// scope
//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_scope_in(prog_info *const prog)
{
    LOG_VERIFY(prog != nullptr, false);

    return stack_push($scope, &$rel);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_scope_out(prog_info *const prog)
{
    LOG_VERIFY(prog != nullptr        , false);
    LOG_VERIFY(!stack_is_empty($scope), false);

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

size_t prog_info_create_command(prog_info *const prog, const IR_CMD type, const bool is_reg,
                                                                          const bool is_mem,
                                                                          const bool is_imm, ...)
{
    LOG_VERIFY(prog != nullptr, -1UL);

    va_list  ap;
    va_start(ap, is_imm);

    IR_node cmd = {};
    IR_node_ctor(&cmd, type, is_reg, is_mem, is_imm, ap);

    va_end(ap);

    size_t result =  $IR->size;
    vector_push_back($IR, &cmd);

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

size_t prog_info_get_next_command_num(const prog_info *const prog)
{
    LOG_VERIFY(prog != nullptr, -1UL);

    return $IR->size;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool prog_info_fixup_jmp_addr(prog_info *const prog, const size_t jmp_cmd_num, const size_t label_num)
{
    LOG_VERIFY(prog != nullptr, false);
    LOG_VERIFY(jmp_cmd_num <= $IR->size, false);
    LOG_VERIFY(label_num   <= $IR->size, false);

    IR_node *jmp_cmd = ((IR_node *) vector_begin($IR)) + jmp_cmd_num;
    IR_node_set_imm_val(jmp_cmd, (int) label_num);

    return true;
}
