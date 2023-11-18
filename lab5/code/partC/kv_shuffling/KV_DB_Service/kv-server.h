#ifndef KV_SERVER_H
#define KV_SERVER_H
// 将hash值在[l,r]范围的键值对序列化返回
char *delete_kv_range(int l, int r);
char *get_all_kv();
void put(char *key, char *value);
char *get(char *key);
void delete(char *key);
#endif
