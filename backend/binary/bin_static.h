#ifndef BIN_STATIC_H
#define BIN_STATIC_H

#include <stdio.h>
#include <stdlib.h>

#include "../../lib/algorithm/algorithm.h"

#include "bin.h"

//================================================================================================================================
// BINARY
//================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

static void binary_node_header_dump     (const binary_node *const node) __attribute__((always_inline));
static void binary_node_ending_dump     ()                              __attribute__((always_inline));
static void binary_node_fields_dump     (const binary_node *const node);

static void binary_node_bool_fields_dump(const binary_node *const node);
static void binary_node_REX_dump        (const binary_node *const node);
static void binary_node_ModRM_dump      (const binary_node *const node);
static void binary_node_SIB_dump        (const binary_node *const node);

#endif //BIN_STATIC_H
