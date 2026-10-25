#include "rulereader.h"

struct rulebook *_rulebook; 

char* print_rules(void) {
    char *rules = kmalloc(1, GFP_KERNEL);
    unsigned int currentlen = 0;
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
        rules = (char*)krealloc(rules, currentlen + strlen(_ruleset->ruleset_name) * sizeof(char), GFP_KERNEL);
        if (rules == NULL) {
            pr_err("morecap failed to kmalloc: %zu bytes\n", strlen(_ruleset->ruleset_name) * sizeof(char));
            return NULL;
        }
        char *current_ruleset = rules + currentlen;
        strcpy(current_ruleset, _ruleset->ruleset_name);
        currentlen += strlen(_ruleset->ruleset_name);
        if (i + 1 != _rulebook->rulesets_len) {
            rules = (char*)krealloc(rules, currentlen + 1, GFP_KERNEL);
            if (rules == NULL) {
                pr_err("morecap failed to kmalloc: %zu bytes\n", strlen(_ruleset->ruleset_name) * sizeof(char));
                return NULL;
            }
            rules[currentlen] = '\n';
            currentlen += 1;
        }
    }
    return rules;
}