#ifndef BIN_TRANSLATOR_STATIC_H
#define BIN_TRANSLATOR_STATIC_H

#include <stdio.h>
#include <stdlib.h>

#define LOG_NTRACE
#include "../../lib/logs/log.h"
#include "../../lib/array/array.h"

#include "../x64/x64.h"
#include "../binary/bin.h"

#include "bin_translator.h"
#include "binary_info/bin_info.h"

//================================================================================================================================
// DSL
//================================================================================================================================

#define $type   (x64_cmd->type)
#define $cc     (x64_cmd->cc)

#define $op_1   (x64_cmd->op_1)
#define $op_2   (x64_cmd->op_2)

//================================================================================================================================
// BINARY_TRANSLATOR
//================================================================================================================================

static void translate_general           (binary_info *const binary, const vector   *const x64);
static void translate_x64_node          (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_general_arithmetic(binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_imul_idiv         (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_setcc             (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_jcc               (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_call              (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_call_rel          (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_call_abs          (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_jmp               (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_ret               (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_push              (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_push_imm          (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_push_reg          (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_push_mem          (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_pop               (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_pop_reg           (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);
static void translate_pop_mem           (binary_info *const binary, const x64_node *const x64_cmd, const size_t cmd_num);

static void translate_push_pop_mem      (binary_info *const binary, const x64_node *const x64_cmd, binary_node *const bin_cmd);

#endif //BIN_TRANSLATOR_STATIC_H
