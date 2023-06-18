#include "JIT_static.h"

//================================================================================================================================
// JIT
//================================================================================================================================

bool JIT_run(const char *const source_code)
{
    log_verify(source_code != nullptr, false);

    JIT *run = JIT_init(source_code);
    if (run == nullptr) return false;

    JIT_execute((size_t) $RAM, $main_pc, $glob_data_size, $output, $input);
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
    buffer   *exe = nullptr;

    ast = frontend(source_code, &var_quantity, &func_quantity, &$main_pc); if (ast == nullptr) { JIT_delete(run); return false; }
    exe = backend (ast        ,  var_quantity,  func_quantity, &$main_pc, &$glob_data_size);

    $exe = mmap(NULL, exe->buff_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    log_verify($exe != nullptr, false);

    memcpy($exe, exe->buff_beg, exe->buff_size);
    $main_pc += (size_t) $exe;

    AST_tree_delete(ast);
    buffer_free    (exe);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// delete
//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_delete(JIT *const run)
{
    if (run == nullptr) return;

    log_free($RAM);
    log_free (run);
}

//--------------------------------------------------------------------------------------------------------------------------------
// execute
//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void JIT_execute(/* rdi */ const size_t RAM,
                        /* rsi */ const size_t main_pc, 
                        /* rdx */ const size_t global_data_size, /* rcx */  void   (*const jit_output)(type_t),
                                                                 /* r8  */  type_t (*const jit_input )())
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
                         // r8  = input
        "mov r9 , rcx\n" // r9  = output
        "mov r10, rdi\n" // r10 = RAM
        "mov rbp, rdx\n" // rbp = main frame offset
        "mov rcx, 100\n" // rcx = SCALE

//      "int 3\n"

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
    fprintf(stderr, "INPUT : ");

    type_t number = 0;
    type_t digit  = 0;
    type_t  sign  = 1;
    bool is_sign  = false;

    for (digit = getc(stdin); digit != EOF && digit != '\n' && digit != '\0'; digit = getc(stdin))
    {
        if (digit == '-')
        {
            if (!is_sign) { is_sign = true; sign = -1; }
            continue;
        }
        if (!isdigit((int) digit)) continue;

        number *= 10;
        number += digit - '0';
    }

    number *= sign;
    number *= SCALE;

    return number;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_output(type_t number)
{
    fprintf(stderr, "OUTPUT: ");

    type_t integer    = number / SCALE;
    type_t fractional = number % SCALE;

    fractional = fractional < 0 ? fractional + SCALE : fractional;

    JIT_output_not_scaled(integer);
    putc(',', stdout);

    JIT_output_not_scaled(fractional);
    putc('\n', stdout);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_output_not_scaled(type_t number)
{
    size_t buff_sz = 100;
    size_t index   = buff_sz - 2;

    bool is_neg = false;
    char out_buff[buff_sz] = {};

    if (number < 0)
    {
        is_neg = true;
        number *= -1;
    }

    do
    {
        out_buff[index--] = (char) (number % 10) + '0';
        number /= 10;
    }
    while (number != 0 && index < buff_sz - 2);

    if  (is_neg) out_buff[index] = '-';
    else         index++;

    for (; out_buff[index] != '\0'; ++index) putc(out_buff[index], stdout);
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_dump(const JIT *const run)
{
    log_verify(run != nullptr, (void) 0);

    JIT_header_dump(run);
    JIT_fields_dump(run);
    JIT_ending_dump();
}

//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_header_dump(const JIT *const run)
{
    log_assert(run != nullptr);

    log_tab_service_message("JIT (addr: %p)\n"
                            "{", "\n", run);
    LOG_TAB++;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_fields_dump(const JIT *const run)
{
    log_assert(run != nullptr);

    usual_field_dump("RAM             ", "%p" , $RAM);
    usual_field_dump("exe             ", "%p" , $exe);
    usual_field_dump("global_data_size", "%lu", $glob_data_size);
    usual_field_dump("main_func_pc    ", "%lx", $main_pc);

    log_message("\n");

    usual_field_dump("JIT_input ", "%p", $input);
    usual_field_dump("JIT_output", "%p", $output);

    log_message("\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static void JIT_ending_dump()
{
    LOG_TAB--;
    log_tab_service_message("}", "\n\n");
}
