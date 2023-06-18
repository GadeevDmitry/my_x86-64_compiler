#include "JIT_static.h"

//================================================================================================================================
// JIT
//================================================================================================================================

bool JIT_run(const char *const source_code)
{
    log_verify(source_code != nullptr, false);

    JIT *run = JIT_init(source_code);
    if (run == nullptr) return false;

    JIT_execute($glob_data_size, $main_pc, $input, $output);
    JIT_delete (run);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// init
//--------------------------------------------------------------------------------------------------------------------------------

static JIT *JIT_init(const char *const source_code)
{
    log_assert(source_code != nullptr);

    JIT *run = (JIT *) log_calloc(1, sizeof(JIT));
    log_verify(run != nullptr, nullptr);

    if (!JIT_init(run, source_code)) { log_free(run); return nullptr; }
    return run;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool JIT_init(JIT *const run, const char *const source_code)
{
    log_assert(run         != nullptr);
    log_assert(source_code != nullptr);

    $RAM    = (type_t *) log_calloc(1, 10000UL /* RAM size */);
    $exe    = nullptr;

    $input  = JIT_input;
    $output = JIT_output;

    log_verify($RAM != nullptr, false);

    size_t  var_quantity = 0UL,
           func_quantity = 0UL;

    AST_node *ast = nullptr;

    ast  = frontend(source_code, &var_quantity, &func_quantity,                &$main_pc); if (ast == nullptr) { JIT_delete(run); return false; }
    $exe = backend (ast        ,  var_quantity,  func_quantity, (size_t) $RAM, &$main_pc, &$glob_data_size);

    size_t exe_page_begin  = (size_t) ($exe->buff_beg) / PAGE_SIZE;
    size_t exe_buff_offset = (size_t) ($exe->buff_beg) - exe_page_begin;

    $main_pc += exe_buff_offset;

    mprotect((void *) exe_page_begin, exe_buff_offset + $exe->buff_size, PROT_EXEC);
    mprotect((void *) exe_page_begin, exe_buff_offset                  , PROT_READ | PROT_WRITE);

    AST_tree_delete(ast);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// delete
//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_delete(JIT *const run)
{
    if (run == nullptr) return;

    buffer_free($exe);
    log_free   ($RAM);
    log_free    (run);
}

//--------------------------------------------------------------------------------------------------------------------------------
// execute
//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void JIT_execute(/* rdi */ const size_t glob_data_size,
                        /* rsi */ const size_t main_pc       , /* rdx */ type_t (*const jit_input )(),
                                                               /* rcx */ void   (*const jit_output)(type_t))
{
    asm(
        ".intel_syntax noprefix\n"

        "push rax\n"
        "push rbx\n"
        "push rbp\n"
        "push r12\n"
        "push r13\n"
        "push r14\n"
        "push r15\n"

        "mov r8 , rdx\n" // r8  = input
        "mov r9 , rcx\n" // r9  = output
        "mov rbp, rdi\n" // rbp = main func frame
        "mov rcx, 100\n" // rcx = SCALE

        "call rsi\n"

        "pop r15\n"
        "pop r14\n"
        "pop r13\n"
        "pop r12\n"
        "pop rbp\n"
        "pop rbx\n"
        "pop rax\n"

        ".att_syntax prefix\n"
    );
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------
// lib
//--------------------------------------------------------------------------------------------------------------------------------

static type_t JIT_input()
{
    type_t result = 0;
    fprintf(stderr, "INPUT: ");

    int status = fscanf(stderr, "%lld", &result);
    while (status == 0)
    {
        fprintf(stderr, "INPUT ERROR, try again: ");
        status = fscanf(stderr, "%lld", &result);
    }

    return SCALE * result;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_output(type_t result)
{
    double _result = (double) result / SCALE;

    fprintf(stderr, "OUTPUT: %lg\n", _result);
}
