#include "bin_static.h"

//================================================================================================================================
// BINARY
//================================================================================================================================

#define $is_operand_pref    (node->is_operand_override_prefix)
#define $is_address_pref    (node->is_address_override_prefix)

#define $is_REX             (node->is_REX)
#define $is_ModRM           (node->is_ModRM)
#define $is_SIB             (node->is_SIB)

#define $is_disp32          (node->is_disp32)
#define $is_imm32           (node->is_imm32)

//--------------------------------------------------------------------------------------------------------------------------------

#define $REX                (node->REX)
#define $opcode             (node->opcode)
#define $ModRM              (node->ModRM)
#define $SIB                (node->SIB)

#define $disp32             (node->disp32)
#define $imm32              (node->imm32)

//--------------------------------------------------------------------------------------------------------------------------------
// service
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_reset(binary_node *const node)
{
    log_verify(node != nullptr, false);

    $is_operand_pref = false;
    $is_address_pref = false;
    $is_REX          = false;
    $is_ModRM        = false;
    $is_SIB          = false;
    $is_disp32       = false;
    $is_imm32        = false;

    $REX             =  0x40;
    $opcode          =     0;
    $ModRM           =     0;
    $SIB             =     0;
    $disp32          =     0;
    $imm32           =     0;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void binary_node_delete(void *const _node)
{
    log_free(_node);
}

//--------------------------------------------------------------------------------------------------------------------------------
// { set
//--------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------
// ModRM
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_ModRM(binary_node *const node, BYTE mod,
                                                    BYTE reg,
                                                    BYTE  rm)
{
    log_verify(node != nullptr, false);

    $is_ModRM = true;

    mod = mod % 4;
    reg = reg % 8;
    rm  = rm  % 8;

    $ModRM = (BYTE) ($ModRM | (mod << 6));
    $ModRM = (BYTE) ($ModRM | (reg << 3));
    $ModRM = (BYTE) ($ModRM | (rm  << 0));

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_ModRM(binary_node *const node, const BYTE mod,
                                                    const GPR  reg,
                                                    const GPR   rm)
{
    log_verify(node != nullptr, false);

    if (reg >= R8) binary_node_set_REX_R(node);
    if (rm  >= R8) binary_node_set_REX_B(node);

    return binary_node_set_ModRM(node, mod, (BYTE) reg, (BYTE) rm);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_ModRM(binary_node *const node, const BYTE mod,
                                                    const BYTE reg,
                                                    const GPR   rm)
{
    log_verify(node != nullptr, false);

    if (rm >= R8) binary_node_set_REX_B(node);

    return binary_node_set_ModRM(node, mod, (BYTE) reg, (BYTE) rm);
}

//--------------------------------------------------------------------------------------------------------------------------------
// SIB
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_SIB(binary_node *const node, BYTE scale,
                                                  BYTE index,
                                                  BYTE base)
{
    log_verify(node != nullptr, false);

    $is_SIB = true;

    scale = scale % 4;
    index = index % 8;
    base  = base  % 8;

    $SIB = (BYTE) ($SIB | (scale << 6));
    $SIB = (BYTE) ($SIB | (index << 3));
    $SIB = (BYTE) ($SIB | (base  << 0));

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_SIB(binary_node *const node, const BYTE scale,
                                                  const GPR  index,
                                                  const GPR  base)
{
    log_verify(node != nullptr, false);

    if (index >= R8) binary_node_set_REX_X(node);
    if (base  >= R8) binary_node_set_REX_B(node);

    return binary_node_set_SIB(node, scale, (BYTE) index, (BYTE) base);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_SIB(binary_node *const node, const BYTE scale,
                                                  const GPR  index,
                                                  const BYTE base)
{
    log_verify(node != nullptr, false);

    if (index >= R8) binary_node_set_REX_X(node);

    return binary_node_set_SIB(node, scale, (BYTE) index, (BYTE) base);
}

// } set
//--------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------
// get
//--------------------------------------------------------------------------------------------------------------------------------

size_t binary_node_get_size(const binary_node *const node)
{
    log_verify(node != nullptr, -1UL);

    size_t size = 1; // opcode

    if ($is_operand_pref) size++;
    if ($is_address_pref) size++;

    if ($is_REX)          size++;
    if ($is_ModRM)        size++;
    if ($is_SIB)          size++;

    if ($is_disp32)       size += 4;
    if ($is_imm32)        size += 4;

    return size;
}

//--------------------------------------------------------------------------------------------------------------------------------

DWORD binary_node_get_disp32(const binary_node *const node)
{
    log_verify(node != nullptr, 0);
    log_verify($is_disp32 == true, 0);

    return $disp32;
}

//--------------------------------------------------------------------------------------------------------------------------------

DWORD binary_node_get_imm32(const binary_node *const node)
{
    log_verify(node != nullptr, 0);
    log_verify($is_imm32 == true, 0);

    return $imm32;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void binary_node_dump(const void *const _node)
{
    log_verify(_node != nullptr, (void) 0);

    const binary_node *const node = (const binary_node *) _node;

    binary_node_header_dump(node);
    binary_node_fields_dump(node);
    binary_node_ending_dump();
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void binary_node_header_dump(const binary_node *const node)
{
    log_assert(node != nullptr);

    log_tab_service_message("binary_node (address: %p)\n"
                            "{", "\n",           node);
    LOG_TAB++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void binary_node_ending_dump()
{
    LOG_TAB--;
    log_tab_service_message("}", "\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_node_fields_dump(const binary_node *const node)
{
    log_assert(node != nullptr);

    usual_field_dump("opcode", "%x", $opcode);

    if ($is_REX   ) binary_node_REX_dump  (node);
    if ($is_ModRM ) binary_node_ModRM_dump(node);
    if ($is_SIB   ) binary_node_SIB_dump  (node);
    if ($is_disp32) usual_field_dump("disp32", "%d", $disp32);
    if ($is_imm32 ) usual_field_dump("imm32 ", "%d", $imm32 );
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_node_REX_dump(const binary_node *const node)
{
    log_assert(node != nullptr);

    log_tab_service_message("REX\n"
                            "{", "\n");
    LOG_TAB++;

    log_tab_service_message(" _______________ "  "\n"
                            "|0|1|0|0|W|R|X|B|", "\n");

    bool is_higher_half_correct = (($REX >> 4) == 4);
    if  (is_higher_half_correct) log_tab_message(HTML_COLOR_LIME_GREEN);
    else                         log_tab_message(HTML_COLOR_DARK_RED  );

    for (size_t bit = 7; bit > 3; --bit) log_message("|%d", ($REX >> bit) & 1);
    log_message(HTML_COLOR_CANCEL);

    for (size_t bit = 3; bit < 4; --bit) log_message("|%d", ($REX >> bit) & 1);
    log_message("|\n");

    log_tab_service_message(" ^ ^ ^ ^ ^ ^ ^ ^ "  "\n"
                            " 7 6 5 4 3 2 1 0 ", "\n");
    LOG_TAB--;
    log_tab_service_message("}", "\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_node_ModRM_dump(const binary_node *const node)
{
    log_assert(node != nullptr);

    log_tab_service_message("ModRM\n"
                            "{", "\n");
    LOG_TAB++;

    log_tab_service_message(" ___________ " "\n"
                            "|mod|reg|r/m|" "\n", "");

    log_message("| %d%d|", ($ModRM >> 7) & 1, ($ModRM >> 6) & 1);                       // mod
    log_message("%d%d%d|", ($ModRM >> 5) & 1, ($ModRM >> 4) & 1, ($ModRM >> 3) & 1);    // reg
    log_message("%d%d%d|", ($ModRM >> 2) & 1, ($ModRM >> 1) & 1, ($ModRM >> 0) & 1);    // r/m

    log_service_message("\n"
                        "^   ^   ^   ^"  "\n"
                        "8   6   3   0", "\n");
    LOG_TAB--;
    log_tab_service_message("}", "\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_node_SIB_dump(const binary_node *const node)
{
    log_assert(node != nullptr);

    log_tab_service_message("SIB\n"
                            "{", "\n");
    LOG_TAB++;

    log_tab_service_message(" ___________ " "\n"
                            "|scl|ind|bas|" "\n", "");

    log_message("| %d%d|", ($SIB >> 7) & 1, ($SIB >> 6) & 1);                   // scale
    log_message("%d%d%d|", ($SIB >> 5) & 1, ($SIB >> 4) & 1, ($SIB >> 3) & 1);  // index
    log_message("%d%d%d|", ($SIB >> 2) & 1, ($SIB >> 1) & 1, ($SIB >> 0) & 1);  // base

    log_service_message("\n"
                        "^   ^   ^   ^"  "\n"
                        "8   6   3   0", "\n");
    LOG_TAB--;
    log_tab_service_message("}", "\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $is_operand_pref
#undef $is_address_pref

#undef $is_REX
#undef $is_ModRM
#undef $is_SIB

#undef $is_disp32
#undef $is_imm32

//--------------------------------------------------------------------------------------------------------------------------------

#undef $REX
#undef $opcode
#undef $ModRM
#undef $SIB

#undef $disp32
#undef $imm32
