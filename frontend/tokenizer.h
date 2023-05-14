#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdlib.h>

//================================================================================================================================
// TOKEN
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// GLOBAL
//--------------------------------------------------------------------------------------------------------------------------------

enum KEY_TYPE
{
    KEY_WORD_UNDEF = -1 ,

    KEY_WORD_INT        ,

    KEY_WORD_IF         ,
    KEY_WORD_ELSE       ,
    KEY_WORD_WHILE      ,

    KEY_WORD_RETURN     ,

    KEY_WORD_INPUT      ,
    KEY_WORD_OUTPUT     ,
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
