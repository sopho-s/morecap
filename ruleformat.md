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

you can enable or disable rules, rulesets, or even the entire rule book on the fly by writing to the 