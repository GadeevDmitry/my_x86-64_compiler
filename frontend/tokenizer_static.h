#ifndef TOKENIZER_STATIC_H
#define TOKENIZER_STATIC_H

#include "tokenizer.h"
#include "common.h"

//================================================================================================================================
// TOKEN_NAME
//================================================================================================================================

struct token_name
{
    const char *name;
    size_t       len;
};

//--------------------------------------------------------------------------------------------------------------------------------
// GLOBAL
//--------------------------------------------------------------------------------------------------------------------------------

static token_name KEY_TYPE_NAMES[] =
{
    {"BARCELONA"        , 0},

    {"MESSI"            , 0},
    {"SUAREZ"           , 0},
    {"NEYMAR"           , 0},

    {"CHAMPIONS_LEAGUE" , 0},

    {"CHECK_BEGIN"      , 0},
    {"CHECK_OVER"       , 0},
};

//--------------------------------------------------------------------------------------------------------------------------------

static token_name OPERATOR_NAMES[] =
{
    {"&&", 0},
    {"||", 0},

    {"==", 0},
    {"!=", 0},
    {"!" , 0},

    {">=", 0},
    {"<=", 0},
    {">" , 0},
    {"<" , 0},

    {"+" , 0},
    {"-" , 0},
    {"*" , 0},
    {"/" , 0},
    {"^" , 0},

    {"=" , 0},

    {";" , 0},
    {"," , 0},

    {"{" , 0},
    {"}" , 0},
    {"(" , 0},
    {")" , 0},
};

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION
//--------------------------------------------------------------------------------------------------------------------------------

static inline void token_name_fill_all();
static inline void token_name_fill_all()
{
    token_name_fill_arr(KEY_TYPE_NAMES, sizeof(KEY_TYPE_NAMES));
    token_name_fill_arr(OPERATOR_NAMES, sizeof(OPERATOR_NAMES));
}

static void token_name_fill_arr(token_name elem[], size_t elem_size);
static void token_name_fill_arr(token_name elem[], size_t elem_size)
{
    log_assert(elem != nullptr);

    for (size_t index = 0; index * sizeof(token_name) < elem_size; ++index)
    {
        elem[index].len = strlen(elem[index].name);
    }
}

//================================================================================================================================
// TOKEN_NAME
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// create
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_create_type_name    (token *const tkn, const char *const name, const size_t line);
static inline bool token_create_type_int     (token *const tkn, const int          imm, const size_t line);
static inline bool token_create_type_key     (token *const tkn, const KEY_TYPE     key, const size_t line);
static inline bool token_create_type_operator(token *const tkn, const OPERATOR_TYPE op, const size_t line);

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_header_dump   (const token *const tkn);

#endif //TOKENIZER_STATIC_H
