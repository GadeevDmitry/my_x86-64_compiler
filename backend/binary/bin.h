#ifndef BIN_H
#define BIN_H

#include <stdlib.h>

#define LOG_NTRACE
#include "../../lib/logs/log.h"

#include "../x64/x64.h"

//================================================================================================================================
// { BINARY_NODE
//================================================================================================================================

typedef unsigned char BYTE;
typedef int          DWORD;

struct binary_node
{
    struct
    {
        bool is_operand_override_prefix;
        bool is_address_override_prefix;

        bool is_REX;
        bool is_ModRM;
        bool is_SIB;

        bool is_disp32;
        bool is_imm32;
    };

    BYTE  REX;
    BYTE  opcode;
    BYTE  ModRM;
    BYTE  SIB;

    DWORD disp32;
    DWORD  imm32;
};

//================================================================================================================================
// HEADERS
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// service
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_reset (binary_node *const  node);
void binary_node_delete(void        *const _node);

//--------------------------------------------------------------------------------------------------------------------------------
// set
//--------------------------------------------------------------------------------------------------------------------------------

static bool binary_node_set_operand_pref(binary_node *const node)                     __attribute__((always_inline));
static bool binary_node_set_address_pref(binary_node *const node)                     __attribute__((always_inline));

static bool binary_node_set_REX         (binary_node *const node, const BYTE REX)     __attribute__((always_inline));
static bool binary_node_set_REX_W       (binary_node *const node)                     __attribute__((always_inline));
static bool binary_node_set_REX_R       (binary_node *const node)                     __attribute__((always_inline));
static bool binary_node_set_REX_X       (binary_node *const node)                     __attribute__((always_inline));
static bool binary_node_set_REX_B       (binary_node *const node)                     __attribute__((always_inline));

static bool binary_node_set_opcode      (binary_node *const node, const BYTE opcode)  __attribute__((always_inline));

       bool binary_node_set_ModRM       (binary_node *const node,       BYTE mod,       BYTE reg,       BYTE rm);
       bool binary_node_set_ModRM       (binary_node *const node, const BYTE mod, const BYTE reg, const GPR  rm);
       bool binary_node_set_ModRM       (binary_node *const node, const BYTE mod, const GPR  reg, const GPR  rm);

       bool binary_node_set_SIB         (binary_node *const node,       BYTE scale,      BYTE index,       BYTE base);
       bool binary_node_set_SIB         (binary_node *const node, const BYTE scale, const GPR index, const GPR  base);
       bool binary_node_set_SIB         (binary_node *const node, const BYTE scale, const GPR index, const BYTE base);


static bool binary_node_set_disp32      (binary_node *const node, const DWORD disp32) __attribute__((always_inline));
static bool binary_node_set_imm32       (binary_node *const node, const DWORD  imm32) __attribute__((always_inline));

//--------------------------------------------------------------------------------------------------------------------------------
// get
//--------------------------------------------------------------------------------------------------------------------------------

size_t binary_node_get_size  (const binary_node *const node);
DWORD  binary_node_get_disp32(const binary_node *const node);
DWORD  binary_node_get_imm32 (const binary_node *const node);

//--------------------------------------------------------------------------------------------------------------------------------
// store
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_store(const binary_node *const node, buffer *const buff);

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void binary_node_dump(const void *const _node);

//================================================================================================================================
// BODIES
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
#define $ModRM              (node->ModRM)
#define $opcode             (node->opcode)
#define $SIB                (node->SIB)

#define $disp32             (node->disp32)
#define $imm32              (node->imm32)

//--------------------------------------------------------------------------------------------------------------------------------
// { set
//--------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------
// pref
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool binary_node_set_operand_pref(binary_node *const node)
{
    log_verify(node != nullptr, false);

    $is_operand_pref = true;
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool binary_node_set_address_pref(binary_node *const node)
{
    log_verify(node != nullptr, false);

    $is_address_pref = true;
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// REX
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool binary_node_set_REX(binary_node *const node, const BYTE REX)
{
    log_verify(node != nullptr, false);

    $is_REX = true;
    $REX    =  REX;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool binary_node_set_REX_W(binary_node *const node)
{
    log_verify(node != nullptr, false);

    $is_REX = true;             //      _______________
    $REX    = $REX | (1 << 3);  // REX |0|1|0|0|W|r|x|b|
                                //      ^ ^ ^ ^ ^ ^ ^ ^
    return true;                //      7 6 5 4 3 0 1 2
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool binary_node_set_REX_R(binary_node *const node)
{
    log_verify(node != nullptr, false);

    $is_REX = true;             //      _______________
    $REX    = $REX | (1 << 2);  // REX |0|1|0|0|w|R|x|b|
                                //      ^ ^ ^ ^ ^ ^ ^ ^
    return true;                //      7 6 5 4 3 2 1 0
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool binary_node_set_REX_X(binary_node *const node)
{
    log_verify(node != nullptr, false);

    $is_REX = true;             //      _______________
    $REX    = $REX | (1 << 1);  // REX |0|1|0|0|w|r|X|b|
                                //      ^ ^ ^ ^ ^ ^ ^ ^
    return true;                //      7 6 5 4 3 2 1 0
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool binary_node_set_REX_B(binary_node *const node)
{
    log_verify(node != nullptr, false);

    $is_REX = true;             //      _______________
    $REX    = $REX | (1 << 0);  // REX |0|1|0|0|w|r|x|B|
                                //      ^ ^ ^ ^ ^ ^ ^ ^
    return true;                //      7 5 5 4 3 2 1 0
}

//--------------------------------------------------------------------------------------------------------------------------------
// opcode
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool binary_node_set_opcode(binary_node *const node, const BYTE opcode)
{
    log_verify(node != nullptr, false);

    $opcode = opcode;
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// disp32
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool binary_node_set_disp32(binary_node *const node, const DWORD disp32)
{
    log_verify(node != nullptr, false);

    $is_disp32 = true;
    $disp32 = disp32;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// imm32
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool binary_node_set_imm32(binary_node *const node, const DWORD imm32)
{
    log_verify(node != nullptr, false);

    $is_imm32 = true;
    $imm32 = imm32;

    return true;
}

// } set
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

// } BINARY
//================================================================================================================================

#endif //BIN_H
