#ifndef AST_H
#define AST_H

#include <stdlib.h>

//================================================================================================================================
// AST_NODE
//================================================================================================================================

enum AST_NODE_TYPE
{
    AST_NODE_FICTIONAL          ,
    AST_NODE_IMM_INT            ,
    AST_NODE_VARIABLE           ,
    AST_NODE_OPERATOR_IF        ,
    AST_NODE_OPERATOR_THEN_ELSE ,
    AST_NODE_OPERATOR_WHILE     ,
    AST_NODE_OPERATOR           ,
    AST_NODE_DECL_VAR           ,
    AST_NODE_DECL_FUNC          ,
    AST_NODE_CALL_FUNC          ,
    AST_NODE_OPERATOR_RETURN    ,
};

enum AST_OPERATOR_TYPE
{
    AST_OPERATOR_ADD = 1    ,
    AST_OPERATOR_SUB        ,
    AST_OPERATOR_MUL        ,
    AST_OPERATOR_DIV        ,
    AST_OPERATOR_POW        ,

    AST_OPERATOR_INPUT      ,
    AST_OPERATOR_OUTPUT     ,

    AST_OPERATOR_ARE_EQUAL  ,
    AST_OPERATOR_MORE       ,
    AST_OPERATOR_LESS       ,
    AST_OPERATOR_MORE_EQUAL ,
    AST_OPERATOR_LESS_EQUAL ,
    AST_OPERATOR_NOT_EQUAL  ,
    AST_OPERATOR_NOT        ,

    AST_OPERATOR_LOG_OR     ,
    AST_OPERATOR_LOG_AND    ,

    AST_OPERATOR_ASSIGNMENT ,
};

enum AST_CHILD
{
    AST_CHILD_LEFT ,
    AST_CHILD_RIGHT,
};

//--------------------------------------------------------------------------------------------------------------------------------

struct AST_node
{
    AST_NODE_TYPE type;

    AST_node *left;
    AST_node *right;
    AST_node *prev;

    union
    {
        int           imm_int;
        size_t        var_ind;
        size_t       func_ind;
        AST_OPERATOR_TYPE  op;
    }
    value;
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

bool      ast_node_ctor(AST_node *const node, AST_node *const left, AST_node *const right, AST_node *const prev, AST_NODE_TYPE type, ...);
AST_node *ast_node_new (                      AST_node *const left, AST_node *const right, AST_node *const prev, AST_NODE_TYPE type, ...);

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

void AST_tree_delete(AST_node *const node);

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void AST_tree_graphviz_dump(const void *const _node);

#endif //AST_H
