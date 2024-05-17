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
    x64_operand *operand_new = (x64_operand *) LOG_CALLOC(1, sizeof(x64_operand));
    LOG_VERIFY  (operand_new != nullptr, nullptr);

    va_list  ap;
    va_start(ap, is_imm);

    if (!x64_operand_ctor(operand_new, is_reg, is_mem, is_imm, ap)) { LOG_FREE(operand_new); return nullptr; }
    return operand_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool x64_operand_ctor(x64_operand *const operand, const bool is_reg,
                                                  const bool is_mem,
                                                  const bool is_imm, ...)
{
    LOG_VERIFY(operand != nullptr, false);

    va_list  ap;
    va_start(ap, is_imm);

    return x64_operand_ctor(operand, is_reg, is_mem, is_imm, ap);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool x64_operand_ctor(x64_operand *const operand, const bool is_reg,
                                                         const bool is_mem,
                                                         const bool is_imm, va_list ap)
{
    LOG_ASSERT(operand != nullptr);

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
    LOG_FREE(_operand);
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static void x64_operand_dump(const x64_operand *const operand)
{
    LOG_ASSERT(operand != nullptr);

    if      ($is_reg && !$is_mem && !$is_imm) x64_operand_dump_reg(operand);
    else if ($is_imm && !$is_mem && !$is_reg) x64_operand_dump_imm(operand);
    else                                      x64_operand_dump_mem(operand);
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void x64_operand_dump_reg(const x64_operand *const operand)
{
    LOG_ASSERT(operand != nullptr);
    LOG_ASSERT($is_reg && !$is_mem && !$is_imm);

    LOG_MESSAGE("%s", GPR_names[$reg]);
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void x64_operand_dump_imm(const x64_operand *const operand)
{
    LOG_ASSERT(operand != nullptr);
    LOG_ASSERT($is_imm && !$is_mem && !$is_reg);

    LOG_MESSAGE("%d", $imm);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void x64_operand_dump_mem(const x64_operand *const operand)
{
    LOG_ASSERT(operand != nullptr);
    LOG_ASSERT($is_mem);

    if      ($is_reg && !$is_imm) LOG_MESSAGE("[%d * %s]"       , $scl, GPR_names[$reg]);
    else if ($is_imm && !$is_reg) LOG_MESSAGE("[%d * %d]"       , $scl,                  $imm);
    else if ($is_imm &&  $is_reg) LOG_MESSAGE("[%d * (%s + %d)]", $scl, GPR_names[$reg], $imm);

    else                          LOG_ASSERT_VERBOSE(false, "impossible operand");
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
    x64_node  *node_new = (x64_node *) LOG_CALLOC(1, sizeof(x64_node));
    LOG_VERIFY(node_new != nullptr, nullptr);

    if (!x64_node_ctor(node_new, type, cc)) { LOG_FREE(node_new); return nullptr; }
    return node_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool x64_node_ctor(x64_node *const node, const X64_CMD type,
                                         const X64_cc    cc /* = X64_cc_no */)
{
    LOG_VERIFY(node != nullptr, false);

    $type = type;
    $cc   =   cc;

    $op_1 =   {};
    $op_2 =   {};

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void x64_node_delete(void *const _node)
{
    LOG_FREE(_node);
}

//--------------------------------------------------------------------------------------------------------------------------------
// set
//--------------------------------------------------------------------------------------------------------------------------------

bool x64_node_set_operand_1(x64_node *const node, const bool is_reg,
                                                  const bool is_mem,
                                                  const bool is_imm, ...)
{
    LOG_VERIFY(node != nullptr, false);

    va_list  ap;
    va_start(ap, is_imm);

    return x64_operand_ctor(&$op_1, is_reg, is_mem, is_imm, ap);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool x64_node_set_operand_2(x64_node *const node, const bool is_reg,
                                                  const bool is_mem,
                                                  const bool is_imm, ...)
{
    LOG_VERIFY(node != nullptr, false);

    va_list  ap;
    va_start(ap, is_imm);

    return x64_operand_ctor(&$op_2, is_reg, is_mem, is_imm, ap);
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void x64_node_dump(const void *const _node)
{
    LOG_VERIFY(_node != nullptr, (void) 0);

    const x64_node *const node = (const x64_node *) _node;

    x64_node_header_dump(node);
    x64_node_fields_dump(node);
    x64_node_ending_dump();
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void x64_node_header_dump(const x64_node *const node)
{
    LOG_ASSERT(node != nullptr);

    LOG_TAB_SERVICE_MESSAGE("x64_node (addr: %p)\n"
                            "{", "\n",     node);
    LOG_TAB++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void x64_node_ending_dump()
{
    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("}", "\n\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough="

static void x64_node_fields_dump(const x64_node *const node)
{
    LOG_ASSERT(node != nullptr);

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
        case X64_CMD_TEST: LOG_MESSAGE(", ");
                           x64_operand_dump(&$op_2);
        default          : LOG_MESSAGE("\n");
                           break;
    }
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------

static void x64_node_type_dump(const x64_node *const node)
{
    LOG_ASSERT(node != nullptr);

    if      ($type == X64_CMD_Jcc  ) x64_node_typecc_dump(node, X64_JCC_names);
    else if ($type == X64_CMD_SETcc) x64_node_typecc_dump(node, X64_SETCC_names);
    else                             LOG_TAB_MESSAGE("%s ", X64_CMD_names[$type]);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void x64_node_typecc_dump(const x64_node *const node, const char *names[])
{
    LOG_ASSERT(node != nullptr);
    LOG_ASSERT(($type == X64_CMD_Jcc) ||
               ($type == X64_CMD_SETcc));

    switch ($cc)
    {
        case X64_cc_G : LOG_TAB_MESSAGE("%s ", names[0]); break;
        case X64_cc_L : LOG_TAB_MESSAGE("%s ", names[1]); break;
        case X64_cc_E : LOG_TAB_MESSAGE("%s ", names[2]); break;
        case X64_cc_GE: LOG_TAB_MESSAGE("%s ", names[3]); break;
        case X64_cc_LE: LOG_TAB_MESSAGE("%s ", names[4]); break;
        case X64_cc_NE: LOG_TAB_MESSAGE("%s ", names[5]); break;

        case X64_cc_no:
        default       : LOG_ASSERT_VERBOSE(false, "undefined X64_cc value");
                        break;
    }
}

// } x64_node_pretty_fields_dump
//--------------------------------------------------------------------------------------------------------------------------------

#undef $type
#undef $cc

#undef $op_1
#undef $op_2
