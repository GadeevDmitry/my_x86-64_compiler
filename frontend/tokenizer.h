#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdlib.h>
#include "../lib/vector/vector.h"

//================================================================================================================================
// TOKENIZER
//================================================================================================================================

vector *tokenizer(buffer *const source);

//================================================================================================================================
// TOKEN
//================================================================================================================================

enum KEY_TYPE
{
    KEY_UNDEF = -1  ,

    KEY_INT         ,

    KEY_IF          ,
    KEY_ELSE        ,
    KEY_WHILE       ,

    KEY_RETURN      ,

    KEY_INPUT       ,
    KEY_OUTPUT      ,
};

//--------------------------------------------------------------------------------------------------------------------------------

enum OPERATOR_TYPE
{
    OPERATOR_UNDEF   = -1   ,
    OPERATOR_LOG_AND        ,
    OPERATOR_LOG_OR         ,

    OPERATOR_ARE_EQUAL      ,
    OPERATOR_NOT_EQUAL      ,
    OPERATOR_NOT            ,

    OPERATOR_MORE_EQUAL     ,
    OPERATOR_LESS_EQUAL     ,
    OPERATOR_MORE           ,
    OPERATOR_LESS           ,

    OPERATOR_ADD            ,
    OPERATOR_SUB            ,
    OPERATOR_MUL            ,
    OPERATOR_DIV            ,
    OPERATOR_POW            ,

    OPERATOR_ASSIGNMENT     ,

    OPERATOR_COMMA_POINT    ,
    OPERATOR_COMMA          ,

    OPERATOR_L_SCOPE_FIGURE ,
    OPERATOR_R_SCOPE_FIGURE ,
    OPERATOR_L_SCOPE_CIRCLE ,
    OPERATOR_R_SCOPE_CIRCLE ,
};

enum TOKEN_TYPE
{
    TOKEN_NAME      ,
    TOKEN_INT       ,
    TOKEN_KEY       ,
    TOKEN_OPERATOR  ,

    TOKEN_FICTIONAL ,
};

//--------------------------------------------------------------------------------------------------------------------------------

struct token
{
    TOKEN_TYPE type;
    size_t     size;
    size_t     line;

    union
    {
        const char *name;
        int      imm_int;
        KEY_TYPE     key;
        OPERATOR_TYPE op;
    }
    value;
};

//--------------------------------------------------------------------------------------------------------------------------------
// token define
//--------------------------------------------------------------------------------------------------------------------------------

#define $type       (tkn->type)
#define $size       (tkn->size)
#define $line       (tkn->line)

#define $name       (tkn->value.name)
#define $imm_int    (tkn->value.imm_int)
#define $key        (tkn->value.key)
#define $op         (tkn->value.op)

//--------------------------------------------------------------------------------------------------------------------------------

#define     token_type_is_smth(token_type, TOKEN_TYPE_name)                                                                     \
inline bool token_type_is_ ##  token_type(const token *const tkn);                                                              \
inline bool token_type_is_ ##  token_type(const token *const tkn) { return $type == TOKEN_TYPE_name; }

token_type_is_smth(name, TOKEN_NAME    )
token_type_is_smth(int , TOKEN_INT     )
token_type_is_smth(key , TOKEN_KEY     )
token_type_is_smth(op  , TOKEN_OPERATOR)

#undef token_type_is_smth

//--------------------------------------------------------------------------------------------------------------------------------

#define     token_op_is_smth(op_type, OP_TYPE_name)                                                                             \
inline bool token_op_is_ ##  op_type(const token *const tkn);                                                                   \
inline bool token_op_is_ ##  op_type(const token *const tkn) { return token_type_is_op(tkn) && $op == OP_TYPE_name; }

token_op_is_smth(log_and        , OPERATOR_LOG_AND       )
token_op_is_smth(log_or         , OPERATOR_LOG_OR        )

token_op_is_smth(are_equal      , OPERATOR_ARE_EQUAL     )
token_op_is_smth(not_equal      , OPERATOR_NOT_EQUAL     )
token_op_is_smth(not            , OPERATOR_NOT           )

token_op_is_smth(more_equal     , OPERATOR_MORE_EQUAL    )
token_op_is_smth(less_equal     , OPERATOR_LESS_EQUAL    )
token_op_is_smth(more           , OPERATOR_MORE          )
token_op_is_smth(less           , OPERATOR_LESS          )

token_op_is_smth(add            , OPERATOR_ADD           )
token_op_is_smth(sub            , OPERATOR_SUB           )
token_op_is_smth(mul            , OPERATOR_MUL           )
token_op_is_smth(div            , OPERATOR_DIV           )
token_op_is_smth(pow            , OPERATOR_POW           )

token_op_is_smth(assignment     , OPERATOR_ASSIGNMENT    )

token_op_is_smth(comma_point    , OPERATOR_COMMA_POINT   )
token_op_is_smth(comma          , OPERATOR_COMMA         )

token_op_is_smth(l_scope_figure , OPERATOR_L_SCOPE_FIGURE)
token_op_is_smth(r_scope_figure , OPERATOR_R_SCOPE_FIGURE)
token_op_is_smth(l_scope_circle , OPERATOR_L_SCOPE_CIRCLE)
token_op_is_smth(r_scope_circle , OPERATOR_R_SCOPE_CIRCLE)

#undef token_op_is_smth

//--------------------------------------------------------------------------------------------------------------------------------

#define     token_key_is_smth(token_key, KEY_TYPE_name)                                                                         \
inline bool token_key_is_  ## token_key(const token *const tkn);                                                                \
inline bool token_key_is_  ## token_key(const token *const tkn) { return token_type_is_key(tkn) && $key == KEY_TYPE_name; }

token_key_is_smth(int   , KEY_INT   )

token_key_is_smth(if    , KEY_IF    )
token_key_is_smth(else  , KEY_ELSE  )
token_key_is_smth(while , KEY_WHILE )

token_key_is_smth(return, KEY_RETURN)

token_key_is_smth(input , KEY_INPUT )
token_key_is_smth(output, KEY_OUTPUT)

#undef token_key_is_smth

//--------------------------------------------------------------------------------------------------------------------------------

#undef $type
#undef $size
#undef $line

#undef $name
#undef $imm_int
#undef $key
#undef $op

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void token_dump(const void *const _tkn);

#endif //TOKENIZER_H
