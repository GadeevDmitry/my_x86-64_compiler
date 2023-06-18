#ifndef STRUCTS_STATIC_H
#define STRUCTS_STATIC_H

#include <stdio.h>
#include <stdlib.h>

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "../../../lib/logs/log.h"
#include "../../../lib/algorithm/algorithm.h"

#include "bin_info.h"
#include "../../x64/x64.h"

//================================================================================================================================
// BINARY_INFO
//================================================================================================================================

#define $cmds       (binary->cmds)
#define $cmd_addr   (binary->cmd_addr)

#define $pc         (binary->pc)
#define $main_addr  (binary->main_func_addr)

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

static void binary_info_fixup_cmd_addr      (binary_info *const binary, const vector *const x64, const size_t cmd_num);
static void binary_info_fixup_main_func_addr(binary_info *const binary);

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static void binary_info_header_dump(const binary_info *const binary) __attribute__((always_inline));
static void binary_info_ending_dump()                                __attribute__((always_inline));

static void binary_info_fields_dump(const binary_info *const binary , const vector   *const x64);
static void binary_info_cmds_dump  (const binary_info *const binary , const vector   *const x64);
static void binary_info_cmd_dump   (const binary_node *const bin_cmd, const x64_node *const x64_cmd, const size_t cmd_pc,
                                                                                                     const size_t cmd_num);

#endif //STRUCTS_STATIC_H
