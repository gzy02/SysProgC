#include "ast.h"
#include "alloc.h"
#include <stdio.h>
#include <string.h>

void Exp_print(Exp_t exp)
{
    switch (exp->type) {
    case EXP_NUM: {
        Exp_Num e = (Exp_Num)exp;
        printf("%d", e->num);
        break;
    }
    case EXP_VAR: {
        Exp_Var e = (Exp_Var)exp;
        printf("%s", e->name);
        break;
    }
    case EXP_ADD: {
        Exp_Add e = (Exp_Add)exp;
        printf("(");
        Exp_print(e->left);
        printf(" + ");
        Exp_print(e->right);
        printf(")");
        break;
    }
    case EXP_SUB: {
        Exp_Sub e = (Exp_Sub)exp;
        printf("(");
        Exp_print(e->left);
        printf(" - ");
        Exp_print(e->right);
        printf(")");
        break;
    }
    case EXP_TIMES: {
        Exp_Times e = (Exp_Times)exp;
        printf("(");
        Exp_print(e->left);
        printf(" * ");
        Exp_print(e->right);
        printf(")");
        break;
    }
    case EXP_DIV: {
        Exp_Div e = (Exp_Div)exp;
        printf("(");
        Exp_print(e->left);
        printf(" / ");
        Exp_print(e->right);
        printf(")");
        break;
    }
    default:
        printf("Error: unsupported expression type\n");
        break;
    }
}

Exp_t Exp_Num_new(int num) {
    Exp_Num exp;

    NEW(exp);
    exp->type = EXP_NUM;
    exp->num = num;
    return (Exp_t)exp;
}

Exp_t Exp_Var_new(char *name) {
    Exp_Var exp;

    NEW(exp);
    exp->type = EXP_VAR;
    exp->name = strdup(name);
    return (Exp_t)exp;
}

Exp_t Exp_Add_new(Exp_t left, Exp_t right) {
    Exp_Add exp;

    NEW(exp);
    exp->type = EXP_ADD;
    exp->left = left;
    exp->right = right;
    return (Exp_t)exp;
}

Exp_t Exp_Sub_new(Exp_t left, Exp_t right) {
    Exp_Sub exp;

    NEW(exp);
    exp->type = EXP_SUB;
    exp->left = left;
    exp->right = right;
    return (Exp_t)exp;
}

Exp_t Exp_Times_new(Exp_t left, Exp_t right) {
    Exp_Times exp;

    NEW(exp);
    exp->type = EXP_TIMES;
    exp->left = left;
    exp->right = right;
    return (Exp_t)exp;
}

Exp_t Exp_Div_new(Exp_t left, Exp_t right) {
    Exp_Div exp;

    NEW(exp);
    exp->type = EXP_DIV;
    exp->left = left;
    exp->right = right;
    return (Exp_t)exp;
}


Stm_t Stm_Assign_new(char *x, Exp_t exp) {
    Stm_Assign stm;

    NEW(stm);
    stm->type = STM_ASSIGN;
    stm->x = x;
    stm->exp = exp;
    return (Stm_t)stm;
}

Stm_t Stm_Print_new(Exp_t exp) {
    Stm_Print stm;

    NEW(stm);
    stm->type = STM_PRINT;
    stm->exp = exp;
    return (Stm_t)stm;
}

Stm_t Stm_Seq_new(Stm_t left, Stm_t right) {
    Stm_Seq stm;

    NEW(stm);
    stm->type = CMD_SEQ;
    stm->left = left;
    stm->right = right;
    return (Stm_t)stm;
}

void Stm_print(Stm_t stm) {
    switch (stm->type) {
        case STM_ASSIGN: {
            Stm_Assign s = (Stm_Assign)stm;
            printf("%s = ", s->x);
            Exp_print(s->exp);
            printf(";\n");
            break;
        }
        case STM_PRINT: {
            Stm_Print s = (Stm_Print)stm;
            printf("print(");
            Exp_print(s->exp);
            printf(");\n");
            break;
        }
        case CMD_SEQ: {
            Stm_Seq s = (Stm_Seq)stm;
            Stm_print(s->left);
            Stm_print(s->right);
            break;
        }
        default:
            printf("Error: unsupported statement type\n");
            break;
    }
}

