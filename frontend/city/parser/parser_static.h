#ifndef PARSER_STATIC_H
#define PARSER_STATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LOG_NTRACE
#define LOG_NLEAK

#define NDEBUG
#define NVERIFY

#include "lib/include/log.h"
#include "lib/include/algorithm.h"
#include "lib/include/stack.h"
#include "lib/include/vector.h"

#include "frontend/tokenizer/tokenizer.h"

#include "structs/structs.h"
#include "parser.h"

//================================================================================================================================
// PARSER
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// GLOBAL
//--------------------------------------------------------------------------------------------------------------------------------

static const char MAIN_FUNC_NAME[] = "MAN_CITY";

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
static bool parse_name_access_dependent     (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
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

static bool parse_is_assignment             (                       token_arr_pass *const tkn_pass                          );
static bool parse_assignment_half           (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool   parse_op_pattern              (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree ,
       bool (*parse_op_lower)               (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree),
       bool (*parse_op_token)               (                       token_arr_pass *const tkn_pass, AST_node **const subtree));

static bool parse_log_or                    (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree) __attribute__((always_inline));
static bool parse_log_and                   (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree) __attribute__((always_inline));
static bool parse_equal                     (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree) __attribute__((always_inline));
static bool parse_cmp                       (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree) __attribute__((always_inline));
static bool parse_add_sub                   (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree) __attribute__((always_inline));
static bool parse_mul_div                   (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree) __attribute__((always_inline));
static bool parse_not                       (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_sqrt                      (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool parse_log_or_token              (                       token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_log_and_token             (                       token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_equal_token               (                       token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_cmp_token                 (                       token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_add_sub_token             (                       token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_mul_div_token             (                       token_arr_pass *const tkn_pass, AST_node **const subtree);

static bool parse_operand                   (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_expression                (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);
static bool parse_imm_int                   (prog_info *const prog, token_arr_pass *const tkn_pass, AST_node **const subtree);

#endif //PARSER_STATIC_H
