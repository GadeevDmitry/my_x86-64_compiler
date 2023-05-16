#ifndef TOKENIZER_STATIC_H
#define TOKENIZER_STATIC_H

#include "common.h"
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
    {"BARCELONA"        , "int"     , 0},

    {"MESSI"            , "if"      , 0},
    {"SUAREZ"           , "else"    , 0},
    {"NEYMAR"           , "while"   , 0},

    {"CHAMPIONS_LEAGUE" , "return"  , 0},

    {"CHECK_BEGIN"      , "input"   , 0},
    {"CHECK_OVER"       , "output"  , 0},
};

//--------------------------------------------------------------------------------------------------------------------------------

static token_name OPERATOR_NAMES[] =
{
    {"OFFSIDE"  , "||", 0},
    {"INSIDE"   , "&&", 0},

    {"GOAL"     , "==", 0},
    {"NO_GOAL"  , "!=", 0},
    {"NO"       , "!" , 0},

    {"R_CORNER" , ">=", 0},
    {"L_CORNER" , "<=", 0},
    {"R"        , ">" , 0},
    {"L"        , "<" , 0},

    {"PLUS"     , "+" , 0},
    {"KEEP"     , "-" , 0},
    {"IN"       , "*" , 0},
    {"OUT"      , "/" , 0},
    {"UP"       , "^" , 0},

    {"PENALTY"  , "=" , 0},

    {"TIME"     , ";" , 0},
    {"KICK"     , "," , 0},

    {"RUN"      , "{" , 0},
    {"JMP"      , "}" , 0},
    {"CUP"      , "(" , 0},
    {"WIN"      , ")" , 0},
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
