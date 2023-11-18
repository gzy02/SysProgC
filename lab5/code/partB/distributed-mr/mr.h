#ifndef MR_H
#define MR_H

char **map_phase(char *map_name, int reduce_size);
void reduce_phase(char *reduce_name, char **input_files, int file_size);

#endif