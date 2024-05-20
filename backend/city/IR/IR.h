#ifndef IR_H
#define IR_H

#include <stdlib.h>
#include <stdarg.h>

//================================================================================================================================
// IR_node
//================================================================================================================================

enum IR_CMD
{
    IR_CMD_ADD          ,
    IR_CMD_SUB          ,
    IR_CMD_MUL          ,
    IR_CMD_DIV          ,
    IR_CMD_SQRT         ,

    IR_CMD_ARE_EQUAL    ,
    IR_CMD_MORE         ,
    IR_CMD_LESS         ,
    IR_CMD_MORE_EQUAL   ,
    IR_CMD_LESS_EQUAL   ,
    IR_CMD_NOT_EQUAL    ,
    IR_CMD_NOT          ,

    IR_CMD_LOG_OR       ,
    IR_CMD_LOG_AND      ,

    IR_CMD_JMP          ,
    IR_CMD_JG           ,
    IR_CMD_JL           ,
    IR_CMD_JE           ,
    IR_CMD_JGE          ,
    IR_CMD_JLE          ,
    IR_CMD_JNE          ,

    IR_CMD_PUSH         ,
    IR_CMD_POP          ,

    IR_CMD_CALL         ,
    IR_CMD_RET          ,

    IR_CMD_IN           ,
    IR_CMD_OUT          ,
};

//--------------------------------------------------------------------------------------------------------------------------------

struct IR_node
{
    IR_CMD type;

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

bool     IR_node_ctor  (IR_node *const  node, const IR_CMD type, const bool is_reg, const bool is_mem, const bool is_imm, va_list ap);
bool     IR_node_ctor  (IR_node *const  node, const IR_CMD type, const bool is_reg, const bool is_mem, const bool is_imm, ...);
IR_node *IR_node_new   (                      const IR_CMD type, const bool is_reg, const bool is_mem, const bool is_imm, ...);
void     IR_node_delete(void    *const _node);

//--------------------------------------------------------------------------------------------------------------------------------
// set
//--------------------------------------------------------------------------------------------------------------------------------

bool IR_node_set_imm_val(IR_node *const node, const int imm_val);
bool IR_node_set_reg_num(IR_node *const node, const int reg_num);

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void IR_node_dump(const void *const _node);

#endif //IR_H
