#include "IR_static.h"

//================================================================================================================================
// IR_NODE
//================================================================================================================================

#define $type       (node->type)

#define $is_reg     (node->is_arg.is_reg_arg)
#define $is_mem     (node->is_arg.is_mem_arg)
#define $is_imm     (node->is_arg.is_imm_arg)

#define $reg_num    (node->reg_num)
#define $imm_val    (node->imm_val)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

bool IR_node_ctor(IR_node *const node, const IR_CMD type, const bool is_reg,
                                                          const bool is_mem,
                                                          const bool is_imm, va_list ap)
{
    LOG_ASSERT(node != nullptr);

    $type = type;

    $is_reg = is_reg;
    $is_mem = is_mem;
    $is_imm = is_imm;

    if (is_reg) $reg_num = (unsigned char) va_arg(ap, int);
    if (is_imm) $imm_val = va_arg(ap, int);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool IR_node_ctor(IR_node *const node, const IR_CMD type, const bool is_reg,
                                                          const bool is_mem,
                                                          const bool is_imm, ...)
{
    LOG_VERIFY(node != nullptr, false);

    va_list  ap;
    va_start(ap, is_imm);

    bool result = IR_node_ctor(node, type, is_reg, is_mem, is_imm, ap);

    va_end(ap);
    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

IR_node *IR_node_new(const IR_CMD type, const bool is_reg,
                                        const bool is_mem,
                                        const bool is_imm, ...)
{
    IR_node   *node_new = (IR_node *) LOG_CALLOC(1, sizeof(IR_node));
    LOG_VERIFY(node_new != nullptr, nullptr);

    va_list  ap;
    va_start(ap, is_imm);

    if (IR_node_ctor(node_new, type, is_reg, is_mem, is_imm, ap)) { va_end(ap); return node_new; }

    LOG_FREE(node_new);
    va_end(ap);
    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------

void IR_node_delete(void *const _node)
{
    LOG_FREE(_node);
}

//--------------------------------------------------------------------------------------------------------------------------------
// set
//--------------------------------------------------------------------------------------------------------------------------------

bool IR_node_set_imm_val(IR_node *const node, const int imm_val)
{
    LOG_VERIFY(node != nullptr, false);

    $imm_val = imm_val;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool IR_node_set_reg_num(IR_node *const node, const int reg_num)
{
    LOG_VERIFY(node != nullptr, false);

    $reg_num = (unsigned char) reg_num;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void IR_node_dump(const void *const _node)
{
    LOG_VERIFY(_node != nullptr, (void) 0);

    const IR_node *const node = (const IR_node *) _node;

    IR_node_header_dump(node);
    IR_node_fields_dump(node);
    IR_node_ending_dump();
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void IR_node_header_dump(const IR_node *const node)
{
    LOG_ASSERT(node != nullptr);

    LOG_TAB_SERVICE_MESSAGE("IR_node (address: %p)\n"
                            "{", "\n",       node);
    LOG_TAB++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void IR_node_fields_dump(const IR_node *const node)
{
    LOG_ASSERT(node != nullptr);

    USUAL_FIELD_DUMP("type", "%s", IR_CMD_NAMES[$type]);

    LOG_TAB_SERVICE_MESSAGE("\n"
                            "is_arg\n"
                            "{",  "\n");
    LOG_TAB++;
    USUAL_FIELD_DUMP("is_reg_arg", "%d", $is_reg);
    USUAL_FIELD_DUMP("is_mem_arg", "%d", $is_mem);
    USUAL_FIELD_DUMP("is_imm_arg", "%d", $is_imm);
    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("}", "\n\n");

    if ($is_reg) USUAL_FIELD_DUMP("reg_num", "%u", $reg_num);
    if ($is_imm) USUAL_FIELD_DUMP("imm_val", "%u", $imm_val);
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void IR_node_ending_dump()
{
    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("}", "\n");
}
