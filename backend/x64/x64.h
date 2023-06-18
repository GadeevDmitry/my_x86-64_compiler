#ifndef X64_H
#define X64_H

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "../../lib/logs/log.h"

//================================================================================================================================
// GLOBAL
//================================================================================================================================

enum X64_CMD
{
    X64_CMD_ADD     ,
    X64_CMD_SUB     ,
    X64_CMD_IMUL    ,
    X64_CMD_IDIV    ,

    X64_CMD_OR      ,
    X64_CMD_AND     ,
    X64_CMD_XOR     ,

    X64_CMD_CMP     ,
    X64_CMD_TEST    ,

    X64_CMD_SETcc   ,
    X64_CMD_Jcc     ,

    X64_CMD_CALL    ,
    X64_CMD_JMP     ,
    X64_CMD_RET     ,

    X64_CMD_PUSH    ,
    X64_CMD_POP     ,
};

//--------------------------------------------------------------------------------------------------------------------------------

enum X64_cc
{
    X64_cc_G  = 0xF , ///< if greater           (ZF = 0 && SF  = OF)
    X64_cc_L  = 0xC , ///< if less              (          SF != OF)
    X64_cc_E  = 0x4 , ///< if equal             (ZF = 1            )
    X64_cc_GE = 0xD , ///< if greater or equal  (          SF  = OF)
    X64_cc_LE = 0xE , ///< if less or equal     (ZF = 1 || SF != OF)
    X64_cc_NE = 0x5 , ///< if not equal         (ZF = 0            )
    X64_cc_no =  -1 , ///< no cc
};

//--------------------------------------------------------------------------------------------------------------------------------

enum GPR
{
    RAX ,
    RCX ,
    RDX ,
    RBX ,
    RSP ,
    RBP ,
    RSI ,
    RDI ,

    R8  ,
    R9  ,
    R10 ,
    R11 ,
    R12 ,
    R13 ,
    R14 ,
    R15 ,

    GPR_UNDEF = -1  ,
};

//================================================================================================================================
// { X64_operand
//================================================================================================================================

struct x64_operand
{
    struct
    {
        bool is_reg;
        bool is_mem;
        bool is_imm;
    };

    GPR  reg;
    int  imm;
    char scale_factor; // addr = scale_factor * (reg + imm)
};                     // addr - адрес относительно начала блока данных

//================================================================================================================================
// HEADERS
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

x64_operand *x64_operand_new   (                             const bool is_reg, const bool is_mem, const bool is_imm, ...);
bool         x64_operand_ctor  (x64_operand *const  operand, const bool is_reg, const bool is_mem, const bool is_imm, ...);
void         x64_operand_delete(void        *const _operand);

//--------------------------------------------------------------------------------------------------------------------------------
// set get
//--------------------------------------------------------------------------------------------------------------------------------

static bool x64_operand_set_reg(      x64_operand *const operand, const GPR           reg) __attribute__((always_inline));
static bool x64_operand_set_imm(      x64_operand *const operand, const int           imm) __attribute__((always_inline));
static bool x64_operand_set_scl(      x64_operand *const operand, const char scale_factor) __attribute__((always_inline));
static GPR  x64_operand_get_reg(const x64_operand *const operand)                          __attribute__((always_inline));
static int  x64_operand_get_imm(const x64_operand *const operand)                          __attribute__((always_inline));
static char x64_operand_get_scl(const x64_operand *const operand)                          __attribute__((always_inline));

//================================================================================================================================
// BODIES
//================================================================================================================================

#define $is_reg (operand->is_reg)
#define $is_mem (operand->is_mem)
#define $is_imm (operand->is_imm)

#define $reg    (operand->reg)
#define $imm    (operand->imm)
#define $scl    (operand->scale_factor)

//--------------------------------------------------------------------------------------------------------------------------------
// set get
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool x64_operand_set_reg(x64_operand *const operand, const GPR reg)
{
    log_verify(operand != nullptr, false);

    $reg = reg;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool x64_operand_set_imm(x64_operand *const operand, const int imm)
{
    log_verify(operand != nullptr, false);

    $imm = imm;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool x64_operand_set_scl(x64_operand *const operand, const char scale_factor)
{
    log_verify(operand != nullptr, false);
    log_verify_verbose((scale_factor == 1) ||
                       (scale_factor == 2) ||
                       (scale_factor == 4) ||
                       (scale_factor == 8), "wrong scale factor", false);

    $scl = scale_factor;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline GPR x64_operand_get_reg(const x64_operand *const operand)
{
    log_verify(operand != nullptr, GPR_UNDEF);

    return $reg;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline int x64_operand_get_imm(const x64_operand *const operand)
{
    log_verify(operand != nullptr, -1);

    return $imm;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline char x64_operand_get_scl(const x64_operand *const operand)
{
    log_verify(operand != nullptr, -1);

    return $scl;
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $is_reg
#undef $is_mem
#undef $is_imm

#undef $reg
#undef $imm
#undef $scl

// } X64_operand
//================================================================================================================================

//================================================================================================================================
// { X64_node
//================================================================================================================================

struct x64_node
{
    X64_CMD     type;
    X64_cc        cc;

    x64_operand op_1;   // intel syntax
    x64_operand op_2;   // intel syntax
};

//================================================================================================================================
// HEADERS
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

x64_node *x64_node_new   (                       const X64_CMD type, const X64_cc cc = X64_cc_no);
bool      x64_node_ctor  (x64_node *const  node, const X64_CMD type, const X64_cc cc = X64_cc_no);
void      x64_node_delete(void     *const _node);

//--------------------------------------------------------------------------------------------------------------------------------
// set
//--------------------------------------------------------------------------------------------------------------------------------

       bool x64_node_set_operand_1    (x64_node *const node, const bool is_reg, const bool is_mem, const bool is_imm, ...);
       bool x64_node_set_operand_2    (x64_node *const node, const bool is_reg, const bool is_mem, const bool is_imm, ...);
static bool x64_node_set_operand_1_scl(x64_node *const node, const char scale_factor) __attribute__((always_inline));
static bool x64_node_set_operand_2_scl(x64_node *const node, const char scale_factor) __attribute__((always_inline));

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void x64_node_dump(const void *const _node);

//================================================================================================================================
// BODIES
//================================================================================================================================

#define $type   (node->type)
#define $cc     (node->cc)

#define $op_1   (node->op_1)
#define $op_2   (node->op_2)

//--------------------------------------------------------------------------------------------------------------------------------
// set
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool x64_node_set_operand_1_scl(x64_node *const node, const char scale_factor)
{
    log_verify(node != nullptr, false);

    return x64_operand_set_scl(&$op_1, scale_factor);
}


//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool x64_node_set_operand_2_scl(x64_node *const node, const char scale_factor)
{
    log_verify(node != nullptr, false);

    return x64_operand_set_scl(&$op_2, scale_factor);
}

//--------------------------------------------------------------------------------------------------------------------------------

#undef $type
#undef $cc

#undef $op_1
#undef $op_2

// } X64_node
//================================================================================================================================

#endif //X64_H
