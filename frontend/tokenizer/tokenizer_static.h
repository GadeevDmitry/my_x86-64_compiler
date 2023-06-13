#ifndef TOKENIZER_STATIC_H
#define TOKENIZER_STATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LOG_NTRACE
#define LOG_NLEAK
#define LOG_NVERIFY
#define LOG_NDEBUG

#include "../../lib/logs/log.h"
#include "../../lib/algorithm/algorithm.h"
#include "../../lib/stack/stack.h"
#include "../../lib/vector/vector.h"

#include "tokenizer.h"

//================================================================================================================================
// TOKENIZER
//================================================================================================================================

static bool try_tokenize_type_key     (buffer *const source, KEY_TYPE      *const key);
static bool try_tokenize_type_int     (buffer *const source, int           *const imm);
static bool try_tokenize_type_operator(buffer *const source, OPERATOR_TYPE *const op );
static bool     tokenize_type_name    (buffer *const source, const char    **    name);

static bool tokenize_last(vector *const tkn_arr, const size_t source_line);

//================================================================================================================================
// TOKEN_NAME
//================================================================================================================================

struct token_name
{
    const char *name;
    const char *name_dump;
    size_t       len;
};

//--------------------------------------------------------------------------------------------------------------------------------
// GLOBAL
//--------------------------------------------------------------------------------------------------------------------------------

static token_name KEY_NAMES[] =
{
    {"int"   , "int"     , 0},

    {"if"    , "if"      , 0},
    {"else"  , "else"    , 0},
    {"while" , "while"   , 0},

    {"return", "return"  , 0},

    {"input" , "input"   , 0},
    {"output", "output"  , 0},
};

//--------------------------------------------------------------------------------------------------------------------------------

static token_name OPERATOR_NAMES[] =
{
    {"&&"   , "&&", 0},
    {"||"   , "||", 0},

    {"=="   , "==", 0},
    {"!="   , "!=", 0},
    {"!"    , "!" , 0},

    {">="   , ">=", 0},
    {"<="   , "<=", 0},
    {">"    , ">" , 0},
    {"<"    , "<" , 0},

    {"+"    , "+" , 0},
    {"-"    , "-" , 0},
    {"*"    , "*" , 0},
    {"/"    , "/" , 0},
    {"^"    , "^" , 0},

    {"="    , "=" , 0},

    {";"    , ";" , 0},
    {","    , "," , 0},

    {"{"    , "{" , 0},
    {"}"    , "}" , 0},
    {"("    , "(" , 0},
    {")"    , ")" , 0},
};

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION
//--------------------------------------------------------------------------------------------------------------------------------

static inline void token_name_fill_all();
static        void token_name_fill_arr(token_name elem[], size_t elem_size);

static inline void token_name_fill_all()
{
    token_name_fill_arr(KEY_NAMES     , sizeof(KEY_NAMES     ));
    token_name_fill_arr(OPERATOR_NAMES, sizeof(OPERATOR_NAMES));
}

static void token_name_fill_arr(token_name elem[], size_t elem_size)
{
    log_assert(elem != nullptr);

    for (size_t index = 0; index * sizeof(token_name) < elem_size; ++index)
    {
        elem[index].len = strlen(elem[index].name);
    }
}

//================================================================================================================================
// TOKEN
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_init_common(      token *const tkn, const TOKEN_TYPE type, const size_t size, const size_t line);
static inline bool token_header_dump(const token *const tkn);

#endif //TOKENIZER_STATIC_H
