#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LOG_NTRACE

#include "../lib/logs/log.h"
#include "../lib/algorithm/algorithm.h"
#include "../lib/stack/stack.h"
#include "../lib/vector/vector.h"

//================================================================================================================================
// GLOBAL
//================================================================================================================================

enum ANALYSIS
{
    ANALYSIS_SUCCESS,
    ANALYSIS_ERROR  ,
    ANALYSIS_FAIL   ,
};

#endif //COMMON_H
