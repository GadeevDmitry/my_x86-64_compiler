#ifndef X64_STATIC_H
#define X64_STATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define LOG_NTRACE
#include "../../lib/logs/log.h"
#include "../../lib/algorithm/algorithm.h"

#include "x64.h"

//================================================================================================================================
// GLOBAL
//================================================================================================================================

const char *GPR_names[] =
{
    "RAX"   ,
    "RCX"   ,
    "RDX"   ,
    "RBX"   ,
    "RSP"   ,
    "RBP"   ,
    "RSI"   ,
    "RDI"   ,

    "R8"    ,
    "R9"    ,
    "R10"   ,
    "R11"   ,
    "R12"   ,
    "R13"   ,
    "R14"   ,
    "R15"   ,
};

//--------------------------------------------------------------------------------------------------------------------------------

const char *X64_CMD_names[] =
{
    "ADD"   ,
    "SUB"   ,
    "IMUL"  ,
    "IDIV"  ,

    "OR"    ,
    "AND"   ,
    "XOR"   ,

    "CMP"   ,
    "TEST"  ,

    "SETcc" ,
    "Jcc"   ,

    "CALL"  ,
    "JMP"   ,
    "RET"   ,

    "PUSH"  ,
    "POP"   ,
};

//--------------------------------------------------------------------------------------------------------------------------------

const char *X64_SETCC_names[] =
{
    "SETG"  ,
    "SETL"  ,
    "SETE"  ,
    "SETGE" ,
    "SETLE" ,
    "SETNE" ,
};

//--------------------------------------------------------------------------------------------------------------------------------

const char *X64_JCC_names[] =
{
    "JG"    ,
    "JL"    ,
    "JE"    ,
    "JGE"   ,
    "JLE"   ,
    "JNE"   ,
};

//================================================================================================================================
// X64_operand
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

static bool x64_operand_ctor(x64_operand *const operand, const bool is_reg,
                                                         const bool is_mem,
                                                         const bool is_imm, va_list ap);

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static void x64_operand_dump    (const x64_operand *const operand);

static void x64_operand_dump_reg(const x64_operand *const operand) __attribute__((always_inline));
static void x64_operand_dump_imm(const x64_operand *const operand) __attribute__((always_inline));
static void x64_operand_dump_mem(const x64_operand *const operand);

//================================================================================================================================
// X64_node
//================================================================================================================================

static void x64_node_header_dump(const x64_node *const node) __attribute__((always_inline));
static void x64_node_ending_dump()                           __attribute__((always_inline));

static void x64_node_fields_dump(const x64_node *const node);
static void x64_node_type_dump  (const x64_node *const node);
static void x64_node_typecc_dump(const x64_node *const node, const char *names[]);

#endif //X64_STATIC_H
