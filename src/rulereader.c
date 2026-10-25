#include "rulereader.h"

struct rulebook *_rulebook; 

int add_const_chars_with_move(char** string, char* const_chars) {
    return 0;
}

char* print_rules(void) {
    char *rules = kmalloc(1, GFP_KERNEL);
    unsigned int current_len = 0;
    if (!_rulebook->enabled) {
        rules = (char*)krealloc(rules, strlen("rulebook is disabled") * sizeof(char), GFP_KERNEL);
        const char tmpholder[] = "rulebook is disabled";
        strcpy(rules, tmpholder);
        return rules;
    }
    if (_rulebook->rulesets_len == 0) {
        rules = (char*)krealloc(rules, strlen("no rules defined") * sizeof(char), GFP_KERNEL);
        const char tmpholder[] = "no rules defined";
        strcpy(rules, tmpholder);
        return rules;
    }
    for (int i = 0; i < _rulebook->rulesets_len; i++) {
        if (_rulebook->rulesets[i] == NULL) {
            kfree(rules);
            pr_err("rulebook contains a nullptr ruleset\n");
            return NULL;
        }
        struct ruleset *_ruleset = _rulebook->rulesets[i];
        if (_ruleset->ruleset_name == NULL) {
            pr_err("ruleset name is NULL (weird)\n");
            return NULL;
        }
        rules = (char*)krealloc(rules, current_len + (strlen(_ruleset->ruleset_name) + strlen("ruleset: ")) * sizeof(char), GFP_KERNEL);
        if (rules == NULL) {
            pr_err("morecap failed to kmalloc: %zu bytes\n", strlen(_ruleset->ruleset_name) * sizeof(char));
            return NULL;
        }

        char *current_ruleset = rules + current_len;
        char ruleset_intro[] = "ruleset: ";
        strcpy(current_ruleset, ruleset_intro);
        current_len += strlen(ruleset_intro);
        current_ruleset = rules + current_len;

        strcpy(current_ruleset, _ruleset->ruleset_name);
        current_len += strlen(_ruleset->ruleset_name);
        current_ruleset = rules + current_len;
        for (int t = 0; t < _ruleset->rules_len; t++) {
            struct rule *_rule = _ruleset->rules[t];


            if (_ruleset->ruleset_name == NULL) {
                pr_err("ruleset name is NULL (weird)\n");
                return NULL;
            }
            rules = (char*)krealloc(rules, current_len + (strlen(_ruleset->ruleset_name) + strlen("rule: ")) * sizeof(char), GFP_KERNEL);
            if (rules == NULL) {
                pr_err("morecap failed to kmalloc: %zu bytes\n", strlen(_ruleset->ruleset_name) * sizeof(char));
                return NULL;
            }

            char *current_ruleset = rules + current_len;
            char rule_intro[] = "rule: ";
            strcpy(current_ruleset, ruleset_intro);
            current_len += strlen(ruleset_intro);
            current_ruleset = rules + current_len;


            if (t + 1 != _ruleset->rules_len) {
                rules = (char*)krealloc(rules, current_len + 1, GFP_KERNEL);
                if (rules == NULL) {
                    pr_err("morecap failed to kmalloc: %zu bytes\n", strlen(_ruleset->ruleset_name) * sizeof(char));
                    return NULL;
                }
                current_ruleset[0] = '\n';
                current_len += 1;
            }
        }
        if (i + 1 != _rulebook->rulesets_len) {
            rules = (char*)krealloc(rules, current_len + 1, GFP_KERNEL);
            if (rules == NULL) {
                pr_err("morecap failed to kmalloc: %zu bytes\n", strlen(_ruleset->ruleset_name) * sizeof(char));
                return NULL;
            }
            current_ruleset[0] = '\n';
            current_len += 1;
        }
    }
    return rules;
}

struct parsereturn parse_rules(char *rulefile) {
    struct parsereturn return_vals = {};
    return return_vals;
}

struct rule* parse_next_rule(char** rule_pointer) {
    while (!isalnum(**rule_pointer)) {
        *rule_pointer = *rule_pointer + 1;
    }
    char* rule_name_start = *rule_pointer;
    while (**rule_pointer != ':') {
        *rule_pointer = *rule_pointer + 1;
    }
    struct rule *return_rule = (struct rule*)kmalloc(sizeof(struct rule), GFP_KERNEL);
    if (return_rule == NULL) {
        pr_err("morecap failed to kmalloc: %zu bytes\n", sizeof(struct rule));
        return NULL;
    }
    return_rule->rule_name = (char*)kmalloc((*rule_pointer - rule_name_start + 1) * sizeof(char), GFP_KERNEL);
    if (return_rule->rule_name == NULL) {
        pr_err("morecap failed to kmalloc: %zu bytes\n", (*rule_pointer - rule_name_start) * sizeof(char));
        return NULL;
    }
    memcpy(return_rule->rule_name, rule_name_start, (*rule_pointer - rule_name_start) * sizeof(char));
    return_rule->rule_name[(*rule_pointer - rule_name_start)] = '\x00';
    return return_rule;
}

void read_rules(void) {
    _rulebook->rulesets = (struct ruleset**)kmalloc(sizeof(struct ruleset*) * 10, GFP_KERNEL);
    while (true) {
        struct file *file;
        char *buf = NULL;
        size_t file_size;
        char rule_file_name[] = "/etc/morecap/main.mcap";
        file = filp_open(rule_file_name, O_RDONLY, 0);
        if (IS_ERR(file)) {
            pr_err("Failed to open ruleset file");
            return;
        }

        if (kernel_read_file(file, 0, (void**)&buf, 4294967296, &file_size, READING_UNKNOWN) < 0) {
            pr_err("Failed to read ruleset file");
            return;
        }
        if (buf == NULL) {
            pr_err("File is empty???");
            return;
        }
        struct ruleset *new_ruleset = (struct ruleset*)kmalloc(sizeof(struct ruleset), GFP_KERNEL);
        new_ruleset->ruleset_name = (char*)kmalloc(strlen(rule_file_name) * sizeof(char), GFP_KERNEL);
        strcpy(new_ruleset->ruleset_name, rule_file_name);
        new_ruleset->rules = (struct rule**)kmalloc(sizeof(struct rule*) * 10, GFP_KERNEL);
        char **rule_pointer = &buf;
        int count = 0;
        while(*rule_pointer != buf + file_size && count < 1) {
            struct rule *new_rule = parse_next_rule(rule_pointer);
            new_ruleset->rules[count] = new_rule;
            new_ruleset->rules_len += 1;
            count += 1;
        }
        _rulebook->rulesets[0] = new_ruleset;
        _rulebook->rulesets_len = 1;
        //kfree(buf);
        return;
    }
}