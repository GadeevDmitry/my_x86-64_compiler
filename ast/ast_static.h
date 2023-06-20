#ifndef AST_STATIC_H
#define AST_STATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "../lib/logs/log.h"
#include "../lib/algorithm/algorithm.h"
#include "../lib/graphviz_const/graphviz_const.h"

#include "ast.h"

//================================================================================================================================
// AST_NODE
//================================================================================================================================

static const char *AST_NODE_TYPE_NAMES[] =
{
    "FICTIONAL" ,
    "IMM_INT"   ,
    "VARIABLE"  ,
    "IF"        ,
    "THEN_ELSE" ,
    "WHILE"     ,
    "OPERATOR"  ,
    "VAR_DECL"  ,
    "FUNC_DECL" ,
    "FUNC_CAll" ,
    "RETURN"    ,
};

static const char *AST_OPERATOR_NAMES[] =
{
    "UNDEF" ,
    "ADD"   ,
    "SUB"   ,
    "MUL"   ,
    "DIV"   ,
    "SQRT"  ,

    "IN"    ,
    "OUT"   ,

    "=="    ,
    ">"     ,
    "<"     ,
    ">="    ,
    "<="    ,
    "!="    ,
    "!"     ,

    "||"    ,
    "&&"    ,

    ":="    ,
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

static        bool AST_node_set_value(AST_node *const node, const AST_NODE_TYPE type, va_list value);
static inline bool AST_node_ctor     (AST_node *const node, const AST_NODE_TYPE type, va_list value);

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static        void AST_tree_static_dump (const AST_node *const node);

static inline void AST_tree_header_dump (                                                     FILE *const stream);
static      size_t AST_tree_content_dump(const AST_node *const node, const size_t node_index, FILE *const stream);
static        void AST_node_content_dump(const AST_node *const node, const size_t node_index, FILE *const stream);
static        void AST_tree_ending_dump (const char       *txt_file, const char    *png_file, FILE *const stream);

static        void AST_node_get_dump_color(const AST_node *const node, GRAPHVIZ_COLOR *const color, GRAPHVIZ_COLOR *const color_fill);
static        void AST_node_get_dump_value(const AST_node *const node, char *const value_dump);

static inline void AST_edge_dump(const size_t src, const size_t dst, FILE *const stream);

#endif //AST_STATIC_H
