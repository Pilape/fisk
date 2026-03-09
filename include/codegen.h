#ifndef __CODEGEN_HEADER__
#define __CODEGEN_HEADER__

#include "../lang.h"
#include "scanner.h"

void GenerateBytecode(tokenlist_t token_list, langctx_t* context);

#endif
