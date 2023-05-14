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

//--------------------------------------------------------------------------------------------------------------------------------
// GLOBAL
//--------------------------------------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------------------------------------

enum TOKEN_TYPE
{
    TOKEN_NAME      ,
    TOKEN_INT       ,
    TOKEN_KEY       ,
    TOKEN_OPERATOR  ,
};

//--------------------------------------------------------------------------------------------------------------------------------
// STRUCT
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

void token_dump(const void *const _tkn);

#endif //TOKENIZER_H
