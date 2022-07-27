#include <string.h>
#include <stdlib.h>
#include <err.h>
#include "unja_hashmap.h"

struct node {
    char *key;
    void *value;
    struct node *next;
};

unsigned long
hash(char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

/* allocate a new unja_hashmap */
struct unja_hashmap *unja_hashmap_new() {
    struct unja_hashmap *hm = malloc(sizeof *hm);
    if (!hm) err(EXIT_FAILURE, "out of memory");
    for (int i=0; i < HASHMAP_CAP; i++) {
        hm->buckets[i] = NULL;
    }

    return hm;
}

/* Inserts a key-value pair into the map. Returns NULL if map did not have key, old value if it did. */
void *unja_hashmap_insert(struct unja_hashmap *hm, char *key, void *value) {
    int pos = hash(key) % HASHMAP_CAP;
    struct node *head = hm->buckets[pos];
    struct node *node = head;
    void *old_value;

    while (node) {
        if (strcmp(node->key, key) == 0) {
            old_value = node->value;
            node->value = value;
            return old_value;
        }
        node = node->next;
    }
    
    node = malloc(sizeof *node);
    node->key = key;
    node->value = value;
    node->next = head;
    hm->buckets[pos] = node;
    return NULL;
}

/* Returns a pointer to the value corresponding to the key. */
void *unja_hashmap_get(struct unja_hashmap *hm, char *key) {
    unsigned int pos = hash(key) % HASHMAP_CAP;
    struct node *node = hm->buckets[pos];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }

        node = node->next;
    }

    return NULL;
}

/* Retrieve pointer to value by key, handles dot notation for nested unja_hashmaps */
void *unja_hashmap_resolve(struct unja_hashmap *hm, char *key) {
    char tmp_key[64];
    int i = 0;
    int j = 0;

    while (1) {
        for (j=0; key[i] != '.' && key[i] != '\0'; i++, j++) {
            tmp_key[j] = key[i];
        }
        tmp_key[j] = '\0';
        hm = unja_hashmap_get(hm, tmp_key);
        
        // stop if we read key to end of string
        if (key[i] == '\0') {
            break;
        }

        // otherwise, continue reading keys
        i++;
    }

    return hm;
}

/* Removes a key from the map, returning the value at the key if the key was previously in the map. */
void *unja_hashmap_remove(struct unja_hashmap *hm, char *key) {
    int pos = hash(key) % HASHMAP_CAP;
    struct node *node = hm->buckets[pos];
    struct node *prev = NULL;
    void *old_value;

    while (node) {
        if (strcmp(node->key, key) == 0) {
            if (prev) {
                prev->next = node->next;
            } else {
                hm->buckets[pos] = node->next;
            }
            old_value = node->value;
            free(node);            
            return old_value;
        }

        node = node->next;
        prev = node;
    }

    return NULL;
}

void unja_hashmap_walk(struct unja_hashmap *hm, void (*fn)(void *value)) {
    struct node *node;
    struct node *next;

    for (int i=0; i < HASHMAP_CAP; i++) {
        node = hm->buckets[i];
        while (node) {
            next = node->next;
            fn(node->value);
            node = next;
        }
    }
}

/* free unja_hashmap related memory */
void unja_hashmap_free(struct unja_hashmap *hm) {
    struct node *node;
    struct node *next;

    for (int i=0; i < HASHMAP_CAP; i++) {
        node = hm->buckets[i];
        while (node) {
            next = node->next;
            free(node);            
            node = next;
        }
    }

    free(hm);
}