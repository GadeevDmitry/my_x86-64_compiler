#ifndef BIN_H
#define BIN_H

#include <stdlib.h>

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "lib/include/log.h"
#include "lib/include/buffer.h"

#include "../x64/x64.h"

//================================================================================================================================
// BINARY_NODE
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

//--------------------------------------------------------------------------------------------------------------------------------
// service
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_reset (binary_node *const  node);
void binary_node_delete(void        *const _node);

//--------------------------------------------------------------------------------------------------------------------------------
// set
//--------------------------------------------------------------------------------------------------------------------------------

bool binary_node_set_operand_pref(binary_node *const node);
bool binary_node_set_address_pref(binary_node *const node);

bool binary_node_set_REX         (binary_node *const node, const BYTE REX);
bool binary_node_set_REX_W       (binary_node *const node);
bool binary_node_set_REX_R       (binary_node *const node);
bool binary_node_set_REX_X       (binary_node *const node);
bool binary_node_set_REX_B       (binary_node *const node);

bool binary_node_set_opcode      (binary_node *const node, const BYTE opcode);

bool binary_node_set_ModRM       (binary_node *const node,       BYTE mod,       BYTE reg,       BYTE rm);
bool binary_node_set_ModRM       (binary_node *const node, const BYTE mod, const BYTE reg, const GPR  rm);
bool binary_node_set_ModRM       (binary_node *const node, const BYTE mod, const GPR  reg, const GPR  rm);

bool binary_node_set_SIB         (binary_node *const node,       BYTE scale,       BYTE index,       BYTE base);
bool binary_node_set_SIB         (binary_node *const node, const BYTE scale, const GPR  index, const GPR  base);
bool binary_node_set_SIB         (binary_node *const node, const BYTE scale, const GPR  index, const BYTE base);
bool binary_node_set_SIB         (binary_node *const node, const BYTE scale, const BYTE index, const GPR  base);

bool binary_node_set_disp32      (binary_node *const node, const DWORD disp32);
bool binary_node_set_imm32       (binary_node *const node, const DWORD  imm32);

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

#endif //BIN_H
