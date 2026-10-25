rules follow the following format

```yaml
rulename:
    action: <allow/log/block>
    capability: <morecap capability>
    program: <full program path>
    kwargs:
        - <specific capability args in key:value format>
    active: <true/false>
```

rules will then be converted by the rule reader into the `rule`, `ruleset`, and `rulebook` structs

`rule` is and individual rule defined within a rule file

`ruleset` is a collection of rules in a rule file

`rulebook` is a collection of rule files

you can include other rule files with the following

```yaml
include:
    - rulefile1.mcap
    - rulefile2.mcap
```

you can enable or disable rules, rulesets, or even the entire rule book on the fly by writing to `/etc/morecap/.newrule`. Rules added on the fly will be added to the `/etc/morecap/.newrule.mcap` ruleset. If you instead want to add to another ruleset (or create a new one) then you can send rules to `/etc/morecap/.newruleset`

if you want to control already loaded rules you can send commands via `/etc/morecap/.control`. Control is done via binary communication, the following is a c struct that would be used for communication

```c
struct control {
    uint8_t command; // the command to run
    uint8_t argc; // amount of arguments
    unsigned char *args; // pointer to args
}
```

each arg should be seperated by a null byte, therefore arguments should not have null bytes themselves