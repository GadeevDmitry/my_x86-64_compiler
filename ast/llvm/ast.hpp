#ifndef LLVM_AST_HPP
#define LLVM_AST_HPP

#include <vector>
#include <memory>

//================================================================================================================================

class LLVM_AST_expression_node
{
// member functions
public:
    LLVM_AST_expression_node(size_t src_line);

    virtual ~LLVM_AST_expression_node() = default;
    virtual void dump() const;

// member data
public:
    const size_t src_line_;
};

//--------------------------------------------------------------------------------------------------------------------------------

class LLVM_AST_value_node: public LLVM_AST_expression_node
{
// member functions
public:
    LLVM_AST_value_node(size_t src_line);

    bool is_lvalue() const;
    void dump     () const override;
};

//--------------------------------------------------------------------------------------------------------------------------------

class LLVM_AST_statement_node: public LLVM_AST_expression_node
{
// member functions
public:
    LLVM_AST_statement_node(size_t src_line);
    void dump() const override;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class LLVM_AST_value_imm_node: public LLVM_AST_value_node
{
// member functions
public:
    LLVM_AST_value_imm_node(size_t src_line, int value);
    void dump() const override;

// member data
public:
    const int value_;
};

//--------------------------------------------------------------------------------------------------------------------------------

class LLVM_AST_value_var_node: public LLVM_AST_value_node
{
// member functions
public:
    LLVM_AST_value_var_node(size_t src_line, const char *name);
    void dump() const override;

// member data
public:
    const char *const name_;
};

//--------------------------------------------------------------------------------------------------------------------------------

class LLVM_AST_value_func_call_node: public LLVM_AST_value_node
{
// member functions
public:
    LLVM_AST_value_func_call_node(size_t src_line, const char *name);
    void dump() const override;

// member data
public:
    const char *const                                 name_;
    std::vector<std::unique_ptr<LLVM_AST_value_node>> params_;
};

//--------------------------------------------------------------------------------------------------------------------------------

enum LLVM_AST_UNARY_OPERATOR_TYPE
{
    AST_UNARY_OPERATOR_UNDEF = -1,

    AST_UNARY_OPERATOR_SQRT  = 0 ,
    AST_UNARY_OPERATOR_NOT       ,

    AST_UNARY_OPERATOR_NUM       ,
};

class LLVM_AST_operator_unary_node: public LLVM_AST_value_node
{
// static
private:
    static constexpr const char *operators_representation[] =
    {
        [AST_UNARY_OPERATOR_SQRT] = "sqrt",
        [AST_UNARY_OPERATOR_NOT ] = "!"   ,
    };

// member functions
public:
    LLVM_AST_operator_unary_node(size_t src_line, LLVM_AST_UNARY_OPERATOR_TYPE type, std::unique_ptr<LLVM_AST_value_node> &&operand);
    void dump() const override;

// member data
public:
    const LLVM_AST_UNARY_OPERATOR_TYPE      type_;
    std::unique_ptr<LLVM_AST_value_node> operand_;
};

//--------------------------------------------------------------------------------------------------------------------------------

enum LLVM_AST_BINARY_OPERATOR_TYPE
{
    AST_BINARY_OPERATOR_UNDEF = -1,

    AST_BINARY_OPERATOR_ADD   = 0 ,
    AST_BINARY_OPERATOR_SUB       ,
    AST_BINARY_OPERATOR_MUL       ,
    AST_BINARY_OPERATOR_DIV       ,
    AST_BINARY_OPERATOR_ARE_EQUAL ,
    AST_BINARY_OPERATOR_MORE      ,
    AST_BINARY_OPERATOR_LESS      ,
    AST_BINARY_OPERATOR_MORE_EQUAL,
    AST_BINARY_OPERATOR_LESS_EQUAL,
    AST_BINARY_OPERATOR_NOT_EQUAL ,
    AST_BINARY_OPERATOR_LOG_OR    ,
    AST_BINARY_OPERATOR_LOG_AND   ,
    AST_BINARY_OPERATOR_ASSIGNMENT,

    AST_BINARY_OPERATOR_NUM       ,
};

class LLVM_AST_operator_binary_node: public LLVM_AST_value_node
{
// static
private:
    static constexpr const char *operators_representation[] =
    {
        [AST_BINARY_OPERATOR_ADD       ] = "+" ,
        [AST_BINARY_OPERATOR_SUB       ] = "-" ,
        [AST_BINARY_OPERATOR_MUL       ] = "*" ,
        [AST_BINARY_OPERATOR_DIV       ] = "/" ,
        [AST_BINARY_OPERATOR_ARE_EQUAL ] = "==",
        [AST_BINARY_OPERATOR_MORE      ] = ">" ,
        [AST_BINARY_OPERATOR_LESS      ] = "<" ,
        [AST_BINARY_OPERATOR_MORE_EQUAL] = ">=",
        [AST_BINARY_OPERATOR_LESS_EQUAL] = "<=",
        [AST_BINARY_OPERATOR_NOT_EQUAL ] = "!=",
        [AST_BINARY_OPERATOR_LOG_OR    ] = "||",
        [AST_BINARY_OPERATOR_LOG_AND   ] = "&&",
        [AST_BINARY_OPERATOR_ASSIGNMENT] = "=" ,
    };

// member functions
public:
    LLVM_AST_operator_binary_node(
        size_t src_line,
        LLVM_AST_BINARY_OPERATOR_TYPE type,
        std::unique_ptr<LLVM_AST_value_node> &&lhs,
        std::unique_ptr<LLVM_AST_value_node> &&rhs);

    void dump() const override;

// member data
public:
    const LLVM_AST_BINARY_OPERATOR_TYPE type_;
    std::unique_ptr<LLVM_AST_value_node> lhs_;
    std::unique_ptr<LLVM_AST_value_node> rhs_;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class LLVM_AST_statement_if_node: public LLVM_AST_statement_node
{
// member functions
public:
    LLVM_AST_statement_if_node(
        size_t src_line,
        std::unique_ptr<LLVM_AST_value_node> &&cond);

    void dump() const override;

// member data
public:
    std::unique_ptr<LLVM_AST_value_node>                   cond_;
    std::vector<std::unique_ptr<LLVM_AST_expression_node>> then_;
    std::vector<std::unique_ptr<LLVM_AST_expression_node>> else_;
};

//--------------------------------------------------------------------------------------------------------------------------------

class LLVM_AST_statement_while_node: public LLVM_AST_statement_node
{
// member functions
public:
    LLVM_AST_statement_while_node(
        size_t src_line,
        std::unique_ptr<LLVM_AST_value_node> &&cond);

    void dump() const override;

// member data
public:
    std::unique_ptr<LLVM_AST_value_node>                   cond_;
    std::vector<std::unique_ptr<LLVM_AST_expression_node>> then_;
};

//--------------------------------------------------------------------------------------------------------------------------------

class LLVM_AST_statement_var_decl_node: public LLVM_AST_statement_node
{
// member functions
public:
    LLVM_AST_statement_var_decl_node(size_t src_line, const char *name);
    void dump() const override;

// member data
public:
    const char *const name_;
};

//--------------------------------------------------------------------------------------------------------------------------------

class LLVM_AST_statement_func_decl_node: public LLVM_AST_statement_node
{
// member functions
public:
    LLVM_AST_statement_func_decl_node(size_t src_line, const char *name);
    void dump() const override;

// member data
public:
    const char *const                                              name_;
    std::vector<std::unique_ptr<LLVM_AST_statement_var_decl_node>> args_;
    std::vector<std::unique_ptr<LLVM_AST_expression_node>>         body_;
};

//--------------------------------------------------------------------------------------------------------------------------------

class LLVM_AST_statement_return_node: public LLVM_AST_statement_node
{
// member functions
public:
    LLVM_AST_statement_return_node(
        size_t src_line,
        std::unique_ptr<LLVM_AST_value_node> &&value);

    void dump() const override;

// member data
public:
    std::unique_ptr<LLVM_AST_value_node> value_;
};

//--------------------------------------------------------------------------------------------------------------------------------

class LLVM_AST_statement_input_node: public LLVM_AST_statement_node
{
// member functions
public:
    LLVM_AST_statement_input_node(
        size_t src_line,
        std::unique_ptr<LLVM_AST_value_node> &&lvalue);

    void dump() const override;

// member data
public:
    std::unique_ptr<LLVM_AST_value_node> lvalue_;
};

//--------------------------------------------------------------------------------------------------------------------------------

class LLVM_AST_statement_output_node: public LLVM_AST_statement_node
{
// member functions
public:
    LLVM_AST_statement_output_node(
        size_t src_line,
        std::unique_ptr<LLVM_AST_value_node> &&value);

    void dump() const override;

// member data
public:
    std::unique_ptr<LLVM_AST_value_node> value_;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class LLVM_AST_module_node
{
// member functions
public:
    LLVM_AST_module_node(const char *name);
    void dump() const;

// member data
public:
    const char *name_;
    std::vector<std::unique_ptr<LLVM_AST_statement_node>> globals_;
};

#endif // LLVM_AST_HPP
