#ifndef IR_H
#define IR_H

#include <stdlib.h>

//================================================================================================================================
// IR_node
//================================================================================================================================

enum IR_INSTRUCTION
{
    IR_INSTRUCTION_PUSH ,
    IR_INSTRUCTION_POP  ,

    IR_INSTRUCTION_ADD  ,
    IR_INSTRUCTION_SUB  ,
    IR_INSTRUCTION_MUL  ,
    IR_INSTRUCTION_DIV  ,

    IR_INSTRUCTION_JMP  ,
    IR_INSTRUCTION_JA   ,
    IR_INSTRUCTION_JB   ,
    IR_INSTRUCTION_JE   ,
    IR_INSTRUCTION_JAE  ,
    IR_INSTRUCTION_JBE  ,
    IR_INSTRUCTION_JNE  ,

    IR_INSTRUCTION_CALL ,
    IR_INSTRUCTION_RET  ,

    IR_INSTRUCTION_IN   ,
    IR_INSTRUCTION_OUT  ,
};

//--------------------------------------------------------------------------------------------------------------------------------

struct IR_node
{
    IR_INSTRUCTION type;

    struct
    {
        bool is_reg_arg;
        bool is_mem_arg;
        bool is_imm_arg;
    }
    is_arg;

    unsigned char reg_num;
    int           imm_val;
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor dtor
//--------------------------------------------------------------------------------------------------------------------------------

bool     IR_node_ctor  (IR_node *const  node, const IR_INSTRUCTION type, const bool is_reg, const bool is_mem, const bool is_imm, ...);
IR_node *IR_node_new   (                      const IR_INSTRUCTION type, const bool is_reg, const bool is_mem, const bool is_imm, ...);
void     IR_node_delete(void    *const _node);

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void IR_node_dump(const void *const _node);

#endif //IR_H
