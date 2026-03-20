#include <stdio.h>
#include <stdlib.h>
#define __LANG_IMPLEMENTATION__
#include "lang.h"

int main(int argc, char* argv[])
{
    Lang_Eval("{ square @dup * }");

    return 0;
}
