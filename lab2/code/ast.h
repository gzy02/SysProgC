#ifndef AST_H
#define AST_H

typedef struct node *node;
struct node{
    char *data;
    struct node *next;
};
struct node *make_node(char *data, struct node *next);

enum Cmd_type_t {
    CMD_ATOM,
    CMD_BACK,
    CMD_PIPE,
    CMD_REDIR,
    CMD_SEQ
};

typedef struct Cmd_t *Cmd_t;
struct Cmd_t{
    enum Cmd_type_t type;
};

// basic cmd
typedef struct Cmd_Atom *Cmd_Atom;
struct Cmd_Atom{
    enum Cmd_type_t type;
    struct node *node;
};
Cmd_t Cmd_Atom_new(struct node *);

// list cmd
typedef struct Cmd_Seq *Cmd_Seq;
struct Cmd_Seq{
    enum Cmd_type_t type;
    Cmd_t left;
    Cmd_t right;
};
Cmd_t Cmd_Seq_new(Cmd_t left, Cmd_t right);

// back cmd
typedef struct Cmd_Back *Cmd_Back;
struct Cmd_Back{
    enum Cmd_type_t type;
    Cmd_t back;
};
Cmd_t Cmd_Back_new(Cmd_t back);

// pipe cmd
typedef struct Cmd_Pipe *Cmd_Pipe;
struct Cmd_Pipe{
    enum Cmd_type_t type;
    Cmd_t left;
    Cmd_t right;
};
Cmd_t Cmd_Pipe_new(Cmd_t left, Cmd_t right);

// redir cmd
typedef struct Cmd_Redir *Cmd_Redir;
struct Cmd_Redir{
    enum Cmd_type_t type;
    Cmd_t left;
    Cmd_t right;
    int fd;
};
Cmd_t Cmd_Redir_new(Cmd_t left, Cmd_t right, int fd);

// print AST
void Cmd_print(Cmd_t cmd);

// read and run cmd
void Cmd_run(Cmd_t cmd);

#endif
