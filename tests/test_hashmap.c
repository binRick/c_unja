#include "test.h"
#include "unja_hashmap.h"

START_TESTS

TEST(unja_hashmap) {
    struct unja_hashmap *hm = unja_hashmap_new();
    char *value = unja_hashmap_get(hm, "foo");
    assert_null(value);
    value = unja_hashmap_insert(hm, "foo", "bar");
    assert_null(value);
    value = unja_hashmap_get(hm, "foo");
    assert_str(value, "bar");
    unja_hashmap_free(hm);
} 

TEST(dot_notation) {
    void *val;
    struct unja_hashmap *user = unja_hashmap_new();
    val = unja_hashmap_insert(user, "name", "Danny");
    assert_null(val);
    struct unja_hashmap *hm = unja_hashmap_new();
    val = unja_hashmap_insert(hm, "user", user);
    assert_null(val);
    assert(unja_hashmap_resolve(hm, "user") == user, "expected user unja_hashmap, got something else");
    val = unja_hashmap_resolve(hm, "user.name");
    assert_str(val, "Danny");
    unja_hashmap_free(user);
    unja_hashmap_free(hm);
} 

TEST(unja_hashmap_remove) {
    struct unja_hashmap *hm = unja_hashmap_new();
    unja_hashmap_insert(hm, "foo", "bar");
    char *value = unja_hashmap_get(hm, "foo");
    assert_str(value, "bar");

    // remove once
    value = unja_hashmap_remove(hm, "foo");
    assert_str(value, "bar");
    value = unja_hashmap_get(hm, "foo");
    assert_null(value);

    // remove again (should no-op)
    value = unja_hashmap_remove(hm, "foo");
    assert_null(value);
    value = unja_hashmap_get(hm, "foo");
    assert_null(value);

    unja_hashmap_free(hm);
} 

END_TESTS