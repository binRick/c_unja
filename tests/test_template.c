#include "test.h"
#include "unja_template.h"

START_TESTS 

TEST(textvc_only) {
    char *input = "Hello world.";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "Hello world.");
    free(output);
}

TEST(expr_number) {
    char *input = "Hello {{ 5 }}.";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "Hello 5.");
    free(output);
}

TEST(expr_number_no_spaces) {
    char *input = "Hello {{5}}.";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "Hello 5.");
    free(output);
}

TEST(expr_string) {
    char *input = "Hello {{ \"world\" }}.";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "Hello world.");
    free(output);
}

TEST(expr_string_no_spaces) {
    char *input = "Hello {{\"world\"}}.";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "Hello world.");
    free(output);
}

TEST(expr_symbol) {
    char *input = "Hello {{ name }}.";
    struct unja_hashmap *ctx = unja_hashmap_new();
    unja_hashmap_insert(ctx, "name", "world");
    char *output =unja_template_string(input, ctx);
    assert_str(output, "Hello world.");
    unja_hashmap_free(ctx);
    free(output);
}

TEST(expr_symbol_no_spaces) {
    char *input = "Hello {{name}}.";
    struct unja_hashmap *ctx = unja_hashmap_new();
    unja_hashmap_insert(ctx, "name", "world");
    char *output =unja_template_string(input, ctx);
    assert_str(output, "Hello world.");
    unja_hashmap_free(ctx);
    free(output);
}

TEST(expr_add) {
    struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{{ 5 + 5 }}.", "10."},
        {"{{ 5 + foo }}.", "15."},
        {"{{ \"foo\" + \"bar\" }}", "foobar"},
        {"{{ \"Hello \" + name }}", "Hello Danny"},
        {"{{ 5 + 5 + 5 }}.", "15."},
        {"{{5+5}}", "10"},
        {"{{ 5+5}}", "10"},
        {"{{ 5 +5}}", "10"},
        {"{{ 5 +5}}", "10"},
        {"{{ 5 + 5}}", "10"},
        {"{{ 5 + 5 }}", "10"},
    };

    struct unja_hashmap *ctx = unja_hashmap_new();
    unja_hashmap_insert(ctx, "foo", "10");
    unja_hashmap_insert(ctx, "name", "Danny");

    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, ctx);
        assert_str(output, tests[i].expected_output);
        free(output);
    }

    unja_hashmap_free(ctx);
}

TEST(expr_op_precedence) {
    struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{{ 5 * 2 + 1 }}.", "11."},
        {"{{ 1 +  5 * 2 }}.", "11."},
        {"{{ 10 / 2 + 1 }}.", "6."},
        {"{{ 1 + 10 / 2 }}.", "6."},
    };

    struct unja_hashmap *ctx = unja_hashmap_new();
    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, ctx);
        assert_str(output, tests[i].expected_output);
        free(output);
    }
    unja_hashmap_free(ctx);
}

TEST(expr_subtract) {
    char *input = "{{ 5 - 5 }}";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "0");
    free(output);
}

TEST(expr_divide) {
    char *input = "{{ 5 / 5 }}";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "1");
    free(output);
}

TEST(expr_multiply) {
    char *input = "{{ 5 * 5 }}";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "25");
    free(output);
}

TEST(expr_modulo) {
    char *input = "{{ 5 % 4 }}";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "1");
    free(output);
}

TEST(expr_whitespace) {
    char *input = "Hello \n{{- \"world\" -}}\n.";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "Helloworld.");
    free(output);
}

TEST(expr_op_whitespace) {
    char *input = "\n{{- 5 + 5 -}}\n";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "10");
    free(output);
}

TEST(for_block) {
    char *input = "{% for n in names %}{{ n }}, {% endfor %}";
    struct unja_hashmap *ctx = unja_hashmap_new();

    struct unja_vector *names = unja_vector_new(9);
    unja_vector_push(names, "John");
    unja_vector_push(names, "Sally");
    unja_vector_push(names, "Eric");
    unja_hashmap_insert(ctx, "names", names);

    char *output =unja_template_string(input, ctx);
    assert_str(output, "John, Sally, Eric, ");
    unja_vector_free(names);
    unja_hashmap_free(ctx);
    free(output);
}


TEST(for_block_vars) {
    char *input = "{% for n in names %}"
                  "{{loop.index + 1}}: {{ n }}"
                  "{% if loop.first %} <--{% endif %}"
                  "{% if not loop.last %}\n{% endif %}"
                  "{% endfor %}";
    struct unja_hashmap *ctx = unja_hashmap_new();

    struct unja_vector *names = unja_vector_new(9);
    unja_vector_push(names, "John");
    unja_vector_push(names, "Sally");
    unja_vector_push(names, "Eric");
    unja_hashmap_insert(ctx, "names", names);

    char *output =unja_template_string(input, ctx);
    assert_str(output, "1: John <--\n2: Sally\n3: Eric");
    unja_vector_free(names);
    unja_hashmap_free(ctx);
    free(output);
}

TEST(for_block_whitespace) {
    char *input = "\n{%- for n in names -%}\n{{ n }}\n{%- endfor -%}\n";
    struct unja_hashmap *ctx = unja_hashmap_new();

    struct unja_vector *names = unja_vector_new(2);
    unja_vector_push(names, "John");
    unja_vector_push(names, "Sally");
    unja_hashmap_insert(ctx, "names", names);

    char *output =unja_template_string(input, ctx);
    assert_str(output, "John\nSally");
    unja_vector_free(names);
    unja_hashmap_free(ctx);
    free(output);
}

TEST(var_dot_notation) {
    char *input = "Hello {{user.name}}!";
    struct unja_hashmap *ctx = unja_hashmap_new();
    struct unja_hashmap *user = unja_hashmap_new();
    unja_hashmap_insert(user, "name", "Danny");
    unja_hashmap_insert(ctx, "user", user);
    char *output =unja_template_string(input, ctx);
    assert_str(output, "Hello Danny!");
    unja_hashmap_free(user);
    unja_hashmap_free(ctx);
    free(output);
}

TEST(comment) {
    char *input = "Hello {# comment here #}world.";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "Hello world.");
    free(output);
}

TEST(if_block) {
    struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{% if 5 > 10 %}1{% endif %}", ""},
        {"{% if 10 > 5 %}1{% endif %}", "1"},
        {"{% if foobar %}1{% endif %}", ""},
        {"{% if name %}1{% endif %}", "1"},
        {"{% if age > 10 %}1{% endif %}", "1"},
        {"{% if 10 + 1 > 10 %}1{% endif %}", "1"},
        {"{% if 6 > 10 - 5 %}1{% endif %}", "1"},
    };

    struct unja_hashmap *ctx = unja_hashmap_new();
    unja_hashmap_insert(ctx, "name", "Danny");
    unja_hashmap_insert(ctx, "age", "29");
    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, ctx);
        assert_str(output, tests[i].expected_output);
        free(output);
    }

    unja_hashmap_free(ctx);
}

TEST(expr_gt) {
    struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{% if 10 > 9 %}1{% endif %}", "1"},
        {"{% if 10 > 10 %}1{% endif %}", ""},
        {"{% if 10 > 11 %}1{% endif %}", ""},
    };

    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, NULL);
        assert_str(output, tests[i].expected_output);
        free(output);
    }
}

TEST(expr_gte) {
    struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{% if 10 >= 9 %}1{% endif %}", "1"},
        {"{% if 10 >= 10 %}1{% endif %}", "1"},
        {"{% if 10 >= 11 %}1{% endif %}", ""},
    };

    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, NULL);
        assert_str(output, tests[i].expected_output);
        free(output);
    }
}

TEST(expr_lt) {
    struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{% if 10 < 9 %}1{% endif %}", ""},
        {"{% if 10 < 10 %}1{% endif %}", ""},
        {"{% if 10 < 11 %}1{% endif %}", "1"},
    };

    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, NULL);
        assert_str(output, tests[i].expected_output);
        free(output);
    }
}

TEST(expr_lte) {
    struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{% if 10 <= 9 %}1{% endif %}", ""},
        {"{% if 10 <= 10 %}1{% endif %}", "1"},
        {"{% if 10 <= 11 %}1{% endif %}", "1"},
    };

    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, NULL);
        assert_str(output, tests[i].expected_output);
        free(output);
    }
}

TEST(expr_eq) {
   struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{% if 10 == 20 %}1{% endif %}", ""},
        {"{% if 10 == 10 %}1{% endif %}", "1"},
    };

    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, NULL);
        assert_str(output, tests[i].expected_output);
        free(output);
    }
}

TEST(expr_string_eq) {
   struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{% if \"foo\" == \"bar\" %}1{% endif %}", ""},
        {"{% if \"foo\" == \"foo\" %}1{% endif %}", "1"},
    };

    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, NULL);
        assert_str(output, tests[i].expected_output);
        free(output);
    }
}

TEST(expr_not_eq) {
   struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{% if 10 != 20 %}1{% endif %}", "1"},
        {"{% if 10 != 10 %}1{% endif %}", ""},
    };

    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, NULL);
        assert_str(output, tests[i].expected_output);
        free(output);
    }
}

TEST(expr_string_not_eq) {
   struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{% if \"foo\" != \"bar\" %}1{% endif %}", "1"},
        {"{% if \"foo\" != \"foo\" %}1{% endif %}", ""},
    };

    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, NULL);
        assert_str(output, tests[i].expected_output);
        free(output);
    }
}

TEST(if_block_whitespace) {
    char *input = "\n{%- if 10 > 5 -%}\nOK\n{%- endif -%}\n";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "OK");
    free(output);
}

TEST(if_else_block) {
    struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{% if 5 > 10 %}1{% else %}2{% endif %}", "2"},
        {"{% if 10 > 5 %}1{% else %}2{% endif %}", "1"},
        {"{% if foobar %}1{% else %}2{% endif %}", "2"},
        {"{% if name %}1{% else %}2{% endif %}", "1"},
        {"{% if age < 10 %}1{% else %}2{% endif %}", "2"},
        {"{% if age + 5 < 10 %}1{% else %}2{% endif %}", "2"},
        {"{% if age + 5 > 29 %}1{% else %}2{% endif %}", "1"},
    };

    struct unja_hashmap *ctx = unja_hashmap_new();
    unja_hashmap_insert(ctx, "name", "Danny");
    unja_hashmap_insert(ctx, "age", "29");
    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output =unja_template_string(tests[i].input, ctx);
        assert_str(output, tests[i].expected_output);
        free(output);
    }

    unja_hashmap_free(ctx);
}

TEST(if_else_block_whitespace) {
    char *input = "\n{%- if 5 > 10 -%}NOT OK{% else -%}\nOK\n{%- endif -%}\n";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "OK");
    free(output);
}

TEST(buffer_alloc) {
    /* Output a string so that output buffer is longer than unja_template buffer, 
        to test dynamic allocation */
    char *input = "{{ n }}";
    struct unja_hashmap *ctx = unja_hashmap_new();
    char *text = "Lorem ipsum dolor sit amet.";
    unja_hashmap_insert(ctx, "n", text);

    char *output =unja_template_string(input, ctx);
    assert_str(output, text);
    unja_hashmap_free(ctx);
    free(output);
}

TEST(inheritance_depth_1) {
    struct env *env = env_new("./tests/data/inheritance-depth-1/");
    char *output = unja_template(env, "one.tmpl", NULL);
    assert_str(output, "Header\nChild content\nFooter\n");
    free(output);
    env_free(env);
}

TEST(inheritance_depth_2) {
    struct env *env = env_new("./tests/data/inheritance-depth-2/");
    char *output = unja_template(env, "two.tmpl", NULL);
    assert_str(output, "0\n1\n2\n");
    free(output);
    env_free(env);
}

TEST(filter_trim) {
    char *input = "{{ text | trim }}";
    struct unja_hashmap *ctx = unja_hashmap_new();
    char *text = "\nHello world\n";
    unja_hashmap_insert(ctx, "text", text);
    char *output =unja_template_string(input, ctx);
    assert_str(output, "Hello world");
    unja_hashmap_free(ctx);
    free(output);
}

TEST(filter_lower) {
    char *input = "{{ \"Hello World\" | lower }}";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "hello world");
    free(output);
}

TEST(filter_wordcount) {
    char *input = "{{ \"Hello World. How are we?\" | wordcount }}";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "5");
    free(output);

    input = "{% if \"Hello World. How are we?\" | wordcount > 4 %}1{% endif %}";
    output =unja_template_string(input, NULL);
    assert_str(output, "1");
    free(output);
}

TEST(filter_title) {
    char *input = "{{ \"Hello world\" | length }}";
    char *output =unja_template_string(input, NULL);
    assert_str(output, "11");
    free(output);
}

TEST(inheritance_depth_2) {
    struct env *env = env_new("./tests/data/template-with-logic/");
    char *output = unja_template(env, "child.tmpl", NULL);
    assert_str(output, "Header\n\thello world\n\t2 is more than 1.\nFooter\n");
    free(output);
    env_free(env);
}

END_TESTS 
