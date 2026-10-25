#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include "rulereader.h"

MODULE_AUTHOR("Nicholas Woods");
MODULE_DESCRIPTION("More capabilities");
MODULE_LICENSE("NONE");

static struct proc_dir_entry *proc_entry;

static ssize_t _read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset) {
    char *rules_string = print_rules();
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
    _rulebook = (struct rulebook*)kmalloc(sizeof(struct rulebook), GFP_KERNEL);
    _rulebook->rulesets_len = 0;
    _rulebook->enabled = false;
    proc_entry = proc_create("morecap", 0666, NULL, &fops);
    printk(KERN_INFO "Loaded morecap");
    return 0;
}

static void __exit deentry(void) {
    proc_remove(proc_entry);
    printk(KERN_INFO "Unloaded morecap");
}

module_init(entry);
module_exit(deentry);