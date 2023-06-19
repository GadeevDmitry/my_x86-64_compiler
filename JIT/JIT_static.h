#ifndef JIT_STATIC_H
#define JIT_STATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/mman.h>

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "../lib/logs/log.h"
#include "../lib/algorithm/algorithm.h"

#include "../frontend/frontend.h"
#include "../ast/ast.h"
#include "../backend/backend.h"

#include "JIT.h"

//--------------------------------------------------------------------------------------------------------------------------------

typedef long long type_t;

static const int SCALE = 100;

//================================================================================================================================
// JIT
//================================================================================================================================

struct JIT
{
    void *exe;
    void *RAM;

    size_t global_data_size;
    size_t main_func_pc;

    type_t (*JIT_pow)   (type_t basis, type_t indicator);
    type_t (*JIT_input )();
    void   (*JIT_output)(type_t number);
};

//--------------------------------------------------------------------------------------------------------------------------------

#define $exe            (run->exe)
#define $RAM            (run->RAM)

#define $glob_data_size (run->global_data_size)
#define $main_pc        (run->main_func_pc)

#define $pow            (run->JIT_pow)
#define $input          (run->JIT_input)
#define $output         (run->JIT_output)

//--------------------------------------------------------------------------------------------------------------------------------
// init
//--------------------------------------------------------------------------------------------------------------------------------

static JIT *JIT_init(                const char *const source_code);
static bool JIT_init(JIT *const run, const char *const source_code);

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_delete(JIT *const run);
static void JIT_dtor  (JIT *const run);

//--------------------------------------------------------------------------------------------------------------------------------
// execute
//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_execute(/* rdi */ const size_t RAM,
                        /* rsi */ const size_t main_pc, 
                        /* rdx */ const size_t global_data_size, /* rcx */  type_t (*const jit_pow   )(type_t, type_t),
                                                                 /* r8  */  type_t (*const jit_input )(),
                                                                 /* r9  */  void   (*const jit_output)(type_t));

//--------------------------------------------------------------------------------------------------------------------------------
// lib
//--------------------------------------------------------------------------------------------------------------------------------

static type_t JIT_pow  (type_t basis, type_t indicator);
static type_t JIT_input();
static void   JIT_output           (type_t number);
static void   JIT_output_not_scaled(type_t number);

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_dump       (const JIT *const run);

static void JIT_header_dump(const JIT *const run);
static void JIT_fields_dump(const JIT *const run);
static void JIT_ending_dump();

#endif //JIT_STATIC_H
