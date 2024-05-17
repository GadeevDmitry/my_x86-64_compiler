#include "tokenizer_static.h"

//================================================================================================================================
// TOKENIZER
//================================================================================================================================

#define $src_beg    (source->beg)
#define $src_pos    (source->pos)
#define $src_size   (source->size)

#define $type       (tkn->type)
#define $size       (tkn->size)
#define $line       (tkn->line)

#define $name       (tkn->value.name)
#define $imm_int    (tkn->value.imm_int)
#define $key        (tkn->value.key)
#define $op         (tkn->value.op)

//--------------------------------------------------------------------------------------------------------------------------------

vector *tokenizer(buffer *const source)
{
    BUFFER_VERIFY(source, nullptr);

    vector    *token_arr = vector_new(sizeof(token), nullptr, nullptr, token_dump);
    LOG_VERIFY(token_arr != nullptr, nullptr);

    size_t source_line = 1;
    token_name_fill_all();

    for (buffer_skip_spaces(source, &source_line); !buffer_is_end(source); buffer_skip_spaces(source, &source_line))
    {
        token  cur_token = {};
        size_t cur_token_size = buffer_get_token_size(source);

        if      (*$src_pos == '#') { buffer_skip_line(source, &source_line); continue; } // пропуск комментария

        if      (try_tokenize_type_key     (source, &cur_token.value.key    )) token_init_common(&cur_token, TOKEN_KEY     , cur_token_size, source_line);
        else if (try_tokenize_type_int     (source, &cur_token.value.imm_int)) token_init_common(&cur_token, TOKEN_INT     , cur_token_size, source_line);
        else if (try_tokenize_type_operator(source, &cur_token.value.op     )) token_init_common(&cur_token, TOKEN_OPERATOR, cur_token_size, source_line);
        else    {    tokenize_type_name    (source, &cur_token.value.name   ); token_init_common(&cur_token, TOKEN_NAME    , cur_token_size, source_line); }

        vector_push_back(token_arr, &cur_token);
    }

    tokenize_last(token_arr, source_line);

    return token_arr;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool try_tokenize_type_operator(buffer *const source, OPERATOR_TYPE *const op)
{
    BUFFER_VERIFY(source       , false);
    LOG_VERIFY   (op != nullptr, false);

    size_t tkn_len = buffer_get_token_size(source);

    for (size_t i = 0; i * sizeof(token_name) < sizeof(OPERATOR_NAMES); ++i)
        if (tkn_len == OPERATOR_NAMES[i].len && strncmp($src_pos, OPERATOR_NAMES[i].name, tkn_len) == 0)
        {
            *op = (OPERATOR_TYPE) i;
            $src_pos += tkn_len;

            return true;
        }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool try_tokenize_type_key(buffer *const source, KEY_TYPE *const key)
{
    BUFFER_VERIFY(source        , false);
    LOG_VERIFY   (key != nullptr, false);

    size_t tkn_len = buffer_get_token_size(source);

    for (size_t i = 0; i * sizeof(token_name) < sizeof(KEY_NAMES); ++i)
        if (tkn_len == KEY_NAMES[i].len && strncmp($src_pos, KEY_NAMES[i].name, tkn_len) == 0)
        {
            *key = (KEY_TYPE) i;
            $src_pos += tkn_len;

            return true;
        }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool try_tokenize_type_int(buffer *const source, int *const imm)
{
    BUFFER_VERIFY(source, false);
    LOG_VERIFY   (imm != nullptr, false);

    size_t tkn_len = buffer_get_token_size(source);

    char *imm_end = nullptr;
    *imm = (int) strtol($src_pos, &imm_end, 10);

    if (imm_end == $src_pos + tkn_len)
    {
        $src_pos = imm_end;
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool tokenize_type_name(buffer *const source, const char **name)
{
    BUFFER_VERIFY(source         , false);
    LOG_VERIFY   (name != nullptr, false);

    *name = $src_pos;
    buffer_skip_token(source);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool tokenize_last(vector *const tkn_arr, const size_t source_line)
{
    LOG_VERIFY(tkn_arr != nullptr, false);

    token last = {};
    token_init_common(&last, TOKEN_FICTIONAL, 0, source_line);

    vector_push_back(tkn_arr, &last);

    return true;
}

//================================================================================================================================
// TOKEN
//================================================================================================================================

#undef $src_beg
#undef $src_pos
#undef $src_size

//--------------------------------------------------------------------------------------------------------------------------------
// create
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_init_common(token *const tkn, const TOKEN_TYPE type, const size_t size, const size_t line)
{
    LOG_VERIFY(tkn != nullptr, false);

    $type = type;
    $size = size;
    $line = line;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void token_dump(const void *const _tkn)
{
    const token *const tkn = (const token *) _tkn;

    if (!token_header_dump(tkn)) return;

    USUAL_FIELD_DUMP("type    ", "%d", $type);
    USUAL_FIELD_DUMP("size    ", "%d", $size);
    USUAL_FIELD_DUMP("line    ", "%d", $line);

    switch ($type)
    {
        case TOKEN_NAME     : LOG_TAB_DEFAULT_MESSAGE("name     = %.*s", "\n", $size, $name);            break;
        case TOKEN_INT      : USUAL_FIELD_DUMP       ("imm_int ", "%d", $imm_int);                       break;
        case TOKEN_KEY      : USUAL_FIELD_DUMP       ("key     ", "%s", KEY_NAMES     [$key].name_dump); break;
        case TOKEN_OPERATOR : USUAL_FIELD_DUMP       ("op      ", "%s", OPERATOR_NAMES[$op ].name_dump); break;
        case TOKEN_FICTIONAL: LOG_TAB_DEFAULT_MESSAGE("no value", "\n");                                 break;

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
