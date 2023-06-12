#ifndef PARSER_STATIC_H
#define PARSER_STATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LOG_NTRACE
#define LOG_NLEAK

#include "../../lib/logs/log.h"
#include "../../lib/algorithm/algorithm.h"
#include "../../lib/stack/stack.h"
#include "../../lib/vector/vector.h"

#include "../tokenizer/tokenizer.h"

#include "structs/structs.h"
#include "parser.h"

//================================================================================================================================
// PARSER
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// GLOBAL
//--------------------------------------------------------------------------------------------------------------------------------

static const char MAIN_FUNC_NAME[] = "CAMP_NOU";

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION
//--------------------------------------------------------------------------------------------------------------------------------

static bool parse_general                   (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_name_decl                 (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool parse_var_decl_independent      (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_var_decl                  (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool parse_func_decl                 (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_args                      (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree, func_info *const          func);
static bool parse_args_only                 (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree, size_t    *const args_quantity);
static bool parse_func_body                 (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool parse_operators                 (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_name_access_independent   (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_name_access               (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_syntactic_unit            (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool parse_assignment_independent    (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_assignment                (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool parse_func_call_independent     (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_func_call                 (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_params                    (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree, func_info *const            func);
static bool parse_params_only               (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree, size_t    *const params_quantity);

static bool parse_input                     (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_output                    (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool parse_scope                     (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_condition                 (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool parse_if                        (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_then_else                 (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_else                      (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool parse_while                     (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_return                    (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool parse_lvalue                    (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_rvalue                    (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

#endif //PARSER_STATIC_H
