#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "lib/include/log.h"
#include "lib/include/stack.h"
#include "lib/include/vector.h"

#include "tokenizer.h"

//================================================================================================================================

struct token_name
{
    const char *name;
    const char *name_dump;
    size_t       len;
};

static inline bool token_init_common(      token *const tkn, const TOKEN_TYPE type, const size_t size, const size_t line);
static inline bool token_header_dump(const token *const tkn);

//--------------------------------------------------------------------------------------------------------------------------------

struct src_line_tracking_t
{
    stack new_line_ptrs;
    size_t cur_src_line;
};

static void src_line_tracking_ctor(src_line_tracking_t *self, const char *str, const size_t str_size);
static void src_line_tracking_dtor(void                *self);

static void src_line_tracking_upd (src_line_tracking_t *self, const char *cur_ptr);

//================================================================================================================================

static token_name KEY_NAMES[] =
{
    {"TREBLE"            , "long"    , 0},

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

static void token_name_fill_arr(token_name elem[], size_t elem_size)
{
    LOG_ASSERT(elem != nullptr);

    for (size_t index = 0; index * sizeof(token_name) < elem_size; ++index)
    {
        elem[index].len = strlen(elem[index].name);
    }
}

static inline void token_name_fill_all()
{
    token_name_fill_arr(KEY_NAMES     , sizeof(KEY_NAMES     ));
    token_name_fill_arr(OPERATOR_NAMES, sizeof(OPERATOR_NAMES));
}

//================================================================================================================================

static const char *SPLIT_CHARS    = "\x20\f\n\r\t\v"; // isspace characters
static const char *NEW_LINE_CHARS = "\f\n\v";

//--------------------------------------------------------------------------------------------------------------------------------

static void skip_comment              (const char *token_ptr, size_t token_len, src_line_tracking_t *src_line_tracking);

static bool try_tokenize_type_operator(const char *token_ptr, size_t token_len, OPERATOR_TYPE *const op);
static bool try_tokenize_type_key     (const char *token_ptr, size_t token_len, KEY_TYPE      *const key);
static bool try_tokenize_type_int     (const char *token_ptr, size_t token_len, int           *const imm);

//--------------------------------------------------------------------------------------------------------------------------------

vector *tokenizer(buffer *const source)
{
    BUFFER_VERIFY(source, nullptr);

    vector *token_arr = vector_new(sizeof(token), nullptr, nullptr, token_dump);
    LOG_VERIFY(token_arr != nullptr, nullptr);

    token_name_fill_all();

    src_line_tracking_t src_line_tracking = {};
    src_line_tracking_ctor(&src_line_tracking, source->beg, source->size);

    for (const char *token_ptr = strtok(source->beg, SPLIT_CHARS); token_ptr != nullptr; token_ptr = strtok(NULL, SPLIT_CHARS))
    {
        src_line_tracking_upd(&src_line_tracking, token_ptr);

        token  cur_token = {};
        size_t token_len = strlen(token_ptr);

        if (*token_ptr == '#')
        {
            skip_comment(token_ptr, token_len, &src_line_tracking);
            continue;
        }

        if (try_tokenize_type_operator(token_ptr, token_len, &cur_token.value.op))
            token_init_common(&cur_token, TOKEN_OPERATOR, token_len, src_line_tracking.cur_src_line);

        else if (try_tokenize_type_key(token_ptr, token_len, &cur_token.value.key))
            token_init_common(&cur_token, TOKEN_KEY, token_len, src_line_tracking.cur_src_line);

        else if (try_tokenize_type_int(token_ptr, token_len, &cur_token.value.imm_int))
            token_init_common(&cur_token, TOKEN_INT, token_len, src_line_tracking.cur_src_line);

        else
        {
            cur_token.value.name = token_ptr;
            token_init_common(&cur_token, TOKEN_NAME, token_len, src_line_tracking.cur_src_line);
        }

        vector_push_back(token_arr, &cur_token);
    }

    src_line_tracking_upd(&src_line_tracking, source->beg + source->size);

    token last = {};
    token_init_common(&last, TOKEN_FICTIONAL, 0, src_line_tracking.cur_src_line);
    vector_push_back(token_arr, &last);

    src_line_tracking_dtor(&src_line_tracking);

    return token_arr;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void skip_comment(const char *token_ptr, size_t token_len, src_line_tracking_t *src_line_tracking)
{
    LOG_ASSERT(token_ptr         != nullptr);
    LOG_ASSERT(src_line_tracking != nullptr);
    LOG_ASSERT(!stack_is_empty(&src_line_tracking->new_line_ptrs)); // должен быть как минимум указатель на последний нулевой символ

    const char *new_line_ptr = nullptr;
    stack_front(&src_line_tracking->new_line_ptrs, &new_line_ptr);

    if (new_line_ptr == token_ptr + token_len)
        return; // следующий токен на новой строке не является частью комментария

    ((char *) token_ptr)[token_len] = '#';                    // гарантируем, что (token_ptr + token_len) станет следующим токеном
    strtok((char *) (token_ptr + token_len), NEW_LINE_CHARS); // пропускаем  все вплоть до перевода строки
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool try_tokenize_type_operator(const char *token_ptr, size_t token_len, OPERATOR_TYPE *const op)
{
    LOG_ASSERT(token_ptr != nullptr);
    LOG_ASSERT(op        != nullptr);

    for (size_t i = 0; i * sizeof(*OPERATOR_NAMES) < sizeof(OPERATOR_NAMES); ++i)
        if (token_len == OPERATOR_NAMES[i].len && strncmp(token_ptr, OPERATOR_NAMES[i].name, token_len) == 0)
        {
            *op = (OPERATOR_TYPE) i;
            return true;
        }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool try_tokenize_type_key(const char *token_ptr, size_t token_len, KEY_TYPE *const key)
{
    LOG_ASSERT(token_ptr != nullptr);
    LOG_ASSERT(key       != nullptr);

    for (size_t i = 0; i * sizeof(*KEY_NAMES) < sizeof(KEY_NAMES); ++i)
        if (token_len == KEY_NAMES[i].len && strncmp(token_ptr, KEY_NAMES[i].name, token_len) == 0)
        {
            *key = (KEY_TYPE) i;
            return true;
        }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool try_tokenize_type_int(const char *token_ptr, size_t token_len, int *const imm)
{
    LOG_ASSERT(token_ptr != nullptr);
    LOG_ASSERT(imm       != nullptr);

    char *imm_end = nullptr;
    *imm = (int) strtol(token_ptr, &imm_end, 10);

    return imm_end == token_ptr + token_len;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void src_line_tracking_ctor(src_line_tracking_t *self, const char *str, const size_t str_size)
{
    LOG_ASSERT(self != nullptr);
    LOG_ASSERT(str  != nullptr);

    self->cur_src_line = 1;
    stack_ctor(&self->new_line_ptrs, sizeof(char *));

    const char *str_ptr = str + str_size - 1;
    LOG_ASSERT(*str_ptr == '\0');

    stack_push(&self->new_line_ptrs, &str_ptr);
    for (; str_ptr != str; --str_ptr)
    {
        if ((*str_ptr == '\n') || // line feed
            (*str_ptr == '\f') || // form feed
            (*str_ptr == '\v'))   // vertical tab
        {
            stack_push(&self->new_line_ptrs, &str_ptr);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static void src_line_tracking_dtor(void *_self)
{
    LOG_ASSERT(_self != nullptr);

    src_line_tracking_t *self = (src_line_tracking_t *) _self;
    stack_dtor(&self->new_line_ptrs);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void src_line_tracking_upd(src_line_tracking_t *self, const char *cur_ptr)
{
    LOG_ASSERT(self    != nullptr);
    LOG_ASSERT(cur_ptr != nullptr);

    const char *new_line_ptr = nullptr;
    while (!stack_is_empty(&self->new_line_ptrs))
    {
        stack_front(&self->new_line_ptrs, &new_line_ptr);

        if (new_line_ptr > cur_ptr)
            break;

        ++self->cur_src_line;
        stack_pop(&self->new_line_ptrs);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_init_common(token *const tkn, const TOKEN_TYPE type, const size_t size, const size_t line)
{
    LOG_VERIFY(tkn != nullptr, false);

    tkn->type = type;
    tkn->size = size;
    tkn->line = line;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void token_dump(const void *const _tkn)
{
    const token *const tkn = (const token *) _tkn;

    if (!token_header_dump(tkn)) return;

    USUAL_FIELD_DUMP("type    ", "%d", tkn->type);
    USUAL_FIELD_DUMP("size    ", "%d", tkn->size);
    USUAL_FIELD_DUMP("line    ", "%d", tkn->line);

    switch (tkn->type)
    {
        case TOKEN_NAME     : LOG_TAB_DEFAULT_MESSAGE("name     = %.*s", "\n", tkn->size, tkn->value.name);        break;
        case TOKEN_INT      : USUAL_FIELD_DUMP       ("imm_int ", "%d",                   tkn->value.imm_int);     break;

        case TOKEN_KEY      : USUAL_FIELD_DUMP       ("key     ", "%s", KEY_NAMES     [tkn->value.key].name_dump); break;
        case TOKEN_OPERATOR : USUAL_FIELD_DUMP       ("op      ", "%s", OPERATOR_NAMES[tkn->value.op ].name_dump); break;
        case TOKEN_FICTIONAL: LOG_TAB_DEFAULT_MESSAGE("no value", "\n");                                           break;

        default             : LOG_ASSERT(false); break;
    }

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("}", "\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_header_dump(const token *const tkn)
{
    LOG_TAB_SERVICE_MESSAGE("token (addr: %p)\n"
                            "{", "\n",   tkn);

    if (tkn == nullptr) { LOG_TAB_SERVICE_MESSAGE("}", "\n"); return false; }
    LOG_TAB++;

    return true;
}
