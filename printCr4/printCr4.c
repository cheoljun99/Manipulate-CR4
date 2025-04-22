#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init cr4_reader_init(void)
{
    unsigned long cr4_val;

    asm volatile("mov %%cr4, %0" : "=r"(cr4_val));
    printk(KERN_INFO "CR4 register value: 0x%016lx\n", cr4_val);

    return 0;
}

static void __exit cr4_reader_exit(void)
{
    printk(KERN_INFO "CR4 reader module unloaded\n");
}

module_init(cr4_reader_init);
module_exit(cr4_reader_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cheoljun Park - cheolun99@kw.ac.kr");
MODULE_DESCRIPTION("Print CR4 register value using printk");
