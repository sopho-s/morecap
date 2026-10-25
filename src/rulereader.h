#include <linux/types.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/kernel_read_file.h>
#include <linux/ctype.h>
#pragma once

struct rule {
    char* rule_name;
    unsigned char action;
    uint16_t capability;
    char* program;
    char** kwargs;
    unsigned char kwargs_len;
    bool enabled;
};

struct ruleset {
    char* ruleset_name;
    struct rule** rules;
    unsigned int rules_len;
    bool enabled;
};

struct rulebook {
    struct ruleset** rulesets;
    unsigned int rulesets_len;
    bool enabled;
};

extern struct rulebook *_rulebook;

void add_const_chars_with_move(char** string, char* const_chars);

char* print_rules(void) __attribute__((cold));

struct parsereturn {
    struct ruleset _ruleset;
    char** includes;
    unsigned int includeam;
};

struct rule* parse_next_rule(char** rulepointer) __attribute__((hot));

struct parsereturn parse_rules(char *rulefile) __attribute__((cold));

void read_rules(void) __attribute__((cold));