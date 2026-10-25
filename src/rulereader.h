#include <linux/types.h>
#include <linux/module.h>
#include <linux/slab.h>
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

char* print_rules(void) __attribute__((cold));