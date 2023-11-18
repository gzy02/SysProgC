#ifndef KEY_VALUE_H
#define KEY_VALUE_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)





typedef struct Kv_t *kv_t;
struct Kv_t{
    char *key;
    char *value;
};

kv_t kv_new(char *key, char *value);
void kv_print(kv_t);
void kv_free(kv_t kv);


#endif







