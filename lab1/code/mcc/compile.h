
#ifndef COMPILE_H
#define COMPILE_H

#include "ast.h"

void compile(Stm_t prog, int fd);
void riscv_compile(Stm_t prog, int fd);

#endif
