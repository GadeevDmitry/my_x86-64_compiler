#ifndef IR_STATIC_H
#define IR_STATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../../lib/logs/log.h"
#include "../../lib/algorithm/algorithm.h"

#include "IR.h"

//================================================================================================================================
// IR_NODE
//================================================================================================================================

static const char *IR_INSTRUCTION_NAMES[] =
{
    "PUSH"  ,
    "POP"   ,

    "ADD"   ,
    "SUB"   ,
    "MUL"   ,
    "DIV"   ,

    "JMP"   ,
    "JA"    ,
    "JB"    ,
    "JE"    ,
    "JAE"   ,
    "JBE"   ,
    "JNE"   ,

    "CALL"  ,
    "RET"   ,

    "IN"    ,
    "OUT"   ,
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

static bool IR_node_ctor(IR_node *const node, const IR_INSTRUCTION type, const bool is_reg,
                                                                         const bool is_mem,
                                                                         const bool is_imm, va_list ap);

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void IR_node_header_dump(const IR_node *const node);
static                 void IR_node_fields_dump(const IR_node *const node);
static __always_inline void IR_node_ending_dump();

#endif //IR_STATIC_H
