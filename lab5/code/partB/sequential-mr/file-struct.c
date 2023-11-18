// file - struct.c
#include "file-struct.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void remove_dir(const char *dir_path)
{
    DIR *dir = opendir(dir_path);
    if (dir)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                char path[512];
                sprintf(path, "%s/%s", dir_path, entry->d_name);
                struct stat st;
                if (stat(path, &st) == -1)
                {
                    perror("stat");
                    exit(EXIT_FAILURE);
                }
                if (S_ISDIR(st.st_mode))
                {
                    remove_dir(path);
                }
                else
                {
                    if (remove(path) == -1)
                    {
                        perror("remove");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
        closedir(dir);
        if (rmdir(dir_path) == -1)
        {
            perror("rmdir");
            exit(EXIT_FAILURE);
        }
    }
}

void create_files(fnode *node, const char *base_fname, int nOutputs, char **file_paths)
{
    char dir_path[256];
    sprintf(dir_path, "/tmp/%s", base_fname);

    struct stat st;
    if (stat(dir_path, &st) == 0)
    {
        // 如果存在，则删除文件夹和其中的内容
        remove_dir(dir_path);
    }
    if (mkdir(dir_path, 0777) == -1)
    {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i <= nOutputs; i++)
    {
        char file_path[512];
        sprintf(file_path, "%s/%d.txt", dir_path, i);
        FILE *fp = fopen(file_path, "w");
        if (fp == NULL)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        fclose(fp);
        file_paths[i - 1] = strdup(file_path);
    }

    // struct stat st = {0};
    remove_dir(node->output_dir);
    if (stat(node->output_dir, &st) == -1)
    {
        if (mkdir(node->output_dir, 0777) == -1)
        {
            perror("mkdir");
            exit(EXIT_FAILURE);
        }
    }
}

fnode *init_fnode(int nInput, const char *argv[], int nOutputs, const char *tmp_dir, const char *output_dir)
{
    fnode *node = malloc(sizeof(fnode));
    if (node == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    node->files = malloc(nInput * sizeof(file_list));
    if (node->files == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    node->num_files = nInput;
    node->tmp_dir = tmp_dir;
    node->output_dir = output_dir;

    for (int i = 0; i < nInput; i++)
    {
        node->files[i].fname = strdup(argv[i + 1]);
        node->files[i].inter = malloc(sizeof(inter_res));
        if (node->files[i].inter == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        node->files[i].inter->file_paths = malloc(nOutputs * sizeof(char *));
        if (node->files[i].inter->file_paths == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        node->files[i].inter->num_fps = nOutputs;
        create_files(node, argv[i + 1], nOutputs, node->files[i].inter->file_paths);
    }
    return node;
}

void free_fnode(fnode *node, int nOutputs)
{
    for (int i = 0; i < node->num_files; i++)
    {
        if (node->files[i].inter != NULL)
        {
            char tmp_dir[256];
            snprintf(tmp_dir, sizeof(tmp_dir), "%s/%s", node->tmp_dir, node->files[i].fname);
            remove_dir(tmp_dir);
            free(node->files[i].inter->file_paths);
            free(node->files[i].inter);
        }
        free(node->files[i].fname);
    }
    free(node->files);
    free(node);
}
