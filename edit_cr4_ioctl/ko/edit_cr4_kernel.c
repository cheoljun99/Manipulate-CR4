#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <asm/special_insns.h> 

#define DEVICE_NAME "edit_cr4_device"
#define IOCTL_CMD_CR4_EDIT 0xFFF1 
#define IOCTL_CMD_CR4_READ 0xFFF2  
#define MAJOR_NUM 506

typedef struct USERDATA {
    unsigned long input[2];   // 유저가 커널에 전달할 값 (2개)
    unsigned long output[2];     // 커널이 계산한 결과를 유저에게 돌려주는 값
}USERDATA;



long edit_cr4_init_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static void edit_cr4_func(void *info);
static void read_cr4_func_left(void *info);
static void read_cr4_func_right(void *info);

long edit_cr4_init_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {

    printk(KERN_INFO "event calling!!!\n");
    int cpu_count = num_online_cpus();
    printk(KERN_INFO "Online CPU count: %d\n", cpu_count);
    
    if (cmd == IOCTL_CMD_CR4_EDIT) {


        int cpu_count = num_online_cpus();
        USERDATA *user_data_arr = kmalloc_array(cpu_count, sizeof(USERDATA), GFP_KERNEL);
        if (!user_data_arr)
            return -ENOMEM;

        if (copy_from_user(user_data_arr, (USERDATA __user *)arg, sizeof(USERDATA) * cpu_count)) {
            kfree(user_data_arr);
            return -EFAULT;
        }
        on_each_cpu(read_cr4_func_left, user_data_arr, 1);
        on_each_cpu(edit_cr4_func, user_data_arr, 1);
        on_each_cpu(read_cr4_func_right, user_data_arr, 1);
        if (copy_to_user((USERDATA __user *)arg, user_data_arr, sizeof(USERDATA) * cpu_count)) {
            kfree(user_data_arr);
            return -EFAULT;
        }
        kfree(user_data_arr);
        printk(KERN_DEBUG "IOCTL_CMD_CR4_EDIT finish");
        
    }
    else if (cmd == IOCTL_CMD_CR4_READ) {
        USERDATA *user_data_arr = kmalloc_array(cpu_count, sizeof(USERDATA), GFP_KERNEL);
        if (!user_data_arr)
            return -ENOMEM;

        if (copy_from_user(user_data_arr, (USERDATA __user *)arg, sizeof(USERDATA) * cpu_count)) {
            kfree(user_data_arr);
            return -EFAULT;
        }
        on_each_cpu(read_cr4_func_right, user_data_arr, 1);
        if (copy_to_user((USERDATA __user *)arg, user_data_arr, sizeof(USERDATA) * cpu_count)) {
            kfree(user_data_arr);
            return -EFAULT;
        }
        kfree(user_data_arr);
        printk(KERN_DEBUG "IOCTL_CMD_CR4_READ finish");

    }
    printk(KERN_INFO "event finishing!!!\n");

    return 0;
}

static void edit_cr4_func(void *info)
{
    USERDATA *user_data_arr = (USERDATA *)info;
    int cpu = smp_processor_id();  // 현재 CPU ID
    USERDATA *user_data = &user_data_arr[cpu];

    unsigned long cr4_val;

    // 현재 CR4 값 읽기
    asm volatile("mov %%cr4, %0" : "=r"(cr4_val));
    printk(KERN_INFO "Original CR4 (CPU %d): 0x%lX\n", cpu, cr4_val);

    if (user_data->input[1] == 0) {
        // 비트 clearing
        unsigned long mask = ~(1UL << user_data->input[0]);
        printk(KERN_INFO "mask for clearing: 0x%lX\n", mask);
        cr4_val &= mask;
        printk(KERN_INFO "CR4 after clearing %lu-th bit (CPU %d): 0x%lX\n", user_data->input[0], cpu, cr4_val);

        // 수정된 CR4 값 기록
        asm volatile("mov %0, %%cr4" :: "r"(cr4_val));

    } else if (user_data->input[1] == 1) {
        // 비트 setting
        unsigned long mask = (1UL << user_data->input[0]);
        printk(KERN_INFO "mask for setting: 0x%lX\n", mask);
        cr4_val |= mask;
        printk(KERN_INFO "CR4 after setting %lu-th bit (CPU %d): 0x%lX\n", user_data->input[0], cpu, cr4_val);

        // 수정된 CR4 값 기록
        asm volatile("mov %0, %%cr4" :: "r"(cr4_val));
    }

    return;
}

static void read_cr4_func_left(void *info){

    USERDATA *user_data_arr = (USERDATA *)info;
    int cpu = smp_processor_id();
    user_data_arr[cpu].output[0] = __read_cr4();
}
static void read_cr4_func_right(void *info){

    USERDATA *user_data_arr = (USERDATA *)info;
    int cpu = smp_processor_id();
    user_data_arr[cpu].output[1] = __read_cr4();
}


static struct file_operations fops = {
    .unlocked_ioctl = edit_cr4_init_ioctl,
};


// 모듈 초기화 함수
static int __init edit_cr4_init(void) {
    int result = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (result < 0) {
        printk(KERN_ALERT "Failed to register char device\n");
        return result;
    }
    printk(KERN_INFO "edit_cr4 device registered with major number %d\n", MAJOR_NUM);
    return 0;
}

// 모듈 종료 함수
static void __exit edit_cr4_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk(KERN_INFO "edit_cr4 device unregistered\n");
}

module_init(edit_cr4_init);
module_exit(edit_cr4_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cheoljun Park - cheolun99@kw.ac.kr");
MODULE_DESCRIPTION("Edit CR4 register kernel module");