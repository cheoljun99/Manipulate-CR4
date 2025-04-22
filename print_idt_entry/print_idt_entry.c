#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>  // virt_to_phys 사용을 위한 헤더


#define IDT_ENTRIES 256
static void test(struct gate_struct c);

static void test(struct gate_struct c){
    unsigned long handler_addr = ((unsigned long)c.offset_low) |
                                     ((unsigned long)c.offset_middle << 16) |
                                     ((unsigned long)c.offset_high << 32);
    pr_info("IDT Entry 253 handler_addr : %px (change)\n",(void *)handler_addr);
}
static int __init idt_table_addr_init(void)
{
    pr_info("IDT Table Address module loaded\n");

    struct desc_ptr idt_ptr;

    // SIDT 명령어로 IDT 테이블의 base 주소 획득
    asm("sidt %0" : "=m" (idt_ptr));

    pr_info("IDT Table Base Address (Virtual): %px\n", (void *)(uintptr_t)idt_ptr.address);


    // 가상 주소를 물리 주소로 변환하여 출력
    unsigned long phys_addr = virt_to_phys((void *)(uintptr_t)idt_ptr.address);
    pr_info("IDT Table Base Address (Physical): %lx\n", phys_addr);

    struct gate_struct *entry_13=0;
    struct gate_struct *entry_253=0;


    // IDT의 각 엔트리 순회
    for (int i = 0; i < IDT_ENTRIES; i++) {
        struct gate_struct *entry = (struct gate_struct *)(uintptr_t)(idt_ptr.address + i * sizeof(struct gate_struct));
        // 각 엔트리의 가상 주소 출력
        pr_info("IDT Entry %d (Virtual): %px\n", i, (void *)entry);
        // 핸들러 주소 구성
        unsigned long handler_addr = ((unsigned long)entry->offset_low) |
                                     ((unsigned long)entry->offset_middle << 16) |
                                     ((unsigned long)entry->offset_high << 32);

        pr_info("IDT Entry %d details : handler Address = %px | offset_low = 0x%04x | offset_middle = 0x%04x| offset_high = 0x%04x | ist : 0x%01x | zero = 0x%02x | type = 0x%02x | dpl : 0x%01x | p : 0x%01x | segment = 0x%04x | reserved = 0x%08x\n",
                i, (void *)handler_addr,entry->offset_low,entry->offset_middle,entry->offset_high,entry->bits.ist,entry->bits.zero, entry->bits.type,entry->bits.dpl,entry->bits.p, entry->segment,entry->reserved);

        if(i==13){
            entry_13=entry;
        }
        else if(i==253){
            entry_253=entry;
        }

    }

    
    pr_info("IDT Entry 13 (Virtual): %px\n",(void *)entry_13);
    pr_info("IDT Entry 253 (Virtual): %px\n",(void *)entry_253);

    struct gate_struct *temp_entry_empty = kmalloc(sizeof(struct gate_struct), GFP_KERNEL);
    if (temp_entry_empty) {
        memset(temp_entry_empty, 0, sizeof(struct gate_struct));
    }

    struct gate_struct *temp_entry_13 = kmalloc(sizeof(struct gate_struct), GFP_KERNEL);
    memcpy(temp_entry_13, entry_13, sizeof(struct gate_struct));

    struct gate_struct *temp_entry_253 = kmalloc(sizeof(struct gate_struct), GFP_KERNEL);
    memcpy(temp_entry_253, entry_253, sizeof(struct gate_struct));
    //temp_entry_253->offset_low =0x0f30;
    //temp_entry_253->offset_middle =0x8240;
    //temp_entry_253->offset_high =0xffffffff;
    
    memcpy(entry_253, temp_entry_13, sizeof(struct gate_struct));
    struct gate_struct c= *entry_253;
    memcpy(entry_253, temp_entry_253, sizeof(struct gate_struct));

    


    // //test(c);

    unsigned long handler_addr = ((unsigned long)c.offset_low) |
                                     ((unsigned long)c.offset_middle << 16) |
                                     ((unsigned long)c.offset_high << 32);
    pr_info("IDT Entry 253 handler_addr : %px (change)\n",(void *)handler_addr);



    handler_addr = ((unsigned long)entry_253->offset_low) |
    ((unsigned long)entry_253->offset_middle << 16) |
    ((unsigned long)entry_253->offset_high << 32);
    pr_info("IDT Entry 253 handler_addr : %px (recovery)\n",(void *)handler_addr);
    



    return 0;
}

static void __exit idt_table_addr_exit(void)
{
    pr_info("IDT Table module exited\n");
}

module_init(idt_table_addr_init);
module_exit(idt_table_addr_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cheoljun Park - cheolun99@kw.ac.kr");
MODULE_DESCRIPTION("IDT Table 및 각 엔트리의 핸들러 주소 출력하는 커널 모듈");
