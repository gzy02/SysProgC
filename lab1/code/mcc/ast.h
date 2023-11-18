#ifndef AST_H
#define AST_H

// expression
enum Exp_Type_t {
    EXP_NUM,
    EXP_VAR,
    EXP_ADD,
    EXP_SUB,
    EXP_TIMES,
    EXP_DIV
};

typedef struct Exp_t *Exp_t;
struct Exp_t {
    enum Exp_Type_t type;
};
void Exp_print(Exp_t);

typedef struct Exp_Num *Exp_Num;
struct Exp_Num {
    enum Exp_Type_t type;
    int num;
};
Exp_t Exp_Num_new(int num);

typedef struct Exp_Var *Exp_Var;
struct Exp_Var {
    enum Exp_Type_t type;
    char *name;
};
Exp_t Exp_Var_new(char *name);

typedef struct Exp_Add *Exp_Add;
struct Exp_Add {
    enum Exp_Type_t type;
    Exp_t left;
    Exp_t right;
};
Exp_t Exp_Add_new(Exp_t left, Exp_t right);

typedef struct Exp_Sub *Exp_Sub;
struct Exp_Sub {
    enum Exp_Type_t type;
    Exp_t left;
    Exp_t right;
};
Exp_t Exp_Sub_new(Exp_t left, Exp_t right);

typedef struct Exp_Times *Exp_Times;
struct Exp_Times {
    enum Exp_Type_t type;
    Exp_t left;
    Exp_t right;
};
Exp_t Exp_Times_new(Exp_t left, Exp_t right);

typedef struct Exp_Div *Exp_Div;
struct Exp_Div {
    enum Exp_Type_t type;
    Exp_t left;
    Exp_t right;
};
Exp_t Exp_Div_new(Exp_t left, Exp_t right);


// statement
enum Stm_Type_t {
    STM_ASSIGN,
    STM_PRINT,
    CMD_SEQ
};
typedef struct Stm_t *Stm_t;
struct Stm_t {
    enum Stm_Type_t type;
};
void Stm_print(Stm_t stm);

typedef struct Exp_t *Exp_t;
typedef struct Stm_Assign *Stm_Assign;
struct Stm_Assign {
    enum Stm_Type_t type;
    char *x;
    Exp_t exp;
};
Stm_t Stm_Assign_new(char *x, Exp_t exp);

typedef struct Stm_Print *Stm_Print;
struct Stm_Print {
    enum Stm_Type_t type;
    Exp_t exp;
};
Stm_t Stm_Print_new(Exp_t exp);

typedef struct Stm_Seq *Stm_Seq;
struct Stm_Seq {
    enum Stm_Type_t type;
    Stm_t left;
    Stm_t right;
};
Stm_t Stm_Seq_new(Stm_t left, Stm_t right);


#endif


