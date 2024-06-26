#ifndef PROG_INFO_STATIC_H
#define PROG_INFO_STATIC_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "lib/include/log.h"
#include "lib/include/algorithm.h"

#include "IR/IR.h"
#include "prog_info.h"

//================================================================================================================================
// DSL
//================================================================================================================================

#define $IR     (prog->IR)

#define $label  (prog->label_addr)
#define $func   (prog-> func_addr)
#define $glob   (prog-> glob_addr)
#define $loc    (prog->  loc_addr)

#define $scope  (prog->scope)
#define $rel    (prog->relative)

//================================================================================================================================
// FUNCTION_DECLARATION
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

static bool loc_addr_ctor(array *const loc_addr);

//--------------------------------------------------------------------------------------------------------------------------------
// var declaration
//--------------------------------------------------------------------------------------------------------------------------------

static bool prog_info_is_local_var_redeclared(prog_info *const prog, const size_t var_ind);

#endif //PROG_INFO_STATIC_H
