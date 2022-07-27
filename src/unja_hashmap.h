#pragma once
#define HASHMAP_CAP 26

struct unja_hashmap {
    struct node *buckets[HASHMAP_CAP];
};

struct unja_hashmap *unja_hashmap_new();
void *unja_hashmap_insert(struct unja_hashmap *hm, char *key, void *value);
void *unja_hashmap_get(struct unja_hashmap *hm, char *key);
void *unja_hashmap_resolve(struct unja_hashmap *hm, char *key);
void *unja_hashmap_remove(struct unja_hashmap *hm, char *key);
void unja_hashmap_free(struct unja_hashmap *hm);
void unja_hashmap_walk(struct unja_hashmap *hm, void (*fn)(void *value));
