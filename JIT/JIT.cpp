#include "JIT_static.h"

//================================================================================================================================
// JIT
//================================================================================================================================

bool JIT_run(const char *const source_code)
{
    LOG_VERIFY(source_code != nullptr, false);

    JIT *run = JIT_init(source_code);
    if (run == nullptr) return false;

    JIT_execute((size_t) $RAM, $main_pc, $glob_data_size, $sqrt, $input, $output);
    JIT_delete(run);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// init
//--------------------------------------------------------------------------------------------------------------------------------

static JIT *JIT_init(const char *const source_code)
{
    LOG_ASSERT(source_code != nullptr);

    JIT *run = (JIT *) LOG_CALLOC(1, sizeof(JIT));
    LOG_VERIFY(run != nullptr, nullptr);

    if (!JIT_init(run, source_code)) { LOG_FREE(run); return nullptr; }
    return run;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool JIT_init(JIT *const run, const char *const source_code)
{
    LOG_ASSERT(run         != nullptr);
    LOG_ASSERT(source_code != nullptr);

    $RAM    = (type_t *) LOG_CALLOC(1, 10000UL /* RAM size */);
    $exe    = nullptr;

    $sqrt   = JIT_sqrt;
    $input  = JIT_input;
    $output = JIT_output;

    LOG_VERIFY($RAM != nullptr, false);

    size_t  var_quantity = 0UL,
           func_quantity = 0UL;

    AST_node *ast = nullptr;
    buffer   *exe = nullptr;

    ast = frontend(source_code, &var_quantity, &func_quantity, &$main_pc); if (ast == nullptr) { JIT_dtor(run); return false; }
    exe = backend (ast        ,  var_quantity,  func_quantity, &$main_pc, &$glob_data_size);

    $exe = mmap(NULL, exe->size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    LOG_VERIFY($exe != nullptr, false);

    memcpy($exe, exe->beg, exe->size);
    $main_pc += (size_t) $exe;

    AST_tree_delete(ast);
    buffer_delete  (exe);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_delete(JIT *const run)
{
    if (run == nullptr) return;

    JIT_dtor(run);
    LOG_FREE(run);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_dtor(JIT *const run)
{
    if (run == nullptr) return;

    LOG_FREE($RAM);
}

//--------------------------------------------------------------------------------------------------------------------------------
// execute
//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_execute(const size_t RAM,
                        const size_t main_pc,
                        const size_t global_data_size, type_t (*const jit_sqrt  )(type_t),
                                                       type_t (*const jit_input )(),
                                                       void   (*const jit_output)(type_t))
{
    register auto   RAM_              asm("r10") = RAM;
    register auto   main_pc_          asm("rsi") = main_pc;
    register auto   global_data_size_ asm("rdi") = global_data_size;
    register auto   jit_sqrt_         asm("r11") = jit_sqrt;
    register auto   jit_input_        asm("r8" ) = jit_input;
    register auto   jit_output_       asm("r9" ) = jit_output;

    register size_t SCALE_            asm("rcx") = SCALE;

    asm volatile(
        ".intel_syntax noprefix\n"

        "push rbx\n"
        "push rbp\n"
        "push r12\n"
        "push r13\n"
        "push r14\n"
        "push r15\n"

        "mov rbp, rdi\n" // rbp := main RAM offset

        "call rsi\n"

        "pop r15\n"
        "pop r14\n"
        "pop r13\n"
        "pop r12\n"
        "pop rbp\n"
        "pop rbx\n"

        ".att_syntax prefix\n"
    :
    : "r"(RAM_), "r"(main_pc_), "r"(global_data_size_), "r"(jit_sqrt_), "r"(jit_input_), "r"(jit_output_), "r"(SCALE_)
    : "cc", "memory");
}

//--------------------------------------------------------------------------------------------------------------------------------
// lib
//--------------------------------------------------------------------------------------------------------------------------------

static type_t JIT_sqrt(type_t number)
{
    type_t result = 0;
    while (result * result <= number * SCALE)
    {
        result++;
    }

    result--;
    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

static type_t JIT_input()
{
    printf("INPUT : ");

    char input_buf[100] = {};
    scanf("%99s", input_buf);

    long long real_number = 0;
    sscanf(input_buf, "%lld", &real_number);

    type_t converted_number = (real_number * SCALE);
    return converted_number;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_output(type_t converted_number)
{
    double real_number = (double) converted_number / SCALE;
    printf("OUTPUT: %.2lf\n", real_number);
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_dump(const JIT *const run)
{
    LOG_VERIFY(run != nullptr, (void) 0);

    JIT_header_dump(run);
    JIT_fields_dump(run);
    JIT_ending_dump();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_header_dump(const JIT *const run)
{
    LOG_ASSERT(run != nullptr);

    LOG_TAB_SERVICE_MESSAGE("JIT (addr: %p)\n"
                            "{", "\n", run);
    LOG_TAB++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_fields_dump(const JIT *const run)
{
    LOG_ASSERT(run != nullptr);

    USUAL_FIELD_DUMP("RAM             ", "%p" , $RAM);
    USUAL_FIELD_DUMP("exe             ", "%p" , $exe);
    USUAL_FIELD_DUMP("global_data_size", "%lu", $glob_data_size);
    USUAL_FIELD_DUMP("main_func_pc    ", "%lx", $main_pc);

    LOG_MESSAGE("\n");

    USUAL_FIELD_DUMP("JIT_sqrt  ", "%p", $sqrt);
    USUAL_FIELD_DUMP("JIT_input ", "%p", $input);
    USUAL_FIELD_DUMP("JIT_output", "%p", $output);

    LOG_MESSAGE("\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_ending_dump()
{
    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("}", "\n\n");
}
