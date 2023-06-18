#include "ast_static.h"

//================================================================================================================================
// AST_NODE
//================================================================================================================================

#define $type   (node->type)

#define $L      (node-> left)
#define $R      (node->right)
#define $P      (node-> prev)

#define $imm    (node->value.imm_int)
#define $var    (node->value.var_ind)
#define $func   (node->value.func_ind)
#define $op     (node->value.op)

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

static bool AST_node_set_value(AST_node *const node, const AST_NODE_TYPE type, va_list value)
{
    log_assert(node != nullptr);

    switch (type)
    {
        case AST_NODE_IMM_INT  : $imm  = va_arg(value, int);    break;

        case AST_NODE_VARIABLE :
        case AST_NODE_DECL_VAR : $var  = va_arg(value, size_t); break;

        case AST_NODE_DECL_FUNC:
        case AST_NODE_CALL_FUNC: $func = va_arg(value, size_t); break;

        case AST_NODE_OPERATOR : $op   = (AST_OPERATOR_TYPE) va_arg(value, int); break;

        case AST_NODE_FICTIONAL         :
        case AST_NODE_OPERATOR_IF       :
        case AST_NODE_OPERATOR_THEN_ELSE:
        case AST_NODE_OPERATOR_WHILE    :
        case AST_NODE_OPERATOR_RETURN   :

        default: break;
    }

    va_end(value);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline bool AST_node_ctor(AST_node *const node, const AST_NODE_TYPE type, va_list value)
{
    log_assert(node != nullptr);

    $type = type;

    $L = nullptr;
    $R = nullptr;
    $P = nullptr;

    return AST_node_set_value(node, type, value);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool AST_node_ctor(AST_node *const node, const AST_NODE_TYPE type, ...)
{
    log_verify(node != nullptr, false);

    va_list  value;
    va_start(value, type);

    return AST_node_ctor(node, type, value);
}

//--------------------------------------------------------------------------------------------------------------------------------

AST_node *AST_node_new(const AST_NODE_TYPE type, ...)
{
    AST_node  *node_new = (AST_node *) log_calloc(1, sizeof(AST_node));
    log_verify(node_new != nullptr, nullptr);

    va_list  value;
    va_start(value, type);

    if (!AST_node_ctor(node_new, type, value)) { log_free(node_new); return nullptr; }
    return node_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool AST_node_set_value(AST_node *const node, const AST_NODE_TYPE type, ...)
{
    log_verify(node != nullptr, false);

    va_list  value;
    va_start(value, type);

    return AST_node_set_value(node, type, value);
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

void AST_node_delete(AST_node *const node)
{
    log_free(node);
}

//--------------------------------------------------------------------------------------------------------------------------------

void AST_tree_delete(AST_node *const node)
{
    if (node == nullptr) return;

    AST_tree_delete($L);
    AST_tree_delete($R);
    AST_node_delete(node);
}

//--------------------------------------------------------------------------------------------------------------------------------
// hang
//--------------------------------------------------------------------------------------------------------------------------------

bool AST_node_hang_left(AST_node *const tree, AST_node *const node)
{
    if (tree != nullptr) tree->left = node;
    if (node != nullptr) node->prev = tree;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool AST_node_hang_right(AST_node *const tree, AST_node *const node)
{
    if (tree != nullptr) tree->right = node;
    if (node != nullptr) node->prev  = tree;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

AST_node *AST_tree_hang_by_adapter(AST_node *const tree, AST_node *const node)
{
    log_verify(tree != nullptr, nullptr);

    if (tree->left  == nullptr) { AST_node_hang_left(tree, node); return tree; }
    if (tree->right == nullptr)
    {
        AST_node *adapter = AST_node_new(AST_NODE_FICTIONAL);

        AST_node_hang_right(tree, adapter);
        AST_node_hang_left (adapter, node);

        return adapter;
    }

    return AST_tree_hang_by_adapter(tree->right, node);
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void AST_tree_graphviz_dump(const void *const _node)
{
    const  AST_node *const node = (const AST_node *) _node;
    log_verify(node != nullptr, (void) 0);

    AST_tree_static_dump(node);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void AST_tree_static_dump(const AST_node *const node)
{
    log_assert(node != nullptr);

    static size_t dump_num = 0;

    char dump_filename_txt[100] = "";
    char dump_filename_png[100] = "";

    system("mkdir -p ast/graphviz_dump_txt");
    system("mkdir -p ast/graphviz_dump_png");

    sprintf(dump_filename_txt, "ast/graphviz_dump_txt/ast_%.5lu.txt", dump_num);
    sprintf(dump_filename_png, "ast/graphviz_dump_png/ast_%.5lu.png", dump_num);

    FILE *stream = fopen(dump_filename_txt, "w");
    log_verify(stream != nullptr,  (void) 0);
    setvbuf   (stream,   nullptr, _IONBF, 0);

    dump_num++;

    AST_tree_header_dump (stream);
    AST_tree_content_dump(node, 0, stream);
    AST_tree_ending_dump (dump_filename_txt, dump_filename_png, stream);

    fclose(stream);
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline void AST_tree_header_dump(FILE *const stream)
{
    log_assert(stream != nullptr);

    fprintf(stream, "digraph {\n"
                    "splines=ortho\n"
                    "node[style=\"rounded, filled\", fontsize=8]\n");
}

//--------------------------------------------------------------------------------------------------------------------------------

static size_t AST_tree_content_dump(const AST_node *const node, const size_t node_index, FILE *const stream)
{
    log_assert(node   != nullptr);
    log_assert(stream != nullptr);

    AST_node_content_dump(node, node_index, stream);

    size_t index_next = node_index + 1;
    size_t index_l    = -1UL;
    size_t index_r    = -1UL;

    if ($L != nullptr) { index_l = index_next; index_next = AST_tree_content_dump($L, index_l, stream); AST_edge_dump(node_index, index_l, stream); }
    if ($R != nullptr) { index_r = index_next; index_next = AST_tree_content_dump($R, index_r, stream); AST_edge_dump(node_index, index_r, stream); }

    return index_next;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void AST_tree_ending_dump(const char *txt_file, const char *png_file, FILE *const stream)
{
    log_assert(txt_file != nullptr);
    log_assert(png_file != nullptr);
    log_assert(stream   != nullptr);

    fprintf(stream, "}\n");

    char    dump_system_cmd[300] = "";
    sprintf(dump_system_cmd, "dot %s -T png -o %s", txt_file, png_file);
    system (dump_system_cmd);

    log_message("<img src=%s>\n", png_file);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void AST_node_content_dump(const AST_node *const node, const size_t node_index, FILE *const stream)
{
    log_assert(node   != nullptr);
    log_assert(stream != nullptr);

    GRAPHVIZ_COLOR color      = GRAPHVIZ_COLOR_BLACK;
    GRAPHVIZ_COLOR color_fill = GRAPHVIZ_COLOR_BLACK;
    AST_node_get_dump_color(node, &color, &color_fill);

    char value_dump[100] = "";
    AST_node_get_dump_value(node, value_dump);

    fprintf(stream, "node%lu[color=\"%s\", fillcolor=\"%s\", label=\"{type: %s | %s }\"]\n",
                         node_index,
                                     GRAPHVIZ_COLOR_NAMES[color],
                                                       GRAPHVIZ_COLOR_NAMES[color_fill],
                                                                            AST_NODE_TYPE_NAMES[$type],
                                                                                 value_dump);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void AST_node_get_dump_color(const AST_node *const node, GRAPHVIZ_COLOR *const color,
                                                                GRAPHVIZ_COLOR *const color_fill)
{
    log_assert(node       != nullptr);
    log_assert(color      != nullptr);
    log_assert(color_fill != nullptr);

    switch ($type)
    {
        case AST_NODE_FICTIONAL : *color      = GRAPHVIZ_COLOR_BLACK;
                                  *color_fill = GRAPHVIZ_COLOR_LIGHT_GREY;
                                  return;

        case AST_NODE_VARIABLE  :
        case AST_NODE_IMM_INT   : *color      = GRAPHVIZ_COLOR_DARK_BLUE;
                                  *color_fill = GRAPHVIZ_COLOR_LIGHT_BLUE;
                                  return;

        case AST_NODE_CALL_FUNC         :
        case AST_NODE_OPERATOR_IF       :
        case AST_NODE_OPERATOR_WHILE    :
        case AST_NODE_OPERATOR_RETURN   :
        case AST_NODE_OPERATOR_THEN_ELSE: *color      = GRAPHVIZ_COLOR_DARK_ORANGE;
                                          *color_fill = GRAPHVIZ_COLOR_LIGHT_ORANGE;
                                          return;

        case AST_NODE_OPERATOR          : *color      = GRAPHVIZ_COLOR_DARK_GREEN;
                                          *color_fill = GRAPHVIZ_COLOR_LIGHT_GREEN;
                                          return;

        case AST_NODE_DECL_VAR  :
        case AST_NODE_DECL_FUNC : *color      = GRAPHVIZ_COLOR_LIGHT_PINK;
                                  *color_fill = GRAPHVIZ_COLOR_DARK_PINK;
        default:                  return;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static void AST_node_get_dump_value(const AST_node *const node, char *const value_dump)
{
    log_assert(node       != nullptr);
    log_assert(value_dump != nullptr);

    switch ($type)
    {
        case AST_NODE_DECL_VAR :
        case AST_NODE_VARIABLE : sprintf(value_dump, "var_index: %lu", $var);
                                 return;

        case AST_NODE_DECL_FUNC:
        case AST_NODE_CALL_FUNC: sprintf(value_dump, "func_index: %lu", $func);
                                 return;

        case AST_NODE_IMM_INT  : sprintf(value_dump, "imm_int: %d", $imm);
                                 return;

        case AST_NODE_OPERATOR : sprintf(value_dump, "operator: %s", AST_OPERATOR_NAMES[$op]);
                                 return;
        
        case AST_NODE_FICTIONAL         :
        case AST_NODE_OPERATOR_IF       :
        case AST_NODE_OPERATOR_WHILE    :
        case AST_NODE_OPERATOR_RETURN   :
        case AST_NODE_OPERATOR_THEN_ELSE: sprintf(value_dump, "-");
        default                         : return;
    }

    return;
}

//--------------------------------------------------------------------------------------------------------------------------------

static inline void AST_edge_dump(const size_t src, const size_t dst, FILE *const stream)
{
    log_assert(stream != nullptr);

    fprintf(stream, "node%lu->node%lu[color=\"black\"]\n", src, dst);
}
