#ifndef LLVM_PARSER_HPP
#define LLVM_PARSER_HPP

#include "ast.hpp"
#include "lib/include/vector.h"

//================================================================================================================================

std::unique_ptr<LLVM_AST_module_node> module_parser(const char *module_name, vector *tokens);

#endif // LLVM_PARSER_HPP
