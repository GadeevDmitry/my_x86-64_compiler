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
    LOG_VERIFY(node != nullptr, false);

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
    LOG_FREE(_node);
}

//--------------------------------------------------------------------------------------------------------------------------------
// { set
//--------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------
// pref
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_operand_pref(binary_node *const node)
{
    LOG_VERIFY(node != nullptr, false);

    $is_operand_pref = true;
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_address_pref(binary_node *const node)
{
    LOG_VERIFY(node != nullptr, false);

    $is_address_pref = true;
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// REX
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_REX(binary_node *const node, const BYTE REX)
{
    LOG_VERIFY(node != nullptr, false);

    $is_REX = true;
    $REX    =  REX;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_REX_W(binary_node *const node)
{
    LOG_VERIFY(node != nullptr, false);

    $is_REX = true;             //      _______________
    $REX    = $REX | (1 << 3);  // REX |0|1|0|0|W|r|x|b|
                                //      ^ ^ ^ ^ ^ ^ ^ ^
    return true;                //      7 6 5 4 3 0 1 2
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_REX_R(binary_node *const node)
{
    LOG_VERIFY(node != nullptr, false);

    $is_REX = true;             //      _______________
    $REX    = $REX | (1 << 2);  // REX |0|1|0|0|w|R|x|b|
                                //      ^ ^ ^ ^ ^ ^ ^ ^
    return true;                //      7 6 5 4 3 2 1 0
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_REX_X(binary_node *const node)
{
    LOG_VERIFY(node != nullptr, false);

    $is_REX = true;             //      _______________
    $REX    = $REX | (1 << 1);  // REX |0|1|0|0|w|r|X|b|
                                //      ^ ^ ^ ^ ^ ^ ^ ^
    return true;                //      7 6 5 4 3 2 1 0
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_REX_B(binary_node *const node)
{
    LOG_VERIFY(node != nullptr, false);

    $is_REX = true;             //      _______________
    $REX    = $REX | (1 << 0);  // REX |0|1|0|0|w|r|x|B|
                                //      ^ ^ ^ ^ ^ ^ ^ ^
    return true;                //      7 5 5 4 3 2 1 0
}

//--------------------------------------------------------------------------------------------------------------------------------
// opcode
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_opcode(binary_node *const node, const BYTE opcode)
{
    LOG_VERIFY(node != nullptr, false);

    $opcode = opcode;
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// ModRM
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_ModRM(binary_node *const node, BYTE mod,
                                                    BYTE reg,
                                                    BYTE  rm)
{
    LOG_VERIFY(node != nullptr, false);

    $is_ModRM = true;

    mod = mod % 4;
    reg = reg % 8;
    rm  = rm  % 8;

    $ModRM = (BYTE) ($ModRM | (mod << 6));  //  ___________
    $ModRM = (BYTE) ($ModRM | (reg << 3));  // |mod|reg|r/m|
    $ModRM = (BYTE) ($ModRM | (rm  << 0));  // ^   ^   ^   ^
                                            // 8   6   3   0
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_ModRM(binary_node *const node, const BYTE mod,
                                                    const GPR  reg,
                                                    const GPR   rm)
{
    LOG_VERIFY(node != nullptr, false);

    if (reg >= R8) binary_node_set_REX_R(node);
    if (rm  >= R8) binary_node_set_REX_B(node);

    return binary_node_set_ModRM(node, mod, (BYTE) reg, (BYTE) rm);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_ModRM(binary_node *const node, const BYTE mod,
                                                    const BYTE reg,
                                                    const GPR   rm)
{
    LOG_VERIFY(node != nullptr, false);

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
    LOG_VERIFY(node != nullptr, false);

    $is_SIB = true;

    scale = scale % 4;
    index = index % 8;
    base  = base  % 8;

    $SIB = (BYTE) ($SIB | (scale << 6));  //  ____________
    $SIB = (BYTE) ($SIB | (index << 3));  // |scl|ind|base|
    $SIB = (BYTE) ($SIB | (base  << 0));  // ^   ^   ^    ^
                                          // 8   6   3    0
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_SIB(binary_node *const node, const BYTE scale,
                                                  const GPR  index,
                                                  const GPR  base)
{
    LOG_VERIFY(node != nullptr, false);

    if (index >= R8) binary_node_set_REX_X(node);
    if (base  >= R8) binary_node_set_REX_B(node);

    return binary_node_set_SIB(node, scale, (BYTE) index, (BYTE) base);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_SIB(binary_node *const node, const BYTE scale,
                                                  const GPR  index,
                                                  const BYTE base)
{
    LOG_VERIFY(node != nullptr, false);

    if (index >= R8) binary_node_set_REX_X(node);

    return binary_node_set_SIB(node, scale, (BYTE) index, (BYTE) base);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_SIB(binary_node *const node, const BYTE scale,
                                                  const BYTE index,
                                                  const GPR  base)
{
    LOG_VERIFY(node != nullptr, false);

    if (base >= R8) binary_node_set_REX_B(node);

    return binary_node_set_SIB(node, scale, (BYTE) index, (BYTE) base);
}

//--------------------------------------------------------------------------------------------------------------------------------
// disp32
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_disp32(binary_node *const node, const DWORD disp32)
{
    LOG_VERIFY(node != nullptr, false);

    $is_disp32 = true;
    $disp32 = disp32;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// imm32
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_imm32(binary_node *const node, const DWORD imm32)
{
    LOG_VERIFY(node != nullptr, false);

    $is_imm32 = true;
    $imm32 = imm32;

    return true;
}

// } set
//--------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------
// get
//--------------------------------------------------------------------------------------------------------------------------------

size_t binary_node_get_size(const binary_node *const node)
{
    LOG_VERIFY(node != nullptr, -1UL);

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
    LOG_VERIFY(node != nullptr, 0);
    LOG_VERIFY($is_disp32 == true, 0);

    return $disp32;
}

//--------------------------------------------------------------------------------------------------------------------------------

DWORD binary_node_get_imm32(const binary_node *const node)
{
    LOG_VERIFY(node != nullptr, 0);
    LOG_VERIFY($is_imm32 == true, 0);

    return $imm32;
}

//--------------------------------------------------------------------------------------------------------------------------------
// store
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_store(const binary_node *const node, buffer *const buff)
{
    LOG_VERIFY   (node != nullptr, false);
    BUFFER_VERIFY(buff, false);

    if ($is_operand_pref) buffer_write(buff, "\x66"  , 1);
    if ($is_address_pref) buffer_write(buff, "\x67"  , 1);

    if ($is_REX)          buffer_write(buff, &$REX   , 1);
    /* opcode */          buffer_write(buff, &$opcode, 1);
    if ($is_ModRM)        buffer_write(buff, &$ModRM , 1);
    if ($is_SIB)          buffer_write(buff, &$SIB   , 1);

    if ($is_disp32)       buffer_write(buff, &$disp32, 4);
    if ($is_imm32)        buffer_write(buff, &$imm32 , 4);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void binary_node_dump(const void *const _node)
{
    LOG_VERIFY(_node != nullptr, (void) 0);

    const binary_node *const node = (const binary_node *) _node;

    binary_node_header_dump(node);
    binary_node_fields_dump(node);
    binary_node_ending_dump();
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void binary_node_header_dump(const binary_node *const node)
{
    LOG_ASSERT(node != nullptr);

    LOG_TAB_SERVICE_MESSAGE("binary_node (address: %p)\n"
                            "{", "\n",           node);
    LOG_TAB++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void binary_node_ending_dump()
{
    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("}", "\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_node_fields_dump(const binary_node *const node)
{
    LOG_ASSERT(node != nullptr);

    USUAL_FIELD_DUMP("opcode", "%x", $opcode);

    if ($is_REX   ) binary_node_REX_dump  (node);
    if ($is_ModRM ) binary_node_ModRM_dump(node);
    if ($is_SIB   ) binary_node_SIB_dump  (node);
    if ($is_disp32) USUAL_FIELD_DUMP("disp32", "%d", $disp32);
    if ($is_imm32 ) USUAL_FIELD_DUMP("imm32 ", "%d", $imm32 );
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_node_REX_dump(const binary_node *const node)
{
    LOG_ASSERT(node != nullptr);

    LOG_TAB_SERVICE_MESSAGE("REX\n"
                            "{", "\n");
    LOG_TAB++;

    LOG_TAB_SERVICE_MESSAGE(" _______________ "  "\n"
                            "|0|1|0|0|W|R|X|B|", "\n");

    bool is_higher_half_correct = (($REX >> 4) == 4);
    if  (is_higher_half_correct) LOG_TAB_MESSAGE(HTML_COLOR_LIME_GREEN);
    else                         LOG_TAB_MESSAGE(HTML_COLOR_DARK_RED  );

    for (size_t bit = 7; bit > 3; --bit) LOG_MESSAGE("|%d", ($REX >> bit) & 1);
    LOG_MESSAGE(HTML_COLOR_CANCEL);

    for (size_t bit = 3; bit < 4; --bit) LOG_MESSAGE("|%d", ($REX >> bit) & 1);
    LOG_MESSAGE("|\n");

    LOG_TAB_SERVICE_MESSAGE(" ^ ^ ^ ^ ^ ^ ^ ^ "  "\n"
                            " 7 6 5 4 3 2 1 0 ", "\n");
    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("}", "\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_node_ModRM_dump(const binary_node *const node)
{
    LOG_ASSERT(node != nullptr);

    LOG_TAB_SERVICE_MESSAGE("ModRM\n"
                            "{", "\n");
    LOG_TAB++;

    LOG_TAB_SERVICE_MESSAGE(" ___________ " "\n"
                            "|mod|reg|r/m|" "\n", "");

    LOG_MESSAGE("| %d%d|", ($ModRM >> 7) & 1, ($ModRM >> 6) & 1);                       // mod
    LOG_MESSAGE("%d%d%d|", ($ModRM >> 5) & 1, ($ModRM >> 4) & 1, ($ModRM >> 3) & 1);    // reg
    LOG_MESSAGE("%d%d%d|", ($ModRM >> 2) & 1, ($ModRM >> 1) & 1, ($ModRM >> 0) & 1);    // r/m

    LOG_SERVICE_MESSAGE("\n"
                        "^   ^   ^   ^"  "\n"
                        "8   6   3   0", "\n");
    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("}", "\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static void binary_node_SIB_dump(const binary_node *const node)
{
    LOG_ASSERT(node != nullptr);

    LOG_TAB_SERVICE_MESSAGE("SIB\n"
                            "{", "\n");
    LOG_TAB++;

    LOG_TAB_SERVICE_MESSAGE(" ___________ " "\n"
                            "|scl|ind|bas|" "\n", "");

    LOG_MESSAGE("| %d%d|", ($SIB >> 7) & 1, ($SIB >> 6) & 1);                   // scale
    LOG_MESSAGE("%d%d%d|", ($SIB >> 5) & 1, ($SIB >> 4) & 1, ($SIB >> 3) & 1);  // index
    LOG_MESSAGE("%d%d%d|", ($SIB >> 2) & 1, ($SIB >> 1) & 1, ($SIB >> 0) & 1);  // base

    LOG_SERVICE_MESSAGE("\n"
                        "^   ^   ^   ^"  "\n"
                        "8   6   3   0", "\n");
    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("}", "\n");
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
