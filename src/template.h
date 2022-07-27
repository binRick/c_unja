#include "unja_hashmap.h"
#include "unja_vector.h"


struct env;
struct env *env_new();
void env_free(struct env *env);
char *template(struct env *env, char *template_name, struct unja_hashmap *ctx);
char *template_string(char *tmpl, struct unja_hashmap *ctx);
char *read_file(char *filename);
