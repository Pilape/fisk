#include <stdio.h>
#include <stdlib.h>

#include "lib/CuTest.h"
#include <stdio.h>
#include "lib/CuTest.h"

CuSuite* ScannerGetSuite();

int main(int argc, char* argv[])
{
    CuString* output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, ScannerGetSuite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);

    return 0;
}
