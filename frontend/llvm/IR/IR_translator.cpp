#include <stdio.h>

#include "IR_translator.hpp"
#include "lib/include/log.h"

#include "llvm/IR/Verifier.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"

//================================================================================================================================

struct codegen_manager_t
{
// types
public:
    struct local_manager_t
    {
    // types
    private:
        struct local_entry_t
        {
            local_entry_t(size_t scope, llvm::AllocaInst *addr):
            scope_(scope),
            addr_ (addr)
            {}

            size_t           scope_;
            llvm::AllocaInst *addr_;
        };

    // member functions
    public:
        bool new_entry(size_t cur_scope, llvm::AllocaInst *addr)
        {
            if (!entries_.empty())
            {
                local_entry_t &top_entry = entries_.top();
                LOG_ASSERT(top_entry.scope_ <= cur_scope);

                if (top_entry.scope_ == cur_scope)
                    return false;
            }

            entries_.emplace(cur_scope, addr);
            return true;
        }

        void scope_out(size_t expiring_scope)
        {
            if (entries_.empty())
                return;

            local_entry_t &top_entry = entries_.top();
            LOG_ASSERT(top_entry.scope_ <= expiring_scope);

            if (top_entry.scope_ == expiring_scope)
                entries_.pop();
        }

        llvm::AllocaInst *get_value() const
        {
            if (entries_.empty())
                return nullptr;

            return entries_.top().addr_;
        }

    // member data
    private:
        std::stack<local_entry_t> entries_;
    };

// member functions
public:
    codegen_manager_t():
    context_  (),
    module_   (nullptr),
    builder_  (context_),
    cur_scope_(0),
    locals_   ()
    {}

    void init(llvm::Module *module)
    {
        module_ = module;
    }

    bool insert_local(const char *name, llvm::AllocaInst *addr)
    {
        return locals_[name].new_entry(cur_scope_, addr);
    }

    llvm::AllocaInst *get_local(const char *name)
    {
        return locals_[name].get_value();
    }

    bool is_global_scope() const
    {
        return cur_scope_ == 0;
    }

    void scope_in()
    {
        ++cur_scope_;
    }

    void scope_out()
    {
        LOG_ASSERT(cur_scope_ > 0);

        for (auto &local : locals_)
            local.second.scope_out(cur_scope_);

        --cur_scope_;
    }

// member data
public:
    llvm::LLVMContext                     context_;
    llvm::Module                          *module_;
    llvm::IRBuilder<>                     builder_;

private:
    size_t                              cur_scope_;
    std::map<std::string, local_manager_t> locals_;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct translator_error_manager_t
{
// static
public:
    static llvm::Value *error(const char *err_msg, size_t src_line)
    {
        fprintf(stderr, "line %3lu: " BASH_COLOR_RED "ERROR: " BASH_COLOR_WHITE "%s\n", src_line, err_msg);
        return nullptr;
    }
};

//================================================================================================================================

std::unique_ptr<llvm::Module> module_translator(LLVM_AST_module_node *AST_module)
{
    codegen_manager_t codegen_manager;
    auto LLVM_module = std::make_unique<llvm::Module>(AST_module->name_, codegen_manager.context_);
    codegen_manager.init(LLVM_module.get());

    for (auto &global : AST_module->globals_)
    {
        llvm::Value *global_value = global->codegen(codegen_manager);
        if (!global_value)
            return nullptr;

        global_value->dump();
    }

    return LLVM_module;
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_value_imm_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    return llvm::ConstantFP::get(CODEGEN_MANAGER.context_, llvm::APFloat((double) value_));
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_value_var_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    llvm::AllocaInst *local = CODEGEN_MANAGER.get_local(name_);
    if (local)
        return CODEGEN_MANAGER.builder_.CreateLoad(local->getAllocatedType(), local, name_);

    llvm::GlobalVariable *global = CODEGEN_MANAGER.module_->getGlobalVariable(name_);
    if (global)
        return CODEGEN_MANAGER.builder_.CreateLoad(global->getValueType(), global, name_);

    return translator_error_manager_t::error("unknown variable name", src_line_);
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_value_func_call_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    llvm::Function *callee = CODEGEN_MANAGER.module_->getFunction(name_);
    if (!callee)
        return translator_error_manager_t::error("unknown function name", src_line_);

    if (callee->arg_size() != params_.size())
        return translator_error_manager_t::error("wrong number of parameters passed", src_line_);

    std::vector<llvm::Value *> params;
    for (int param_cnt = 0; param_cnt < params_.size(); ++param_cnt)
    {
        params.push_back(params_[param_cnt]->codegen(CODEGEN_MANAGER));
        if (!params.back()) return nullptr;
    }

    return CODEGEN_MANAGER.builder_.CreateCall(callee, params, name_);
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_operator_unary_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    llvm::Value *operand = operand_->codegen(CODEGEN_MANAGER);
    if (!operand) return nullptr;

    switch (type_)
    {
        case AST_UNARY_OPERATOR_SQRT:
        {
            return CODEGEN_MANAGER.builder_.CreateUnaryIntrinsic(llvm::Intrinsic::sqrt, operand, nullptr, "sqrt");
        }
        case AST_UNARY_OPERATOR_NOT:
        {
            llvm::Value *not_instr = CODEGEN_MANAGER.builder_.CreateNot(operand, "!");
            return CODEGEN_MANAGER.builder_.CreateUIToFP(not_instr, llvm::Type::getDoubleTy(CODEGEN_MANAGER.context_), "cast to double");
        }
        case AST_UNARY_OPERATOR_UNDEF:
        case AST_UNARY_OPERATOR_NUM  :
        default                      :
        {
            LOG_ASSERT_VERBOSE(false, "unknown unary operator");
            return nullptr;
        } break;
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_operator_binary_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    llvm::Value *lhs = lhs_->codegen(CODEGEN_MANAGER);
    llvm::Value *rhs = rhs_->codegen(CODEGEN_MANAGER);
    if (!lhs_ || !rhs_) return nullptr;

    static constexpr llvm::FCmpInst::Predicate ast2llvm_cmp_binary_operators[] =
    {
        [AST_BINARY_OPERATOR_ARE_EQUAL ] = llvm::FCmpInst::FCMP_UEQ,
        [AST_BINARY_OPERATOR_MORE      ] = llvm::FCmpInst::FCMP_UGT,
        [AST_BINARY_OPERATOR_LESS      ] = llvm::FCmpInst::FCMP_ULT,
        [AST_BINARY_OPERATOR_MORE_EQUAL] = llvm::FCmpInst::FCMP_UGE,
        [AST_BINARY_OPERATOR_LESS_EQUAL] = llvm::FCmpInst::FCMP_ULE,
        [AST_BINARY_OPERATOR_NOT_EQUAL ] = llvm::FCmpInst::FCMP_UNE,
    };

    switch (type_)
    {
        case AST_BINARY_OPERATOR_ADD: return CODEGEN_MANAGER.builder_.CreateAdd (lhs, rhs, "+");
        case AST_BINARY_OPERATOR_SUB: return CODEGEN_MANAGER.builder_.CreateSub (lhs, rhs, "-");
        case AST_BINARY_OPERATOR_MUL: return CODEGEN_MANAGER.builder_.CreateMul (lhs, rhs, "*");
        case AST_BINARY_OPERATOR_DIV: return CODEGEN_MANAGER.builder_.CreateSDiv(lhs, rhs, "/");

        case AST_BINARY_OPERATOR_ARE_EQUAL :
        case AST_BINARY_OPERATOR_MORE      :
        case AST_BINARY_OPERATOR_LESS      :
        case AST_BINARY_OPERATOR_MORE_EQUAL:
        case AST_BINARY_OPERATOR_LESS_EQUAL:
        case AST_BINARY_OPERATOR_NOT_EQUAL :
        {
            llvm::Value *are_equal_instr =
                CODEGEN_MANAGER.builder_.CreateFCmp(ast2llvm_cmp_binary_operators[type_], lhs, rhs, operators_representation[type_]);
            return CODEGEN_MANAGER.builder_.CreateUIToFP(are_equal_instr, llvm::Type::getDoubleTy(CODEGEN_MANAGER.context_));
        }

        case AST_BINARY_OPERATOR_LOG_OR :
        case AST_BINARY_OPERATOR_LOG_AND:
        {
            llvm::Value *lhs_integer = CODEGEN_MANAGER.builder_.CreateFPToUI(lhs, llvm::Type::getInt1Ty(CODEGEN_MANAGER.context_), "lhs: double to int");
            llvm::Value *rhs_integer = CODEGEN_MANAGER.builder_.CreateFPToUI(lhs, llvm::Type::getInt1Ty(CODEGEN_MANAGER.context_), "rhs: double to int");

            llvm::Value *operator_instr = nullptr;
            if (type_ == AST_BINARY_OPERATOR_LOG_OR)
                operator_instr = CODEGEN_MANAGER.builder_.CreateLogicalOr(lhs_integer, rhs_integer, "||");
            else
                operator_instr = CODEGEN_MANAGER.builder_.CreateLogicalAnd(lhs_integer, rhs_integer, "&&");

            return CODEGEN_MANAGER.builder_.CreateUIToFP(operator_instr, llvm::Type::getDoubleTy(CODEGEN_MANAGER.context_));
        }

        case AST_BINARY_OPERATOR_ASSIGNMENT:
        {
            LLVM_AST_value_var_node *lvalue_node = dynamic_cast<LLVM_AST_value_var_node *>(lhs_.get());
            LOG_ASSERT(lvalue_node != nullptr);

            llvm::AllocaInst *local = CODEGEN_MANAGER.get_local(lvalue_node->name_);
            if (local)
            {
                CODEGEN_MANAGER.builder_.CreateStore(rhs, local);
                return rhs;
            }

            llvm::GlobalObject *global = CODEGEN_MANAGER.module_->getGlobalVariable(lvalue_node->name_);
            if (!global) translator_error_manager_t::error("unknown variable name", src_line_);

            CODEGEN_MANAGER.builder_.CreateStore(rhs, global);
            return rhs;
        }

        case AST_BINARY_OPERATOR_UNDEF:
        case AST_BINARY_OPERATOR_NUM  :
        default                       :
        {
            LOG_ASSERT_VERBOSE(false, "unknown binary operator");
            return nullptr;
        } break;
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_statement_if_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    // Condition
    llvm::Value *cond = cond_->codegen(CODEGEN_MANAGER);
    if (!cond) return nullptr;
    cond = CODEGEN_MANAGER.builder_.CreateFCmpONE(cond, llvm::ConstantFP::get(CODEGEN_MANAGER.context_, llvm::APFloat(0.0)), "if cond");

    // Basic blocks
    llvm::Function   *func       = CODEGEN_MANAGER.builder_.GetInsertBlock()->getParent();
    llvm::BasicBlock *then_block = llvm::BasicBlock::Create(CODEGEN_MANAGER.context_, "then", func);
    llvm::BasicBlock *cont_block = llvm::BasicBlock::Create(CODEGEN_MANAGER.context_, "if cont");
    llvm::BasicBlock *else_block = nullptr;
    if (!else_.empty())
        else_block = llvm::BasicBlock::Create(CODEGEN_MANAGER.context_, "else");

    // Terminator in cond block
    CODEGEN_MANAGER.builder_.CreateCondBr(cond, then_block, (else_block) ? else_block : cont_block);

    // then_block
    CODEGEN_MANAGER.builder_.SetInsertPoint(then_block);
    CODEGEN_MANAGER.scope_in();

    for (auto &expr : then_)
        if (!expr->codegen(CODEGEN_MANAGER))
            return nullptr;

    CODEGEN_MANAGER.scope_out();
    CODEGEN_MANAGER.builder_.CreateBr(cont_block);

    // BasicBlockList (no insert function)
    llvm::Function::BasicBlockListType &func_block_list = func->getBasicBlockList();

    // else_block
    if (else_block)
    {
        func_block_list.push_back(else_block);

        CODEGEN_MANAGER.builder_.SetInsertPoint(else_block);
        CODEGEN_MANAGER.scope_in();

        for (auto &expr : else_)
            if (!expr->codegen(CODEGEN_MANAGER))
                return nullptr;

        CODEGEN_MANAGER.scope_out();
        CODEGEN_MANAGER.builder_.CreateBr(cont_block);
    }

    // cont_block
    func_block_list.push_back(cont_block);
    CODEGEN_MANAGER.builder_.SetInsertPoint(cont_block);

    return then_block;
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_statement_while_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    // Basic blocks
    llvm::Function   *func       = CODEGEN_MANAGER.builder_.GetInsertBlock()->getParent();
    llvm::BasicBlock *cond_block = llvm::BasicBlock::Create(CODEGEN_MANAGER.context_, "cond", func);
    llvm::BasicBlock *then_block = llvm::BasicBlock::Create(CODEGEN_MANAGER.context_, "then");
    llvm::BasicBlock *cont_block = llvm::BasicBlock::Create(CODEGEN_MANAGER.context_, "if cont");

    // Terminator in block before the cond_block
    CODEGEN_MANAGER.builder_.CreateBr(cond_block);

    // cond_block
    CODEGEN_MANAGER.builder_.SetInsertPoint(cond_block);

    llvm::Value *cond = cond_->codegen(CODEGEN_MANAGER);
    if (!cond) return nullptr;
    cond = CODEGEN_MANAGER.builder_.CreateFCmpONE(cond, llvm::ConstantFP::get(CODEGEN_MANAGER.context_, llvm::APFloat(0.0)), "if cond");

    CODEGEN_MANAGER.builder_.CreateCondBr(cond, then_block, cont_block);

    // BasicBlockList (no insert function)
    llvm::Function::BasicBlockListType &func_block_list = func->getBasicBlockList();

    // then_block
    func_block_list.push_back(then_block);

    CODEGEN_MANAGER.builder_.SetInsertPoint(then_block);
    CODEGEN_MANAGER.scope_in();

    for (auto &expr : then_)
        if (!expr->codegen(CODEGEN_MANAGER))
            return nullptr;

    CODEGEN_MANAGER.scope_out();
    CODEGEN_MANAGER.builder_.CreateBr(cond_block);

    // cont_block
    func_block_list.push_back(cont_block);
    CODEGEN_MANAGER.builder_.SetInsertPoint(cont_block);

    return then_block;
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_statement_var_decl_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    if (CODEGEN_MANAGER.is_global_scope())
    {
        if (CODEGEN_MANAGER.module_->getGlobalVariable(name_))
            translator_error_manager_t::error("redefinition of global variable", src_line_);

        return CODEGEN_MANAGER.module_->getOrInsertGlobal(name_, llvm::Type::getDoubleTy(CODEGEN_MANAGER.context_));
    }

    llvm::AllocaInst *addr = CODEGEN_MANAGER.builder_.CreateAlloca(llvm::Type::getDoubleTy(CODEGEN_MANAGER.context_), nullptr, name_);
    if (!CODEGEN_MANAGER.insert_local(name_, addr))
        translator_error_manager_t::error("redefinition of local variable", src_line_);

    return addr;
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_statement_func_decl_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    if (CODEGEN_MANAGER.module_->getFunction(name_))
        translator_error_manager_t::error("redefinition of function", src_line_);

    std::vector<llvm::Type *> doubles(args_.size(), llvm::Type::getDoubleTy(CODEGEN_MANAGER.context_));
    llvm::FunctionType *func_t = llvm::FunctionType::get(llvm::Type::getDoubleTy(CODEGEN_MANAGER.context_), doubles, false);
    llvm::Function     *func   = llvm::Function::Create(func_t, llvm::Function::InternalLinkage, name_, CODEGEN_MANAGER.module_);

    size_t arg_cnt = 0;
    for (auto &arg : func->args())
        arg.setName(args_[arg_cnt++]->name_);

    llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(CODEGEN_MANAGER.context_, "entry", func);
    CODEGEN_MANAGER.builder_.SetInsertPoint(entry_block);
    CODEGEN_MANAGER.scope_in();

    for (auto &arg : args_)
        if (!arg->codegen(CODEGEN_MANAGER))
            return nullptr;


    for (auto &expr : body_)
        if (!expr->codegen(CODEGEN_MANAGER))
            return nullptr;

    llvm::verifyFunction(*func);
    CODEGEN_MANAGER.scope_out();

    return func;
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_statement_return_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    llvm::Value *value = value_->codegen(CODEGEN_MANAGER);
    if (!value) return nullptr;

    llvm::Value *ret = CODEGEN_MANAGER.builder_.CreateRet(value);

    llvm::BasicBlock *next_block = llvm::BasicBlock::Create(CODEGEN_MANAGER.context_, "", CODEGEN_MANAGER.builder_.GetInsertBlock()->getParent());
    CODEGEN_MANAGER.builder_.SetInsertPoint(next_block);

    return ret;
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_statement_input_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------

llvm::Value *LLVM_AST_statement_output_node::codegen(codegen_manager_t &CODEGEN_MANAGER) const
{
    return nullptr;
}
