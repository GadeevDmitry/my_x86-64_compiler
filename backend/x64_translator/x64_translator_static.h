#ifndef X64_TRANSLATOR_STATIC_H
#define X64_TRANSLATOR_STATIC_H

#include <stdio.h>
#include <stdlib.h>

#define LOG_NTRACE
#include "../../lib/logs/log.h"

#include "../IR/IR.h"
#include "../x64/x64.h"

#include "x64_info/x64_info.h"
#include "x64_translator.h"

//================================================================================================================================
// X64_TRANSLATOR
//================================================================================================================================

static void translate_general    (x64_info *const x64, const vector  *const IR);
static void translate_IR_node    (x64_info *const x64, const IR_node *const IR_cmd);

static void translate_add_sub    (x64_info *const x64, const IR_node *const IR_cmd);
static void translate_mul        (x64_info *const x64, const IR_node *const IR_cmd);
static void translate_div        (x64_info *const x64, const IR_node *const IR_cmd);
static void translate_conditional(x64_info *const x64, const IR_node *const IR_cmd);
static void translate_not        (x64_info *const x64, const IR_node *const IR_cmd);
static void translate_log_or     (x64_info *const x64, const IR_node *const IR_cmd);
static void translate_log_and    (x64_info *const x64, const IR_node *const IR_cmd);
static void translate_jmp        (x64_info *const x64, const IR_node *const IR_cmd);
static void translate_jcc        (x64_info *const x64, const IR_node *const IR_cmd);
static void translate_push_pop   (x64_info *const x64, const IR_node *const IR_cmd);
static void translate_call       (x64_info *const x64, const IR_node *const IR_cmd);
static void translate_ret        (x64_info *const x64, const IR_node *const IR_cmd);
static void translate_in_out     (x64_info *const x64, const IR_node *const IR_cmd);

#endif //X64_TRANSLATOR_STATIC_H
