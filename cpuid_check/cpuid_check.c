#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/processor.h>
#include <asm/msr.h>
#include <asm/io.h>

static bool cpu_supports_pke(void)
{
    unsigned int eax, ebx, ecx, edx;

    // CPUID with EAX=7, ECX=0
    asm volatile("cpuid"
                 : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                 : "a"(0x7), "c"(0x0));

    printk(KERN_INFO "[cpuid_check] CPUID.(EAX=7, ECX=0):\n");
    printk(KERN_INFO "  EAX=0x%08x\n", eax);
    printk(KERN_INFO "  EBX=0x%08x\n", ebx);
    printk(KERN_INFO "  ECX=0x%08x\n", ecx);
    printk(KERN_INFO "  EDX=0x%08x\n", edx);

    return ecx & (1 << 4);  // PKE is ECX[4]
}


static int __init check_pke_support_init(void)
{
    if (cpu_supports_pke()) {
        printk(KERN_INFO "[cpuid_check] CPU supports PKE (CR4[22])\n");
    } else {
        printk(KERN_WARNING "[cpuid_check] CPU does NOT support PKE (CR4[22])\n");
    }

    return 0;
}

static void __exit check_pke_support_exit(void)
{
    printk(KERN_INFO "[cpuid_check] Module unloaded\n");
}

module_init(check_pke_support_init);
module_exit(check_pke_support_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cheoljun Park - cheolun99@kw.ac.kr");
MODULE_DESCRIPTION("Check if CPU supports PKE using CPUID");
