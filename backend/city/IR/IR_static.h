#ifndef IR_STATIC_H
#define IR_STATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "lib/include/log.h"
#include "lib/include/algorithm.h"

#include "IR.h"

//================================================================================================================================
// IR_NODE
//================================================================================================================================

static const char *IR_CMD_NAMES[] =
{
    "ADD"           ,
    "SUB"           ,
    "MUL"           ,
    "DIV"           ,
    "SQRT"          ,

    "ARE_EQUAL"     ,
    "MORE"          ,
    "LESS"          ,
    "MORE_EQUAL"    ,
    "LESS_EQUAL"    ,
    "NOT_EQUAL"     ,
    "NOT"           ,

    "LOG_OR"        ,
    "LOG_AND"       ,

    "JMP"           ,
    "JG"            ,
    "JL"            ,
    "JE"            ,
    "JGE"           ,
    "JLE"           ,
    "JNE"           ,

    "PUSH"          ,
    "POP"           ,

    "CALL"          ,
    "RET"           ,

    "IN"            ,
    "OUT"           ,
};

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void IR_node_header_dump(const IR_node *const node);
static                 void IR_node_fields_dump(const IR_node *const node);
static __always_inline void IR_node_ending_dump();

#endif //IR_STATIC_H
