#include "tokenizer_static.h"

//================================================================================================================================
// TOKENIZER
//================================================================================================================================

#define $src_beg    (source->buff_beg)
#define $src_pos    (source->buff_pos)
#define $src_size   (source->buff_size)

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
    buf_verify(source, nullptr);

    vector    *token_arr = vector_new(sizeof(token), nullptr, nullptr, token_dump);
    log_verify(token_arr != nullptr, nullptr);

    size_t source_line = 1;
    token_name_fill_all();

    for (buffer_skip_spaces(source, &source_line); !buffer_is_end(source); buffer_skip_spaces(source, &source_line))
    {
        token  cur_token = {};
        size_t cur_token_size = buffer_get_token_size(source);

        if      (*$src_pos == '#') buffer_skip_line(source);    // пропуск комментария

        else if (try_tokenize_type_key     (source, &cur_token.value.key    )) token_init_common(&cur_token, TOKEN_KEY     , cur_token_size, source_line);
        else if (try_tokenize_type_int     (source, &cur_token.value.imm_int)) token_init_common(&cur_token, TOKEN_INT     , cur_token_size, source_line);
        else if (try_tokenize_type_operator(source, &cur_token.value.op     )) token_init_common(&cur_token, TOKEN_OPERATOR, cur_token_size, source_line);
        else    {    tokenize_type_name    (source, &cur_token.value.name   ); token_init_common(&cur_token, TOKEN_NAME    , cur_token_size, source_line); }

        vector_push_back(token_arr, &cur_token);
    }

    return token_arr;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool try_tokenize_type_operator(buffer *const source, OPERATOR_TYPE *const op)
{
    buf_verify(source       , false);
    log_verify(op != nullptr, false);

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
    buf_verify(source        , false);
    log_verify(key != nullptr, false);

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
    buf_verify(source, false);
    log_verify(imm != nullptr, false);

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
    buf_verify(source         , false);
    log_verify(name != nullptr, false);

    *name = $src_pos;
    buffer_skip_token(source);

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
    log_verify(tkn != nullptr, false);

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

    usual_field_dump("type   ", "%d", $type);
    usual_field_dump("size   ", "%d", $size);
    usual_field_dump("line   ", "%d", $line);

    switch ($type)
    {
        case TOKEN_NAME    : log_tab_default_message("name    = %.*s", "\n", $size, $name);            break;
        case TOKEN_INT     : usual_field_dump       ("imm_int", "%d", $imm_int);                       break;
        case TOKEN_KEY     : usual_field_dump       ("key    ", "%s", KEY_NAMES     [$key].name_dump); break;
        case TOKEN_OPERATOR: usual_field_dump       ("op     ", "%s", OPERATOR_NAMES[$op ].name_dump); break;

        default            : log_assert(false); break;
    }

    LOG_TAB--;
    log_tab_service_message("}", "\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_header_dump(const token *const tkn)
{
    log_tab_service_message("token (addr: %p)\n"
                            "{", "\n",   tkn);

    if (tkn == nullptr) { log_tab_service_message("}", "\n"); return false; }
    LOG_TAB++;

    return true;
}
