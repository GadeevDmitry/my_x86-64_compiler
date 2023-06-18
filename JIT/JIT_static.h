#ifndef JIT_STATIC_H
#define JIT_STATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define LOG_NTRACE
#define LOG_NLEAK
#include "../lib/logs/log.h"
#include "../lib/algorithm/algorithm.h"

#include "../frontend/frontend.h"
#include "../ast/ast.h"
#include "../backend/backend.h"

#include "JIT.h"

//--------------------------------------------------------------------------------------------------------------------------------

typedef long long type_t;

static const int    SCALE     =  100;
static const size_t PAGE_SIZE = 4096;

//================================================================================================================================
// JIT
//================================================================================================================================

struct JIT
{
    buffer *exe;
    void   *RAM;

    size_t global_data_size;
    size_t main_func_pc;

    type_t (*JIT_input )();
    void   (*JIT_output)(type_t result);
};

//--------------------------------------------------------------------------------------------------------------------------------

#define $exe            (run->exe)
#define $RAM            (run->RAM)

#define $glob_data_size (run->global_data_size)
#define $main_pc        (run->main_func_pc)

#define $input          (run->JIT_input)
#define $output         (run->JIT_output)

//--------------------------------------------------------------------------------------------------------------------------------
// init
//--------------------------------------------------------------------------------------------------------------------------------

static JIT *JIT_init(                const char *const source_code);
static bool JIT_init(JIT *const run, const char *const source_code);

//--------------------------------------------------------------------------------------------------------------------------------
// delete
//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_delete(JIT *const run);

//--------------------------------------------------------------------------------------------------------------------------------
// execute
//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_execute(/* rdi */ const size_t glob_data_size,
                        /* rsi */ const size_t main_pc       , /* rdx */ type_t (*const jit_input )(),
                                                               /* rcx */ void   (*const jit_output)(type_t));

//--------------------------------------------------------------------------------------------------------------------------------
// lib
//--------------------------------------------------------------------------------------------------------------------------------

static type_t JIT_input ();
static void   JIT_output(type_t result);

#endif //JIT_STATIC_H
