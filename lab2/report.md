<h1 align = center> LAB2 实验报告</h1>

<div align = right>SA23225XXX 高XX</div>



## Part A: Introduction to Shell

### **Exercise 1**: If you add a background command after the pipeline command, can the pipeline work?

可以，如图所示，pipeline的各进程（3082、3084、3085、3086）依次向下游传递消息，pipeline的最后一个进程在console打印结果。在此过程中，由于3082号进程运行时间较长，故隔了10s才打印出了结果，期间可以自由地使用终端。

![image-20231009133946903](C:\Users\12624\AppData\Roaming\Typora\typora-user-images\image-20231009133946903.png)



## Part B: Principle

### **Exercise 2**: What would be the result of the program without this line pid = wait((int *) 0)?

```C
	int pid = fork();
    if(pid > 0){
        printf("parent: child=%d\n", pid);
        pid = wait((int *) 0);
        printf("child %d is done\n", pid);
    } else if(pid == 0){
        printf("child: exiting\n");
        exit(0);
    } else {
        printf("fork error\n");
    }
```

去掉`wait`后，父进程无法知晓子进程是否结束。在这个例子中，父子进程均很快退出，故没有什么太大的影响。但如果父进程运行较慢，例如在父进程间插入`sleep(100)`，则子进程会变为僵尸进程（进程6028的状态栏为`defunct`）

![image-20231009141300685](C:\Users\12624\AppData\Roaming\Typora\typora-user-images\image-20231009141300685.png)

僵尸进程是一个早已死亡的进程，但在进程表中仍占了一个位置。如果该进程的父进程已经结束了，那么该进程就不会变成僵尸进程。因为每个进程结束的时候，系统都会扫描当前系统中所运行的所有进程，看看有没有哪个进程是刚刚结束的这个进程的子进程。如果有的话就由Init进程来接管他，成为他的父进程，从而保证每个进程都会有一个父进程。而Init进程会自动`wait`其子进程，因此被Init接管的所有进程都不会变成僵尸进程。

测试程序如下：

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{
    int pid = fork();
    if (pid > 0)
    {
        printf("parent: child=%d\n", pid);
        // pid = wait((int *)0);
        sleep(100);
        printf("child %d is done\n", pid);
    }
    else if (pid == 0)
    {
        printf("child: exiting\n");
        exit(0);
    }
    else
    {
        printf("fork error\n");
    }
}
```



### **Exercise 3**: Can fork and exec be combined in one call?

可以。例如`posix_spawn`函数便实现了`fork`和`exec`的组合功能。

```C
#include <spawn.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
int main()
{
    pid_t pid;
    char *argv[] = {"/bin/echo", "Hello, world!", NULL};
    extern char **environ;

    if (posix_spawn(&pid, "/bin/echo", NULL, NULL, argv, environ) == 0)
    {
        printf("Child process created with PID: %d\n", pid);
        pid = wait((int *)0);
        printf("child %d is done\n", pid);
    }
    else
    {
        perror("posix_spawn failed");
        return 1;
    }

    return 0;
}

```

![image-20231009145613103](C:\Users\12624\AppData\Roaming\Typora\typora-user-images\image-20231009145613103.png)

### **Exercise 4**: How does the above program implement input redirection?

该程序的子程序关闭0号文件描述符（标准输入），打开文件`input.txt`并为其赋予文件描述符0，将从而实现将标准输入重定向到文件。之后的`exec`系统调用不改变打开文件表，故新装载的程序`cat`的标准输入为`input.txt`。

```C
    char *argv[2];
    argv[0] = "cat";
    argv[1] = 0;
    if(fork() == 0) {
        close(0);
        open("input.txt", O_RDONLY);
        exec("cat", argv);
    }
```

### **Exercise 5**: For the two lines `close(0); dup(p[0]);` can we reorder them?

不能，否则`dup(p[0])`会将`p[0]`对应的文件描述符复制到3号文件描述符，而不是0号。`wc`程序也就无法从标准输入(0号文件描述符对应的输入)那里获取正确的输入了。

![image-20231009162457903](C:\Users\12624\AppData\Roaming\Typora\typora-user-images\image-20231009162457903.png)



### **Exercise 6**: What are the advantages of pipes over temporary files in this situation?

以无名管道进行进程间通讯时，消息缓冲区位于内存，两个进程可以以半双工的方式边读边写，效率高。而以临时文件的方式进行通讯时，读进程就必须等写进程执行完毕后才能工作，否则会一直被`read`阻塞，而且硬盘I/O的效率远没有直接访问内存高。

而且使用管道方法代码较为简洁，便于编写复杂命令。



## Part C: Implementation

### **Exercise 7**

实现如下：

```C
Cmd_t Cmd_Back_new(Cmd_t back)
{
    // TODO
    Cmd_Back cmd;
    NEW(cmd);
    cmd->type = CMD_BACK;
    cmd->back = back;
    return (Cmd_t)cmd;
}

Cmd_t Cmd_Pipe_new(Cmd_t left, Cmd_t right)
{
    // TODO
    Cmd_Pipe cmd;
    NEW(cmd);
    cmd->type = CMD_PIPE;
    cmd->left = left;
    cmd->right = right;
    return (Cmd_t)cmd;
}

Cmd_t Cmd_Redir_new(Cmd_t left, Cmd_t right, int fd)
{
    // TODO
    Cmd_Redir cmd;
    NEW(cmd);
    cmd->type = CMD_REDIR;
    cmd->left = left;
    cmd->right = right;
    cmd->fd = fd;
    return (Cmd_t)cmd;
}
```



### Exercise 8

实现如下：

```c
// print AST
void Cmd_print(struct Cmd_t *cmd)
{
    switch (cmd->type)
    {
    case CMD_ATOM:
    {
        struct Cmd_Atom *t = (struct Cmd_Atom *)cmd;
        struct node *node = t->node;
        while (node)
        {
            printf("%s ", node->data);
            node = node->next;
        }
        break;
    }

    case CMD_SEQ:
    {
        printf("enter\n");
        struct Cmd_Seq *t = (struct Cmd_Seq *)cmd;
        printf("enter\n");
        Cmd_t left = t->left;
        Cmd_t right = t->right;

        Cmd_print(left);
        printf("; ");
        Cmd_print(right);
        break;
    }

    case CMD_BACK:
    {
        // TODO
        struct Cmd_Back *t = (struct Cmd_Back *)cmd;
        Cmd_t back = t->back;
        Cmd_print(back);
        printf("& ");
        break;
    }

    case CMD_PIPE:
    {
        // TODO
        struct Cmd_Pipe *t = (struct Cmd_Pipe *)cmd;
        Cmd_t left = t->left;
        Cmd_t right = t->right;
        Cmd_print(left);
        printf("| ");
        Cmd_print(right);
        break;
    }

    case CMD_REDIR:
    {
        // TODO
        struct Cmd_Redir *t = (struct Cmd_Redir *)cmd;
        Cmd_t left = t->left;
        Cmd_t right = t->right;
        int fd = t->fd;
        Cmd_print(left);
        switch (fd)
        {
        case 0:
            printf("< ");
            break;
        case 1:
            printf("> ");
            break;
        }

        Cmd_print(right);
        break;
    }
    default:
        break;
    }
}
```

随后我们修改`scanner.l`的`ID`正则表达式为`[/a-zA-Z0-9._-]+`使之支持文件路径，并添加输入重定向token`<`: `">"|"<" {return yytext[0];}`。同理，修改`parser.y`的语法，将`<`的优先级设为与`>`相同`%left '>' '<'`，并新加一条转移式和对应语义`command : command '<' command { $$ = Cmd_Redir_new($1, $3, 0);}`，并将`main()`中的`Cmd_run`改为`Cmd_print`。

实验结果如下，可以很好地支持各种shell语句：

![image-20231009205740418](C:\Users\12624\AppData\Roaming\Typora\typora-user-images\image-20231009205740418.png)

### Exercise 9

实现如下：

```C
// run cmd
void Cmd_run(struct Cmd_t *cmd)
{
    switch (cmd->type)
    {
    case CMD_ATOM:
    {
        char *arg[10] = {0};
        int i = 0;
        struct Cmd_Atom *t = (struct Cmd_Atom *)cmd;
        struct node *node = t->node;
        while (node)
        {
            arg[i] = node->data;
            node = node->next;
            i++;
        }
        char *root = "/bin/";
        char binPath[50];
        strcpy(binPath, root);
        strcat(binPath, arg[0]);

        if (execv(binPath, arg) == -1)
        {
            char *path = "/usr/bin/";
            char usrBinPath[50];
            strcpy(usrBinPath, path);
            strcat(usrBinPath, arg[0]);
            if (execv(usrBinPath, arg) == -1)
            {
                fprintf(stderr, "cannot run command, check your input.\n");
            }
        }
        break;
    }

    case CMD_SEQ:
    {
        struct Cmd_Seq *t = (struct Cmd_Seq *)cmd;
        Cmd_t left = t->left;
        Cmd_t right = t->right;

        if (fork() == 0)
        {
            Cmd_run(left);
        }
        else
        {
            wait(0);
            Cmd_run(right);
        }

        break;
    }

    case CMD_BACK:
    {
        // TODO
        struct Cmd_Back *t = (struct Cmd_Back *)cmd;
        Cmd_t back = t->back;
        int pid = fork();
        if (pid == 0)
        {
            Cmd_run(back);
        }
        // wait(0); // Don't wait for child process
        break;
    }

    case CMD_PIPE:
    {
        // TODO
        struct Cmd_Pipe *t = (struct Cmd_Pipe *)cmd;
        Cmd_t left = t->left;
        Cmd_t right = t->right;
        int fd[2];
        pipe(fd);
        if (fork() == 0)
        {
            close(fd[0]);
            dup2(fd[1], 1);
            close(fd[1]);
            Cmd_run(left);
            close(1);
        }
        else
        {
            close(fd[1]);
            dup2(fd[0], 0);
            close(fd[0]);
            Cmd_run(right);
            close(0);
            wait(0);
        }
        break;
    }

    case CMD_REDIR:
    {
        // TODO
        struct Cmd_Redir *t = (struct Cmd_Redir *)cmd;
        Cmd_t left = t->left;
        Cmd_Atom right = (struct Cmd_Atom *)t->right;
        const char *file = right->node->data;
        switch (t->fd)
        {
        case 0: // <
            if (fork() == 0)
            {
                int fd = open(file, O_RDONLY);
                dup2(fd, 0);
                Cmd_run(left);
            }
            else
                wait(0);
            break;
        case 1: // >
            if (fork() == 0)
            {
                int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                dup2(fd, 1);
                Cmd_run(left);
            }
            else
                wait(0);
            break;
        default:
            break;
        }
    }

    default:
        break;
    }

    exit(0);
}
```



实验结果如下，可以很好地支持各种shell语句：

`cat hello.txt > c.txt `

![image-20231009220938119](C:\Users\12624\AppData\Roaming\Typora\typora-user-images\image-20231009220938119.png)



`sleep 10 &`立刻返回终端，`sleep 10`则会阻塞终端

![image-20231009221231987](C:\Users\12624\AppData\Roaming\Typora\typora-user-images\image-20231009221231987.png)

![image-20231009221308859](C:\Users\12624\AppData\Roaming\Typora\typora-user-images\image-20231009221308859.png)



`cat hello.txt | grep -n hello>c.txt; cat c.txt; ls`

![image-20231009221216388](C:\Users\12624\AppData\Roaming\Typora\typora-user-images\image-20231009221216388.png)



输入重定向、带有'/'的文件路径也可以支持：`echo hello world >/tmp/xyz; wc </tmp/xyz`

![image-20231009223818034](C:\Users\12624\AppData\Roaming\Typora\typora-user-images\image-20231009223818034.png)



较为复杂的情形也能支持

```shell
sleep 5; ls | grep i & 
ls
```

![image-20231009224312538](C:\Users\12624\AppData\Roaming\Typora\typora-user-images\image-20231009224312538.png)