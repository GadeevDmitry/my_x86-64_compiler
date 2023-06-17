#include "x64_static.h"

//================================================================================================================================
// X64_operand
//================================================================================================================================

#define $is_reg (operand->is_reg)
#define $is_mem (operand->is_mem)
#define $is_imm (operand->is_imm)

#define $reg    (operand->reg)
#define $imm    (operand->imm)
#define $scl    (operand->scale_factor)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

x64_operand *x64_operand_new(const bool is_reg,
                             const bool is_mem,
                             const bool is_imm, ...)
{
    x64_operand *operand_new = (x64_operand *) log_calloc(1, sizeof(x64_operand));
    log_verify  (operand_new != nullptr, nullptr);

    va_list  ap;
    va_start(ap, is_imm);

    if (!x64_operand_ctor(operand_new, is_reg, is_mem, is_imm, ap)) { log_free(operand_new); return nullptr; }
    return operand_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool x64_operand_ctor(x64_operand *const operand, const bool is_reg,
                                                  const bool is_mem,
                                                  const bool is_imm, ...)
{
    log_verify(operand != nullptr, false);

    va_list  ap;
    va_start(ap, is_imm);

    return x64_operand_ctor(operand, is_reg, is_mem, is_imm, ap);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool x64_operand_ctor(x64_operand *const operand, const bool is_reg,
                                                         const bool is_mem,
                                                         const bool is_imm, va_list ap)
{
    log_assert(operand != nullptr);

    $is_reg = is_reg;
    $is_mem = is_mem;
    $is_imm = is_imm;

    $scl    =      1;

    if (is_reg) $reg = (GPR) va_arg(ap, int);
    if (is_imm) $imm =       va_arg(ap, int);

    va_end(ap);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void x64_operand_delete(void *const _operand)
{
    log_free(_operand);
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static void x64_operand_dump(const x64_operand *const operand)
{
    log_assert(operand != nullptr);

    if      ($is_reg && !$is_mem && !$is_imm) x64_operand_dump_reg(operand);
    else if ($is_imm && !$is_mem && !$is_reg) x64_operand_dump_imm(operand);
    else                                      x64_operand_dump_mem(operand);
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void x64_operand_dump_reg(const x64_operand *const operand)
{
    log_assert(operand != nullptr);
    log_assert($is_reg && !$is_mem && !$is_imm);

    log_message("%s", GPR_names[$reg]);
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void x64_operand_dump_imm(const x64_operand *const operand)
{
    log_assert(operand != nullptr);
    log_assert($is_imm && !$is_mem && !$is_reg);

    log_message("%d", $imm);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void x64_operand_dump_mem(const x64_operand *const operand)
{
    log_assert(operand != nullptr);
    log_assert($is_mem);

    if      ($is_reg && !$is_imm) log_message("[%d * %s]"       , $scl, GPR_names[$reg]);
    else if ($is_imm && !$is_reg) log_message("[%d * %d]"       , $scl,                  $imm);
    else if ($is_imm &&  $is_reg) log_message("[%d * (%s + %d)]", $scl, GPR_names[$reg], $imm);

    else                          log_assert_verbose(false, "impossible operand");
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $is_reg
#undef $is_mem
#undef $is_imm

#undef $reg
#undef $imm
#undef $scl

//================================================================================================================================
// X64_node
//================================================================================================================================

#define $type   (node->type)
#define $cc     (node->cc)

#define $op_1   (node->op_1)
#define $op_2   (node->op_2)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

x64_node *x64_node_new(const X64_CMD type,
                       const X64_cc    cc /* = X64_cc_no */)
{
    x64_node  *node_new = (x64_node *) log_calloc(1, sizeof(x64_node));
    log_verify(node_new != nullptr, nullptr);

    if (!x64_node_ctor(node_new, type, cc)) { log_free(node_new); return nullptr; }
    return node_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool x64_node_ctor(x64_node *const node, const X64_CMD type,
                                         const X64_cc    cc /* = X64_cc_no */)
{
    log_verify(node != nullptr, false);

    $type = type;
    $cc   =   cc;

    $op_1 =   {};
    $op_2 =   {};

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void x64_node_delete(void *const _node)
{
    log_free(_node);
}

//--------------------------------------------------------------------------------------------------------------------------------
// set
//--------------------------------------------------------------------------------------------------------------------------------

bool x64_node_set_operand_1(x64_node *const node, const bool is_reg,
                                                  const bool is_mem,
                                                  const bool is_imm, ...)
{
    log_verify(node != nullptr, false);

    va_list  ap;
    va_start(ap, is_imm);

    return x64_operand_ctor(&$op_1, is_reg, is_mem, is_imm, ap);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool x64_node_set_operand_2(x64_node *const node, const bool is_reg,
                                                  const bool is_mem,
                                                  const bool is_imm, ...)
{
    log_verify(node != nullptr, false);

    va_list  ap;
    va_start(ap, is_imm);

    return x64_operand_ctor(&$op_2, is_reg, is_mem, is_imm, ap);
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void x64_node_dump(const void *const _node)
{
    log_verify(_node != nullptr, (void) 0);

    const x64_node *const node = (const x64_node *) _node;

    x64_node_header_dump(node);
    x64_node_fields_dump(node);
    x64_node_ending_dump();
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void x64_node_header_dump(const x64_node *const node)
{
    log_assert(node != nullptr);

    log_tab_service_message("x64_node (addr: %p)\n"
                            "{", "\n",     node);
    LOG_TAB++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void x64_node_ending_dump()
{
    LOG_TAB--;
    log_tab_service_message("}", "\n\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static void x64_node_fields_dump(const x64_node *const node)
{
    log_assert(node != nullptr);

    x64_node_type_dump(node);

    if ($type == X64_CMD_RET) return;

    x64_operand_dump(&$op_1);

    switch ($type)
    {
        case X64_CMD_ADD :
        case X64_CMD_SUB :

        case X64_CMD_OR  :
        case X64_CMD_AND :
        case X64_CMD_XOR :

        case X64_CMD_CMP :
        case X64_CMD_TEST: log_message(", ");
                           x64_operand_dump(&$op_2);
                           log_message("\n");
                           break;
        default          : break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static void x64_node_type_dump(const x64_node *const node)
{
    log_assert(node != nullptr);

    if      ($type == X64_CMD_Jcc  ) x64_node_typecc_dump(node, X64_JCC_names);
    else if ($type == X64_CMD_SETcc) x64_node_typecc_dump(node, X64_SETCC_names);
    else                             log_tab_message("%s ", X64_CMD_names[$type]);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void x64_node_typecc_dump(const x64_node *const node, const char *names[])
{
    log_assert(node != nullptr);
    log_assert(($type == X64_CMD_Jcc) ||
               ($type == X64_CMD_SETcc));

    switch ($cc)
    {
        case X64_cc_G : log_tab_message("%s ", names[0]); break;
        case X64_cc_L : log_tab_message("%s ", names[1]); break;
        case X64_cc_E : log_tab_message("%s ", names[2]); break;
        case X64_cc_GE: log_tab_message("%s ", names[3]); break;
        case X64_cc_LE: log_tab_message("%s ", names[4]); break;
        case X64_cc_NE: log_tab_message("%s ", names[5]); break;

        default       : log_assert_verbose(false, "undefined X64_cc value");
                        break;
    }
}

// } x64_node_pretty_fields_dump
//--------------------------------------------------------------------------------------------------------------------------------

#undef $type
#undef $cc

#undef $op_1
#undef $op_2
