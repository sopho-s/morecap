#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include "rulereader.h"

MODULE_AUTHOR("Nicholas Woods");
MODULE_DESCRIPTION("More capabilities");
MODULE_LICENSE("GPL and additional rights");

static struct proc_dir_entry *proc_entry;

static ssize_t _read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset) {
    char *rules_string = print_rules();
    if (rules_string == NULL) {
        return -EFAULT;
    }
    int len = strlen(rules_string);
    if (*offset > 0) {
        kfree(rules_string);
        return 0;
    }
    if (copy_to_user(user_buffer, rules_string, len)) {
        kfree(rules_string);
        return -EFAULT;
    }
    kfree(rules_string);
    *offset = len;
    return len;
}

static const struct proc_ops fops = {
    .proc_read = _read
};

static int __init entry(void) {
    printk(KERN_INFO "Loaded morecap\n");
    _rulebook = (struct rulebook*)kmalloc(sizeof(struct rulebook), GFP_KERNEL);
    _rulebook->rulesets_len = 0;
    read_rules();
    proc_entry = proc_create("morecap", 0666, NULL, &fops);
    if (_rulebook->rulesets_len == 0) {
        printk(KERN_INFO "No rulesets loaded, rulebook set as disabled\n");
        _rulebook->enabled = false;
    } else {
        printk(KERN_INFO "Rulebook initialised\n");
        _rulebook->enabled = true;
    }
    return 0;
}

static void __exit deentry(void) {
    proc_remove(proc_entry);
    printk(KERN_INFO "Unloaded morecap\n");
}

module_init(entry);
module_exit(deentry);