#include "tokenizer_static.h"

//================================================================================================================================
// TOKENIZER
//================================================================================================================================

vector *tokenizer(buffer *const source)
{
    
}

//================================================================================================================================
// TOKEN
//================================================================================================================================

#define $type       (tkn->type)
#define $line       (tkn->line)

#define $name       (tkn->value.name)
#define $imm_int    (tkn->value.imm_int)
#define $key        (tkn->value.key)
#define $op         (tkn->value.op)

//--------------------------------------------------------------------------------------------------------------------------------
// create
//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_create_type_name(token *const tkn, const char *const name, const size_t line)
{
    log_verify(tkn  != nullptr, false);
    log_verify(name != nullptr, false);

    $type = TOKEN_NAME;
    $name = name;
    $line = line;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_create_type_int(token *const tkn, const int imm, const size_t line)
{
    log_verify(tkn != nullptr, false);

    $type    = TOKEN_INT;
    $imm_int = imm;
    $line    = line;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_create_type_key(token *const tkn, const KEY_TYPE key, const size_t line)
{
    log_verify(tkn != nullptr, false);

    $type = TOKEN_KEY;
    $key  = key;
    $line = line;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool token_create_type_operator(token *const tkn, const OPERATOR_TYPE op, const size_t line)
{
    log_verify(tkn != nullptr, false);

    $type = TOKEN_OPERATOR;
    $op   = op;
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
    usual_field_dump("line   ", "%d", $line);

    switch ($type)
    {
        case TOKEN_NAME    : usual_field_dump("name   ", "%s", $name);
        case TOKEN_INT     : usual_field_dump("imm_int", "%d", $imm_int);
        case TOKEN_KEY     : usual_field_dump("key    ", "%s", KEY_TYPE_NAMES[$key]);
        case TOKEN_OPERATOR: usual_field_dump("op     ", "%s", OPERATOR_NAMES[$op] );

        default            : log_assert(false);
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
