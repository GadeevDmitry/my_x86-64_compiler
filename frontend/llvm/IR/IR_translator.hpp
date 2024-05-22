#ifndef LLVM_IR_HPP
#define LLVM_IR_HPP

#include "ast.hpp"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"

//================================================================================================================================

std::unique_ptr<llvm::Module> module_translator(LLVM_AST_module_node *AST_module);

#endif // LLVM_IR_HPP
