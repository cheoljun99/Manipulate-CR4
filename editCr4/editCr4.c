#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>


static void modify_cr4(void *info)
{
    unsigned long cr4_val;

    // 현재 CR4 값 읽기
    asm volatile("mov %%cr4, %0" : "=r"(cr4_val));
    printk(KERN_INFO "Original CR4: 0x%016lx\n", cr4_val);

    //PKE 비트 set
    unsigned long mask = ~(3<< 20);
    printk(KERN_INFO "mask for clearing: 0x%016lx\n", mask);
    cr4_val = cr4_val & mask;
    printk(KERN_INFO "CR4 after set PKE bit 1 (before write): 0x%016lx\n", cr4_val);

    // 수정된 CR4 값 기록
    asm volatile("mov %0, %%cr4" :: "r"(cr4_val));
    pr_info("CR4 value set to: %lx\n", cr4_val);  // 직후 로그 출력

    // 다시 CR4 읽기
    asm volatile("mov %%cr4, %0" : "=r"(cr4_val));
    printk(KERN_INFO "Final CR4 after clearing PKE: 0x%016lx\n", cr4_val);


    return ;
}

static int __init cr4_modifier_init(void){
  printk(KERN_INFO "CR4 modifier module loaded\n");
 // 시스템에 존재하는 모든 CPU에서 CR4 값을 수정
    on_each_cpu(modify_cr4, NULL, 1);
    return 0;
}

static void __exit cr4_modifier_exit(void)
{
    printk(KERN_INFO "CR4 modifier module unloaded\n");
}

module_init(cr4_modifier_init);
module_exit(cr4_modifier_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cheoljun Park - cheolun99@kw.ac.kr");
MODULE_DESCRIPTION("set PKE bit (bit 22) 1 in CR4 register and verify");
