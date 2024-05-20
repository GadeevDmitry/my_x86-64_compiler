#include <stdio.h>

#include "parser.hpp"
#include "lib/include/log.h"
#include "frontend/tokenizer/tokenizer.h"

//================================================================================================================================

class token_manager_t
{
// member functions
public:
    token_manager_t():
    pos_(nullptr),
    end_(nullptr)
    {}

    void init(vector *tokens)
    {
        pos_ = (token *) vector_begin(tokens);
        end_ = (token *) vector_end  (tokens) - 1; // указатель на fictional-токен
    }

    const token &get() const
    {
        return *pos_;
    }

    const token &next_before()
    {
        LOG_ASSERT(pos_ != end_);
        return *++pos_;
    }

    const token &next_after()
    {
        LOG_ASSERT(pos_ != end_);
        return *pos_++;
    }

    void next()
    {
        LOG_ASSERT(pos_ != end_);
        ++pos_;
    }

    bool is_empty() const
    {
        return pos_ == end_;
    }

// member data
private:
    const token *pos_;
    const token *end_;
}
static TOKEN_MANAGER;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct parser_error_manager_t
{
// static
public:
    static void error(const char *err_msg)
    {
        fprintf(stderr, "line %3lu: " BASH_COLOR_RED "ERROR: " BASH_COLOR_WHITE "%s\n", TOKEN_MANAGER.get().line, err_msg);
    }

    static std::unique_ptr<LLVM_AST_expression_node> expression_error(const char *err_msg)
    {
        error(err_msg);
        return nullptr;
    }

    static std::unique_ptr<LLVM_AST_value_node> value_error(const char *err_msg)
    {
        error(err_msg);
        return nullptr;
    }

    static std::unique_ptr<LLVM_AST_statement_node> statement_error(const char *err_msg)
    {
        error(err_msg);
        return nullptr;
    }
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct binary_operator_manager_t
{
// static
private:
    static constexpr const LLVM_AST_BINARY_OPERATOR_TYPE token2ast_binary_operator_[] =
    {
        [OPERATOR_LOG_AND       ] = AST_BINARY_OPERATOR_LOG_AND   ,
        [OPERATOR_LOG_OR        ] = AST_BINARY_OPERATOR_LOG_OR    ,

        [OPERATOR_ARE_EQUAL     ] = AST_BINARY_OPERATOR_ARE_EQUAL ,
        [OPERATOR_NOT_EQUAL     ] = AST_BINARY_OPERATOR_NOT_EQUAL ,
        [OPERATOR_NOT           ] = AST_BINARY_OPERATOR_UNDEF,

        [OPERATOR_MORE_EQUAL    ] = AST_BINARY_OPERATOR_MORE_EQUAL,
        [OPERATOR_LESS_EQUAL    ] = AST_BINARY_OPERATOR_LESS_EQUAL,
        [OPERATOR_MORE          ] = AST_BINARY_OPERATOR_MORE      ,
        [OPERATOR_LESS          ] = AST_BINARY_OPERATOR_LESS      ,

        [OPERATOR_ADD           ] = AST_BINARY_OPERATOR_ADD       ,
        [OPERATOR_SUB           ] = AST_BINARY_OPERATOR_SUB       ,
        [OPERATOR_MUL           ] = AST_BINARY_OPERATOR_MUL       ,
        [OPERATOR_DIV           ] = AST_BINARY_OPERATOR_DIV       ,
        [OPERATOR_POW           ] = AST_BINARY_OPERATOR_UNDEF     ,

        [OPERATOR_ASSIGNMENT    ] = AST_BINARY_OPERATOR_ASSIGNMENT,

        [OPERATOR_COMMA_POINT   ] = AST_BINARY_OPERATOR_UNDEF     ,
        [OPERATOR_COMMA         ] = AST_BINARY_OPERATOR_UNDEF     ,

        [OPERATOR_L_SCOPE_FIGURE] = AST_BINARY_OPERATOR_UNDEF     ,
        [OPERATOR_R_SCOPE_FIGURE] = AST_BINARY_OPERATOR_UNDEF     ,
        [OPERATOR_L_SCOPE_CIRCLE] = AST_BINARY_OPERATOR_UNDEF     ,
        [OPERATOR_R_SCOPE_CIRCLE] = AST_BINARY_OPERATOR_UNDEF     ,
    };

    static constexpr const unsigned ast_binary_operator_priorities_[] =
    {
        [AST_BINARY_OPERATOR_ADD       ] = 5,
        [AST_BINARY_OPERATOR_SUB       ] = 5,
        [AST_BINARY_OPERATOR_MUL       ] = 6,
        [AST_BINARY_OPERATOR_DIV       ] = 6,
        [AST_BINARY_OPERATOR_ARE_EQUAL ] = 3,
        [AST_BINARY_OPERATOR_MORE      ] = 4,
        [AST_BINARY_OPERATOR_LESS      ] = 4,
        [AST_BINARY_OPERATOR_MORE_EQUAL] = 4,
        [AST_BINARY_OPERATOR_LESS_EQUAL] = 4,
        [AST_BINARY_OPERATOR_NOT_EQUAL ] = 3,
        [AST_BINARY_OPERATOR_LOG_OR    ] = 1,
        [AST_BINARY_OPERATOR_LOG_AND   ] = 2,
        [AST_BINARY_OPERATOR_ASSIGNMENT] = 0,
    };

public:
    static LLVM_AST_BINARY_OPERATOR_TYPE token2ast_binary_operator(const token &tkn)
    {
        if (!token_type_is_op(&tkn))
            return AST_BINARY_OPERATOR_UNDEF;

        LOG_ASSERT(
            (tkn.value.op >= 0) &&
            (tkn.value.op * sizeof(*token2ast_binary_operator_)< sizeof(token2ast_binary_operator_)));

        return token2ast_binary_operator_[tkn.value.op];
    }

    static unsigned get_priority(LLVM_AST_BINARY_OPERATOR_TYPE type)
    {
        LOG_ASSERT(
            (type > AST_BINARY_OPERATOR_UNDEF) &&
            (type < AST_BINARY_OPERATOR_NUM));

        return ast_binary_operator_priorities_[type];
    }
};

//--------------------------------------------------------------------------------------------------------------------------------

struct unary_operator_manager_t
{
// static
private:
    static constexpr const LLVM_AST_UNARY_OPERATOR_TYPE token2ast_unary_operator_[] =
    {
        [OPERATOR_LOG_AND       ] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_LOG_OR        ] = AST_UNARY_OPERATOR_UNDEF,

        [OPERATOR_ARE_EQUAL     ] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_NOT_EQUAL     ] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_NOT           ] = AST_UNARY_OPERATOR_NOT  ,

        [OPERATOR_MORE_EQUAL    ] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_LESS_EQUAL    ] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_MORE          ] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_LESS          ] = AST_UNARY_OPERATOR_UNDEF,

        [OPERATOR_ADD           ] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_SUB           ] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_MUL           ] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_DIV           ] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_POW           ] = AST_UNARY_OPERATOR_SQRT ,

        [OPERATOR_ASSIGNMENT    ] = AST_UNARY_OPERATOR_UNDEF,

        [OPERATOR_COMMA_POINT   ] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_COMMA         ] = AST_UNARY_OPERATOR_UNDEF,

        [OPERATOR_L_SCOPE_FIGURE] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_R_SCOPE_FIGURE] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_L_SCOPE_CIRCLE] = AST_UNARY_OPERATOR_UNDEF,
        [OPERATOR_R_SCOPE_CIRCLE] = AST_UNARY_OPERATOR_UNDEF,
    };

public:
    static LLVM_AST_UNARY_OPERATOR_TYPE token2ast_unary_operator(const token &tkn)
    {
        if (!token_type_is_op(&tkn))
            return AST_UNARY_OPERATOR_UNDEF;

        LOG_ASSERT(
            (tkn.value.op >= 0) &&
            (tkn.value.op * sizeof(*token2ast_unary_operator_)< sizeof(token2ast_unary_operator_)));

        return token2ast_unary_operator_[tkn.value.op];
    }
};

//================================================================================================================================

static std::unique_ptr<LLVM_AST_statement_node>          parse_global_decl              ();
static std::unique_ptr<LLVM_AST_statement_node>          parse_func_args_and_body       (size_t src_line, const char *func_name);
static bool                                              parse_func_args                (LLVM_AST_statement_func_decl_node &function);
static std::unique_ptr<LLVM_AST_statement_var_decl_node> parse_func_arg                 ();

static bool                                              parse_expression_scope         (std::vector<std::unique_ptr<LLVM_AST_expression_node>> &expressions);
static std::unique_ptr<LLVM_AST_value_node>              parse_expression_value         ();
static std::unique_ptr<LLVM_AST_statement_node>          parse_expression_statement     ();

static std::unique_ptr<LLVM_AST_value_node>              parse_value                    ();
static std::unique_ptr<LLVM_AST_value_node>              parse_value_binary_operator_rhs(const unsigned min_priority, std::unique_ptr<LLVM_AST_value_node> lhs);
static std::unique_ptr<LLVM_AST_value_node>              parse_value_primary            ();
static std::unique_ptr<LLVM_AST_value_node>              parse_value_func_params        (const size_t src_line, const char *func_name);
static std::unique_ptr<LLVM_AST_value_node>              parse_value_lvalue             ();

static std::unique_ptr<LLVM_AST_statement_node>          parse_statement_var_decl       ();
static std::unique_ptr<LLVM_AST_statement_node>          parse_statement_if             ();
static std::unique_ptr<LLVM_AST_statement_node>          parse_statement_while          ();
static std::unique_ptr<LLVM_AST_statement_node>          parse_statement_return         ();
static std::unique_ptr<LLVM_AST_statement_node>          parse_statement_input          ();
static std::unique_ptr<LLVM_AST_statement_node>          parse_statement_output         ();

//================================================================================================================================

std::unique_ptr<LLVM_AST_module_node> module_parser(const char *module_name, vector *tokens)
{
    TOKEN_MANAGER.init(tokens);

    auto module = std::make_unique<LLVM_AST_module_node>(module_name);
    while (TOKEN_MANAGER.get().type != TOKEN_FICTIONAL)
    {
        std::unique_ptr<LLVM_AST_statement_node> global = parse_global_decl();
        if (!global) return nullptr;

        LOG_ASSERT(
            dynamic_cast<LLVM_AST_statement_var_decl_node  *>(global.get()) ||
            dynamic_cast<LLVM_AST_statement_func_decl_node *>(global.get()));

        module->globals_.push_back(std::move(global));
    }

    return module;
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_statement_node> parse_global_decl()
{
    if (!token_key_is_int(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected type-name before function or variable declaration");

    if (!token_type_is_name(&TOKEN_MANAGER.next_before()))
        return parser_error_manager_t::statement_error("expected name of new variable or function");

    const size_t src_line = TOKEN_MANAGER.get().line;
    const char *decl_name = TOKEN_MANAGER.next_after().value.name;

    if (token_op_is_comma_point(&TOKEN_MANAGER.get()))
    {
        TOKEN_MANAGER.next();
        return std::make_unique<LLVM_AST_statement_var_decl_node>(src_line, decl_name);
    }

    if (!token_op_is_l_scope_circle(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected '(' before function arguments");

    return parse_func_args_and_body(src_line, decl_name);
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_statement_node> parse_func_args_and_body(const size_t src_line, const char *func_name)
{
    auto function = std::make_unique<LLVM_AST_statement_func_decl_node>(src_line, func_name);

    if (!parse_func_args(*function))
        return nullptr;

    if (!token_op_is_l_scope_figure(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected '{' before function body");
    TOKEN_MANAGER.next();

    if (!parse_expression_scope(function->body_))
        return nullptr;

    if (!token_op_is_r_scope_figure(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected '}' after function body");
    TOKEN_MANAGER.next();

    return function;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_func_args(LLVM_AST_statement_func_decl_node &function)
{
    if (token_op_is_r_scope_circle(&TOKEN_MANAGER.next_before()))
    {
        TOKEN_MANAGER.next();
        return true;
    }

    while (true)
    {
        std::unique_ptr<LLVM_AST_statement_var_decl_node> arg = parse_func_arg();
        if (!arg) return false;

        function.args_.push_back(std::move(arg));

        if (!token_op_is_comma(&TOKEN_MANAGER.get()))
            break;
        TOKEN_MANAGER.next();
    }

    if (!token_op_is_r_scope_circle(&TOKEN_MANAGER.get()))
    {
        parser_error_manager_t::error("expected ')' after function arguments\n");
        return false;
    }
    TOKEN_MANAGER.next();

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_statement_var_decl_node> parse_func_arg()
{
    if (!token_key_is_int(&TOKEN_MANAGER.get()))
    {
        parser_error_manager_t::error("expected type-name before argument declaration");
        return nullptr;
    }

    if (!token_type_is_name(&TOKEN_MANAGER.next_before()))
    {
        parser_error_manager_t::error("expected name of argument");
        return nullptr;
    }

    const size_t src_line = TOKEN_MANAGER.get().line;
    return std::make_unique<LLVM_AST_statement_var_decl_node>(src_line, TOKEN_MANAGER.next_after().value.name);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_expression_scope(std::vector<std::unique_ptr<LLVM_AST_expression_node>> &expressions)
{
    while (!token_op_is_r_scope_figure(&TOKEN_MANAGER.get()))
    {
        std::unique_ptr<LLVM_AST_expression_node> expr = nullptr;

        if (token_type_is_key(&TOKEN_MANAGER.get())) expr = parse_expression_statement();
        else                                         expr = parse_expression_value();

        if (!expr) return false;
        expressions.push_back(std::move(expr));
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_value_node> parse_expression_value()
{
    std::unique_ptr<LLVM_AST_value_node> value = parse_value();

    if (!token_op_is_comma_point(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::value_error("expected ';' after value");
    TOKEN_MANAGER.next();

    return value;
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_value_node> parse_value()
{
    std::unique_ptr<LLVM_AST_value_node> lhs = parse_value_primary();
    if (!lhs) return nullptr;

    return parse_value_binary_operator_rhs(0, std::move(lhs));
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_value_node> parse_value_binary_operator_rhs(const unsigned min_priority, std::unique_ptr<LLVM_AST_value_node> lhs)
{
    while (true)
    {
        LLVM_AST_BINARY_OPERATOR_TYPE before_rhs_type = binary_operator_manager_t::token2ast_binary_operator(TOKEN_MANAGER.get());
        if (before_rhs_type == AST_BINARY_OPERATOR_UNDEF)
            return lhs;

        unsigned before_rhs_priority = binary_operator_manager_t::get_priority(before_rhs_type);
        if (before_rhs_priority < min_priority)
            return lhs;
        const size_t before_rhs_src_line = TOKEN_MANAGER.next_after().line;

        if ((before_rhs_type == AST_BINARY_OPERATOR_ASSIGNMENT) && !lhs->is_lvalue())
            parser_error_manager_t::value_error("expected lvalue before operator '='");

        std::unique_ptr<LLVM_AST_value_node> rhs = parse_value_primary();
        if (!rhs) return nullptr;

        LLVM_AST_BINARY_OPERATOR_TYPE after_rhs_type = binary_operator_manager_t::token2ast_binary_operator(TOKEN_MANAGER.get());
        if (after_rhs_type == AST_BINARY_OPERATOR_UNDEF)
            return std::make_unique<LLVM_AST_operator_binary_node>(before_rhs_src_line, before_rhs_type, std::move(lhs), std::move(rhs));

        unsigned after_rhs_priority = binary_operator_manager_t::get_priority(after_rhs_type);
        if ((after_rhs_priority >  before_rhs_priority) ||
           ((after_rhs_priority >= before_rhs_priority) && after_rhs_type == AST_BINARY_OPERATOR_ASSIGNMENT))
        {
            rhs = parse_value_binary_operator_rhs(before_rhs_priority + 1, std::move(rhs));
        }

        lhs = std::make_unique<LLVM_AST_operator_binary_node>(before_rhs_src_line, before_rhs_type, std::move(lhs), std::move(rhs));
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_value_node> parse_value_primary()
{
    if (token_type_is_int(&TOKEN_MANAGER.get()))
    {
        const size_t src_line = TOKEN_MANAGER.get().line;
        return std::make_unique<LLVM_AST_value_imm_node>(src_line, TOKEN_MANAGER.next_after().value.imm_int);
    }

    if (token_type_is_name(&TOKEN_MANAGER.get()))
    {
        const size_t src_line    = TOKEN_MANAGER.get().line;
        const char  *access_name = TOKEN_MANAGER.next_after().value.name;

        if (!token_op_is_l_scope_circle(&TOKEN_MANAGER.get()))
            return std::make_unique<LLVM_AST_value_var_node>(src_line, access_name);

        return parse_value_func_params(src_line, access_name);
    }

    if (token_op_is_l_scope_circle(&TOKEN_MANAGER.get()))
    {
        TOKEN_MANAGER.next();

        std::unique_ptr<LLVM_AST_value_node> sub_value = parse_value();
        if (!sub_value) return nullptr;

        if (!token_op_is_r_scope_circle(&TOKEN_MANAGER.get()))
            return parser_error_manager_t::value_error("expected ')' after sub value");
        TOKEN_MANAGER.next();

        return sub_value;
    }

    LLVM_AST_UNARY_OPERATOR_TYPE unary_type = unary_operator_manager_t::token2ast_unary_operator(TOKEN_MANAGER.get());
    if (unary_type != AST_UNARY_OPERATOR_UNDEF)
    {
        const size_t src_line = TOKEN_MANAGER.next_after().line;

        std::unique_ptr<LLVM_AST_value_node> sub_value = parse_value();
        if (!sub_value) return nullptr;

        return std::make_unique<LLVM_AST_operator_unary_node>(src_line, unary_type, std::move(sub_value));
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_value_node> parse_value_func_params(const size_t src_line, const char *func_name)
{
    auto func_call = std::make_unique<LLVM_AST_value_func_call_node>(src_line, func_name);

    if (token_op_is_r_scope_circle(&TOKEN_MANAGER.next_before()))
    {
        TOKEN_MANAGER.next();
        return func_call;
    }

    while (true)
    {
        std::unique_ptr<LLVM_AST_value_node> param = parse_value();
        if (!param) return nullptr;

        func_call->params_.push_back(std::move(param));

        if (!token_op_is_comma(&TOKEN_MANAGER.get()))
            break;
        TOKEN_MANAGER.next();
    }

    if (!token_op_is_r_scope_circle(&TOKEN_MANAGER.get()))
        parser_error_manager_t::value_error("expected ')' after function parameters");
    TOKEN_MANAGER.next();

    return func_call;
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_value_node> parse_value_lvalue()
{
    std::unique_ptr<LLVM_AST_value_node> lvalue = parse_value();
    if (!lvalue->is_lvalue()) return nullptr;

    return lvalue;
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_statement_node> parse_expression_statement()
{
    std::unique_ptr<LLVM_AST_statement_node> statement = nullptr;

    switch (TOKEN_MANAGER.get().value.key)
    {
        case KEY_INT   : statement = parse_statement_var_decl(); break;
        case KEY_IF    : statement = parse_statement_if      (); break;
        case KEY_WHILE : statement = parse_statement_while   (); break;
        case KEY_RETURN: statement = parse_statement_return  (); break;
        case KEY_INPUT : statement = parse_statement_input   (); break;
        case KEY_OUTPUT: statement = parse_statement_output  (); break;

        case KEY_ELSE  : return parser_error_manager_t::statement_error("\"else\" statement can't be master statement");
        case KEY_UNDEF :
        default        : LOG_ASSERT_VERBOSE(false, "undef key-token"); return nullptr;
    }

    return statement;
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_statement_node> parse_statement_var_decl()
{
    if (!token_type_is_name(&TOKEN_MANAGER.next_before()))
        return parser_error_manager_t::statement_error("expected name of new variable");

    const size_t src_line = TOKEN_MANAGER.get().line;
    const char  *var_name = TOKEN_MANAGER.next_after().value.name;

    if (!token_op_is_comma_point(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected ';' after variable declaration");
    TOKEN_MANAGER.next();

    return std::make_unique<LLVM_AST_statement_var_decl_node>(src_line, var_name);
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_statement_node> parse_statement_if()
{
    const size_t src_line = TOKEN_MANAGER.get().line;

    if (!token_op_is_l_scope_circle(&TOKEN_MANAGER.next_before()))
        return parser_error_manager_t::statement_error("expected '(' before condition");
    TOKEN_MANAGER.next();

    std::unique_ptr<LLVM_AST_value_node> cond = parse_value();
    if (!cond) return nullptr;

    if (!token_op_is_r_scope_circle(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected ')' after condition");

    std::unique_ptr<LLVM_AST_statement_if_node> if_statement = std::make_unique<LLVM_AST_statement_if_node>(src_line, std::move(cond));

    if (!token_op_is_l_scope_figure(&TOKEN_MANAGER.next_before()))
        return parser_error_manager_t::statement_error("expected '{' before \"then\" statement");
    TOKEN_MANAGER.next();

    if (!parse_expression_scope(if_statement->then_))
        return nullptr;

    if (!token_op_is_r_scope_figure(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected '}' after \"then\" statement");

    if (!token_key_is_else(&TOKEN_MANAGER.next_before()))
        return if_statement;

    if (!token_op_is_l_scope_figure(&TOKEN_MANAGER.next_before()))
        return parser_error_manager_t::statement_error("expected '{' before \"else\" statement");
    TOKEN_MANAGER.next();

    if (!parse_expression_scope(if_statement->else_))
        return nullptr;

    if (!token_op_is_r_scope_figure(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected '}' after \"else\" statement");
    TOKEN_MANAGER.next();

    return if_statement;
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_statement_node> parse_statement_while()
{
    const size_t src_line = TOKEN_MANAGER.get().line;

    if (!token_op_is_l_scope_circle(&TOKEN_MANAGER.next_before()))
        return parser_error_manager_t::statement_error("expected '(' before condition");
    TOKEN_MANAGER.next();

    std::unique_ptr<LLVM_AST_value_node> cond = parse_value();
    if (!cond) return nullptr;

    if (!token_op_is_r_scope_circle(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected ')' after condition");

    std::unique_ptr<LLVM_AST_statement_while_node> while_statement = std::make_unique<LLVM_AST_statement_while_node>(src_line, std::move(cond));

    if (!token_op_is_l_scope_figure(&TOKEN_MANAGER.next_before()))
        return parser_error_manager_t::statement_error("expected '{' before \"then\" statement");
    TOKEN_MANAGER.next();

    if (!parse_expression_scope(while_statement->then_))
        return nullptr;

    if (!token_op_is_r_scope_figure(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected '}' after \"then\" statement");
    TOKEN_MANAGER.next();

    return while_statement;
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_statement_node> parse_statement_return()
{
    const size_t src_line = TOKEN_MANAGER.next_after().line;

    std::unique_ptr<LLVM_AST_value_node> value = parse_value();
    if (!value) return nullptr;

    if (!token_op_is_comma_point(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected ';' after \"return\" statement");
    TOKEN_MANAGER.next();

    return std::make_unique<LLVM_AST_statement_return_node>(src_line, std::move(value));
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_statement_node> parse_statement_input()
{
    const size_t src_line = TOKEN_MANAGER.next_after().line;

    std::unique_ptr<LLVM_AST_value_node> lvalue = parse_value_lvalue();
    if (!lvalue) return nullptr;

    if (!token_op_is_comma_point(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected ';' after \"input\" statement");
    TOKEN_MANAGER.next();

    return std::make_unique<LLVM_AST_statement_input_node>(src_line, std::move(lvalue));
}

//--------------------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<LLVM_AST_statement_node> parse_statement_output()
{
    const size_t src_line = TOKEN_MANAGER.next_after().line;

    std::unique_ptr<LLVM_AST_value_node> value = parse_value();
    if (!value) return nullptr;

    if (!token_op_is_comma_point(&TOKEN_MANAGER.get()))
        return parser_error_manager_t::statement_error("expected ';' after \"output\" statement");
    TOKEN_MANAGER.next();

    return std::make_unique<LLVM_AST_statement_output_node>(src_line, std::move(value));
}
