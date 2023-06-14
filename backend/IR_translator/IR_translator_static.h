#ifndef IR_TRANSLATOR_STATIC_H
#define IR_TRANSLATOR_STATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_NTRACE
#define LOG_NLEAK
#include "../../lib/logs/log.h"

#include "../IR/IR.h"

#include "prog_info/prog_info.h"
#include "IR_translator.h"

//================================================================================================================================
// GLOBAL
//================================================================================================================================

#pragma GCC diagnostic ignored "-Wswitch-enum"

#include "../translator.h"

//================================================================================================================================
// IR_TRANSLATOR
//================================================================================================================================

static void   translate_general                  (prog_info *const prog, const AST_node *const subtree);
static void   translate_args                     (prog_info *const prog, const AST_node *const subtree);
static void   translate_operators                (prog_info *const prog, const AST_node *const subtree);

static void   translate_operator_independent     (prog_info *const prog, const AST_node *const subtree);
static void   translate_if                       (prog_info *const prog, const AST_node *const subtree);
static void   translate_while                    (prog_info *const prog, const AST_node *const subtree);
static void   translate_return                   (prog_info *const prog, const AST_node *const subtree);

static void   translate_func_call_ret_val_ignored(prog_info *const prog, const AST_node *const subtree);
static void   translate_func_call_ret_val_used   (prog_info *const prog, const AST_node *const subtree);
static size_t translate_params                   (prog_info *const prog, const AST_node *const subtree);

static void   translate_input                    (prog_info *const prog, const AST_node *const subtree);
static void   translate_output                   (prog_info *const prog, const AST_node *const subtree);

static void   translate_assignment_independent   (prog_info *const prog, const AST_node *const subtree);
static void   translate_assignment_dependent     (prog_info *const prog, const AST_node *const subtree);

static void   translate_expression               (prog_info *const prog, const AST_node *const subtree);

static void   translate_immediate_int_operand    (prog_info *const prog, const AST_node *const subtree);
static void   translate_variable_operand         (prog_info *const prog, const AST_node *const subtree);
static void   translate_operator_dependent       (prog_info *const prog, const AST_node *const subtree);

static void   translate_operator_unary           (prog_info *const prog, const AST_node *const subtree);
static void   translate_operator_binary          (prog_info *const prog, const AST_node *const subtree);

#endif //IR_TRANSLATOR_STATIC_H
