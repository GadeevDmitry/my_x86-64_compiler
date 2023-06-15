#include "x64_static.h"

//================================================================================================================================
// X64_operand
//================================================================================================================================

#define $is_reg (operand->is_reg)
#define $is_mem (operand->is_mem)
#define $is_imm (operand->is_imm)

#define $reg    (operand->reg)
#define $imm    (operand->imm)

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

void x64_operand_dump(const void *const _operand)
{
    log_verify(_operand != nullptr, (void) 0);

    const x64_operand *const operand = (const x64_operand *) _operand;

    x64_operand_header_dump(operand);
    x64_operand_fields_dump(operand);
    x64_operand_ending_dump();
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void x64_operand_header_dump(const x64_operand *const operand)
{
    log_assert(operand != nullptr);

    log_tab_service_message("x64_operand (addr: %p)\n"
                            "{", "\n",     operand);
    LOG_TAB++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void x64_operand_fields_dump(const x64_operand *const operand)
{
    log_assert(operand != nullptr);

    log_tab_service_message("{", "\n");
    LOG_TAB++;

    usual_field_dump("is_reg", "%d", $is_reg);
    usual_field_dump("is_mem", "%d", $is_mem);
    usual_field_dump("is_imm", "%d", $is_imm);

    LOG_TAB--;
    log_tab_service_message("}", "\n\n");

    if ($is_reg) log_tab_message("reg = %s\n", GPR_names[$reg]);
    if ($is_imm) log_tab_message("imm = %d\n", $imm);
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void x64_operand_ending_dump()
{
    LOG_TAB--;
    log_tab_service_message("}", "\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $is_reg
#undef $is_mem
#undef $is_imm

#undef $reg
#undef $imm

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
                       const X64_cc    cc)
{
    x64_node  *node_new = (x64_node *) log_calloc(1, sizeof(x64_node));
    log_verify(node_new != nullptr, nullptr);

    if (!x64_node_ctor(node_new, type, cc)) { log_free(node_new); return nullptr; }
    return node_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool x64_node_ctor(x64_node *const node, const X64_CMD type,
                                         const X64_cc    cc)
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

static void x64_node_fields_dump(const x64_node *const node)
{
    log_assert(node != nullptr);

    usual_field_dump("type", "%s", X64_CMD_names[$type]);

    if ($type == X64_CMD_Jcc ||
        $type == X64_CMD_SETcc)
    {
        switch ($cc)
        {
            case X64_cc_G : usual_field_dump("cc  ", "%s", X64_cc_names[0]);
            case X64_cc_L : usual_field_dump("cc  ", "%s", X64_cc_names[1]);
            case X64_cc_E : usual_field_dump("cc  ", "%s", X64_cc_names[2]);

            case X64_cc_GE: usual_field_dump("cc  ", "%s", X64_cc_names[3]);
            case X64_cc_LE: usual_field_dump("cc  ", "%s", X64_cc_names[4]);
            case X64_cc_NE: usual_field_dump("cc  ", "%s", X64_cc_names[5]);

            default       : log_assert_verbose(false, "undefined X64_cc value");
        }
    }

    if ($type != X64_CMD_RET) x64_operand_dump(&$op_1);

    switch ($type)
    {
        case X64_CMD_ADD :
        case X64_CMD_SUB :

        case X64_CMD_OR  :
        case X64_CMD_AND :
        case X64_CMD_XOR :

        case X64_CMD_CMP :
        case X64_CMD_TEST: x64_operand_dump(&$op_2);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void x64_node_ending_dump()
{
    LOG_TAB--;
    log_tab_service_message("}", "\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $type
#undef $cc

#undef $op_1
#undef $op_2
