#include <stdio.h>
#include <stdlib.h>

#include "frontend.hpp"

int main(const int argc, const char *argv[])
{
    if (argc != 2) { fprintf(stderr, "./jit usage: ./jit <source code filename>\n"); return 0; }

    frontend(argv[1]);
}
