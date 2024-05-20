#include <cstdio>

#include "lib/include/log.h"
#include "ast.hpp"

//================================================================================================================================

LLVM_AST_expression_node::LLVM_AST_expression_node(size_t src_line):
src_line_(src_line)
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_expression_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_expression_node (this = %p)\n"
        "{\n"
        "    src_line_ = %llu\n"
        "};", "\n\n",
        this,
        src_line_);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_value_node::LLVM_AST_value_node(size_t src_line):
LLVM_AST_expression_node(src_line)
{}

//--------------------------------------------------------------------------------------------------------------------------------

bool LLVM_AST_value_node::is_lvalue() const
{
    if (dynamic_cast<const LLVM_AST_value_var_node *>(this))
        return true;

    const LLVM_AST_operator_binary_node *binary = nullptr;
    if ((binary = dynamic_cast<const LLVM_AST_operator_binary_node *>(this)))
        return binary->type_ == AST_BINARY_OPERATOR_ASSIGNMENT;

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_value_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_value_node (this = %p)\n"
        "{\n"
        "    src_line_ = %llu\n"
        "};", "\n\n",
        this,
        src_line_);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_statement_node::LLVM_AST_statement_node(size_t src_line):
LLVM_AST_expression_node(src_line)
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_statement_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_statement_node (this = %p)\n"
        "{\n"
        "    src_line_ = %llu\n"
        "};", "\n\n",
        this,
        src_line_);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_value_imm_node::LLVM_AST_value_imm_node(size_t src_line, int value):
LLVM_AST_value_node(src_line),
value_(value)
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_value_imm_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_value_imm_node (this = %p)\n"
        "{", "\n", this);

    LOG_TAB++;
    USUAL_FIELD_DUMP("src_line_", "%llu", src_line_);
    USUAL_FIELD_DUMP("value_   ", "%d"  , value_);
    LOG_TAB--;

    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_value_var_node::LLVM_AST_value_var_node(size_t src_line, const char *name):
LLVM_AST_value_node(src_line),
name_(name)
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_value_var_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_value_var_node (this = %p)\n"
        "{", "\n", this);

    LOG_TAB++;
    USUAL_FIELD_DUMP("src_line_", "%llu", src_line_);
    USUAL_FIELD_DUMP("name_    ", "%s"  , name_);
    LOG_TAB--;

    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_value_func_call_node::LLVM_AST_value_func_call_node(size_t src_line, const char *name):
LLVM_AST_value_node(src_line),
name_(name)
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_value_func_call_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_value_func_call_node (this = %p)\n"
        "{", "\n", this);
    LOG_TAB++;

    USUAL_FIELD_DUMP("src_line_", "%llu", src_line_);
    USUAL_FIELD_DUMP("name_    ", "%s"  , name_);

    {
        LOG_TAB_SERVICE_MESSAGE(
            "params_\n"
            "{", "\n");
        LOG_TAB++;

        size_t cnt = 1;
        for (auto &param : params_)
        {
            LOG_TAB_SERVICE_MESSAGE("#%llu", "\n", cnt++);
            param->dump();
        }

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_operator_unary_node::LLVM_AST_operator_unary_node(
    size_t src_line,
    LLVM_AST_UNARY_OPERATOR_TYPE type,
    std::unique_ptr<LLVM_AST_value_node> &&operand):
LLVM_AST_value_node(src_line),
type_   (type),
operand_(std::move(operand))
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_operator_unary_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_operator_unary_node (this = %p)\n"
        "{", "\n", this);
    LOG_TAB++;

    USUAL_FIELD_DUMP    ("src_line_", "%llu", src_line_);
    if ((type_ <= AST_UNARY_OPERATOR_UNDEF) ||
        (type_ >= AST_UNARY_OPERATOR_NUM  ))
        ERROR_FIELD_DUMP("type_    ", "%d", type_);
    else
        USUAL_FIELD_DUMP("type_    ", "%s", operators_representation[type_]);

    {
        LOG_TAB_SERVICE_MESSAGE(
            "operand_\n"
            "{", "\n");
        LOG_TAB++;

        operand_->dump();

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_operator_binary_node::LLVM_AST_operator_binary_node(
    size_t src_line,
    LLVM_AST_BINARY_OPERATOR_TYPE type,
    std::unique_ptr<LLVM_AST_value_node> &&lhs,
    std::unique_ptr<LLVM_AST_value_node> &&rhs):
LLVM_AST_value_node(src_line),
type_(type),
lhs_ (std::move(lhs)),
rhs_ (std::move(rhs))
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_operator_binary_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_operator_binary_node (this = %p)\n"
        "{", "\n", this);
    LOG_TAB++;

    USUAL_FIELD_DUMP    ("src_line_", "%llu", src_line_);
    if ((type_ <= AST_BINARY_OPERATOR_UNDEF) ||
        (type_ >= AST_BINARY_OPERATOR_NUM  ))
        ERROR_FIELD_DUMP("type_    ", "%d", type_);
    else
        USUAL_FIELD_DUMP("type_    ", "%s", operators_representation[type_]);

    {
        LOG_TAB_SERVICE_MESSAGE(
            "lhs_\n"
            "{", "\n");
        LOG_TAB++;

        lhs_->dump();

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }
    {
        LOG_TAB_SERVICE_MESSAGE(
            "rhs_\n"
            "{", "\n");
        LOG_TAB++;

        rhs_->dump();

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_statement_if_node::LLVM_AST_statement_if_node(
    size_t src_line,
    std::unique_ptr<LLVM_AST_value_node> &&cond):
LLVM_AST_statement_node(src_line),
cond_(std::move(cond)),
then_(),
else_()
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_statement_if_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_statement_if_node (this = %p)\n"
        "{", "\n", this);
    LOG_TAB++;

    USUAL_FIELD_DUMP("src_line_", "%llu", src_line_);
    {
        LOG_TAB_SERVICE_MESSAGE(
            "cond_\n"
            "{", "\n");
        LOG_TAB++;

        cond_->dump();

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }
    {
        LOG_TAB_SERVICE_MESSAGE(
            "then_\n"
            "{", "\n");
        LOG_TAB++;

        size_t cnt = 1;
        for (auto &then_expr : then_)
        {
            LOG_TAB_SERVICE_MESSAGE("#%llu", "\n", cnt++);
            then_expr->dump();
        }

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }
    {
        LOG_TAB_SERVICE_MESSAGE(
            "else_\n"
            "{", "\n");
        LOG_TAB++;

        size_t cnt = 1;
        for (auto &else_expr : else_)
        {
            LOG_TAB_SERVICE_MESSAGE("#%llu", "\n", cnt++);
            else_expr->dump();
        }

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_statement_while_node::LLVM_AST_statement_while_node(
    size_t src_line,
    std::unique_ptr<LLVM_AST_value_node> &&cond):
LLVM_AST_statement_node(src_line),
cond_(std::move(cond)),
then_()
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_statement_while_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_statement_while_node (this = %p)\n"
        "{", "\n", this);
    LOG_TAB++;

    USUAL_FIELD_DUMP("src_line_", "%llu", src_line_);
    {
        LOG_TAB_SERVICE_MESSAGE(
            "cond_\n"
            "{", "\n");
        LOG_TAB++;

        cond_->dump();

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }
    {
        LOG_TAB_SERVICE_MESSAGE(
            "then_\n"
            "{", "\n");
        LOG_TAB++;

        size_t cnt = 1;
        for (auto &then_expr : then_)
        {
            LOG_TAB_SERVICE_MESSAGE("#%llu", "\n", cnt++);
            then_expr->dump();
        }

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_statement_var_decl_node::LLVM_AST_statement_var_decl_node(size_t src_line, const char *name):
LLVM_AST_statement_node(src_line),
name_(name)
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_statement_var_decl_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_statement_var_decl_node (this = %p)\n"
        "{", "\n", this);

    LOG_TAB++;
    USUAL_FIELD_DUMP("src_line_", "%llu", src_line_);
    USUAL_FIELD_DUMP("name_    ", "%s"  , name_);
    LOG_TAB--;

    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_statement_func_decl_node::LLVM_AST_statement_func_decl_node(size_t src_line, const char *name):
LLVM_AST_statement_node(src_line),
name_(name),
args_(),
body_()
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_statement_func_decl_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_statement_func_decl_node (this = %p)\n"
        "{", "\n", this);
    LOG_TAB++;

    USUAL_FIELD_DUMP("src_line_", "%llu", src_line_);
    USUAL_FIELD_DUMP("name_    ", "%s"  , name_);
    {
        LOG_TAB_SERVICE_MESSAGE(
            "args_\n"
            "{", "\n");
        LOG_TAB++;

        size_t cnt = 1;
        for (auto &arg : args_)
        {
            LOG_TAB_SERVICE_MESSAGE("#%llu", "\n", cnt++);
            arg->dump();
        }

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }
    {
        LOG_TAB_SERVICE_MESSAGE(
            "body_\n"
            "{", "\n");
        LOG_TAB++;

        size_t cnt = 1;
        for (auto &body_expr : body_)
        {
            LOG_TAB_SERVICE_MESSAGE("#%llu", "\n", cnt++);
            body_expr->dump();
        }

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_statement_return_node::LLVM_AST_statement_return_node(
    size_t src_line,
    std::unique_ptr<LLVM_AST_value_node> &&value):
LLVM_AST_statement_node(src_line),
value_(std::move(value))
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_statement_return_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_statement_return_node (this = %p)\n"
        "{", "\n", this);
    LOG_TAB++;

    USUAL_FIELD_DUMP("src_line_", "%llu", src_line_);
    {
        LOG_TAB_SERVICE_MESSAGE(
            "value_\n"
            "{", "\n");
        LOG_TAB++;

        value_->dump();

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_statement_input_node::LLVM_AST_statement_input_node(
    size_t src_line,
    std::unique_ptr<LLVM_AST_value_node> &&lvalue):
LLVM_AST_statement_node(src_line),
lvalue_(std::move(lvalue))
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_statement_input_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_statement_input_node (this = %p)\n"
        "{", "\n", this);
    LOG_TAB++;

    USUAL_FIELD_DUMP("src_line_", "%llu", src_line_);
    {
        LOG_TAB_SERVICE_MESSAGE(
            "lvalue_\n"
            "{", "\n");
        LOG_TAB++;

        lvalue_->dump();

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_statement_output_node::LLVM_AST_statement_output_node(
    size_t src_line,
    std::unique_ptr<LLVM_AST_value_node> &&value):
LLVM_AST_statement_node(src_line),
value_(std::move(value))
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_statement_output_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_statement_output_node (this = %p)\n"
        "{", "\n", this);
    LOG_TAB++;

    USUAL_FIELD_DUMP("src_line_", "%llu", src_line_);
    {
        LOG_TAB_SERVICE_MESSAGE(
            "value_\n"
            "{", "\n");
        LOG_TAB++;

        value_->dump();

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLVM_AST_module_node::LLVM_AST_module_node(const char *name):
name_(name)
{}

//--------------------------------------------------------------------------------------------------------------------------------

void LLVM_AST_module_node::dump() const
{
    LOG_TAB_SERVICE_MESSAGE(
        "LLVM_AST_module_node (this = %p)\n"
        "{", "\n", this);
    LOG_TAB++;

    USUAL_FIELD_DUMP("name_", "%s", name_);

    {
        LOG_TAB_SERVICE_MESSAGE(
            "globals_\n"
            "{", "\n");
        LOG_TAB++;

        size_t cnt = 1;
        for (auto &global : globals_)
        {
            LOG_TAB_SERVICE_MESSAGE("#%llu", "\n", cnt++);
            global->dump();
        }

        LOG_TAB--;
        LOG_TAB_SERVICE_MESSAGE("}", "\n");
    }

    LOG_TAB--;
    LOG_TAB_SERVICE_MESSAGE("};", "\n\n");
}
