#include <stdio.h>
#include <stdlib.h>
#define __LANG_IMPLEMENTATION__
#include "lang.h"

int main(int argc, char* argv[])
{
    Lang_Eval("{ square (i -- i*i)@dup * }\n\n #5 square print (Multiline \n comment \n yay) #25 square print");

    return 0;
}
