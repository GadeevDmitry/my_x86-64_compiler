#ifndef TOKENIZER_STATIC_H
#define TOKENIZER_STATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "../../lib/include/log.h"
#include "../../lib/include/algorithm.h"
#include "../../lib/include/stack.h"
#include "../../lib/include/vector.h"

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
    {"TRABLE"            , "long"    , 0},

    {"CHAMPIONS_LEAGUE"  , "if"      , 0},
    {"PREMIER_LEAGUE"    , "else"    , 0},
    {"GUARDIOLA"         , "while"   , 0},

    {"MANCHESTER_IS_BLUE", "return"  , 0},

    {"DE_BRUYNE"         , "input"   , 0},
    {"RODRI"             , "output"  , 0},
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
    LOG_ASSERT(elem != nullptr);

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
