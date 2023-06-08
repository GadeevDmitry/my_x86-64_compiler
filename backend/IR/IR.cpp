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

static bool IR_node_ctor(IR_node *const node, const IR_CMD type, const bool is_reg,
                                                                 const bool is_mem,
                                                                 const bool is_imm, va_list ap)
{
    log_assert(node != nullptr);

    $type = type;

    $is_reg = is_reg;
    $is_mem = is_mem;
    $is_imm = is_imm;

    if (is_reg) $reg_num = (unsigned char) va_arg(ap, int);
    if (is_imm) $imm_val = va_arg(ap, int);

    va_end(ap);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool IR_node_ctor(IR_node *const node, const IR_CMD type, const bool is_reg,
                                                          const bool is_mem,
                                                          const bool is_imm, ...)
{
    log_verify(node != nullptr, false);

    $type = type;

    $is_reg = is_reg;
    $is_mem = is_mem;
    $is_imm = is_imm;

    va_list  ap;
    va_start(ap, is_imm);

    if (is_reg) $reg_num = (unsigned char) va_arg(ap, int);
    if (is_imm) $imm_val = va_arg(ap, int);

    va_end(ap);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

IR_node *IR_node_new(const IR_CMD type, const bool is_reg,
                                        const bool is_mem,
                                        const bool is_imm, ...)
{
    IR_node   *node_new = (IR_node *) log_calloc(1, sizeof(IR_node));
    log_verify(node_new != nullptr, nullptr);

    va_list  ap;
    va_start(ap, is_imm);

    if (IR_node_ctor(node_new, type, is_reg, is_mem, is_imm, ap)) return node_new;

    log_free(node_new);
    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------

void IR_node_delete(void *const _node)
{
    log_free(_node);
}

//--------------------------------------------------------------------------------------------------------------------------------
// set
//--------------------------------------------------------------------------------------------------------------------------------

bool IR_node_set_imm_val(IR_node *const node, const int imm_val)
{
    log_verify(node != nullptr, false);

    $imm_val = imm_val;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool IR_node_set_reg_num(IR_node *const node, const int reg_num)
{
    log_verify(node != nullptr, false);

    $reg_num = (unsigned char) reg_num;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void IR_node_dump(const void *const _node)
{
    log_verify(_node != nullptr, (void) 0);

    const IR_node *const node = (const IR_node *) _node;

    IR_node_header_dump(node);
    IR_node_fields_dump(node);
    IR_node_ending_dump();
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void IR_node_header_dump(const IR_node *const node)
{
    log_assert(node != nullptr);

    log_tab_service_message("IR_node (address: %p)\n"
                            "{", "\n",       node);
    LOG_TAB++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void IR_node_fields_dump(const IR_node *const node)
{
    log_assert(node != nullptr);

    usual_field_dump("type", "%s", IR_CMD_NAMES[$type]);

    log_tab_service_message("\n"
                            "is_arg\n"
                            "{",  "\n");
    LOG_TAB++;
    usual_field_dump("is_reg_arg", "%d", $is_reg);
    usual_field_dump("is_mem_arg", "%d", $is_mem);
    usual_field_dump("is_imm_arg", "%d", $is_imm);
    LOG_TAB--;
    log_tab_service_message("}", "\n\n");

    if ($is_reg) usual_field_dump("reg_num", "%u", $reg_num);
    if ($is_imm) usual_field_dump("imm_val", "%u", $imm_val);
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void IR_node_ending_dump()
{
    LOG_TAB--;
    log_tab_service_message("}", "\n");
}
