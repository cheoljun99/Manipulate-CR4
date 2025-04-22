#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "comms_device"
#define IOCTL_CMD 0xFFA1  // ioctl 명령어 정의
#define USER_VA 0xFFA2  // ioctl 명령어 정의
#define ACCESS_CMD 0xFFA3  // 새로운 ACCESS_CMD 명령어
#define FUNC_VA 0xFFA4  // 새로운 FUNC_VA 명령어
#define EXCUTE_CMD 0xFFA5 

static int major;
static char message[256] = "Hello from Kernel Module!";
long comms_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

unsigned long long user_var_address;
unsigned long long user_func_address;

// ioctl 함수
long comms_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {

    printk("event calling!!!!!!!!!\n");
    
    if (cmd == IOCTL_CMD) {
        if (copy_to_user((char * __user)arg, message, sizeof(message))) {
            return -EFAULT;
        }
        printk("send to user with string\n");
    }
    else if (cmd == USER_VA) {
        // 64비트 주소값을 받아올 unsigned long long 변수 정의
        // 유저로부터 받은 주소값을 user_var_address에 저장
        if (copy_from_user(&user_var_address, (unsigned long long * __user)arg, sizeof(user_var_address))) {
            return -EFAULT;
        }
        // 이 주소값을 사용하여 원하는 작업 수행
        printk("User space data's address : 0x%llx\n", user_var_address);  // 16진수로 출력
    }
    else if(cmd == ACCESS_CMD){
    

    if(user_var_address == 0x0){
        printk("Push 2 in User App\n");
    }
    else{
        // user_var_address를 char*로 변환하여 포인터로 사용
        char *user_space_pointer = (char *)user_var_address;
        
        // user_space_pointer가 실제로 가리키는 주소를 16진수로 출력
        printk("User space data's address : 0x%llx\n", (unsigned long long)user_space_pointer);  // 16진수로 출력
        // 예시: user_space_pointer가 가리키는 값도 출력
        printk("Value at user space char data : %c\n", *user_space_pointer);  // 예를 들어 문자 값 출력
        printk("Value at user space char array data : %s\n", user_space_pointer);  // 예를 들어 문자 값 출력
        *user_space_pointer=(*user_space_pointer)+1;
        printk("Wrtie Value at user space char data : %c\n", *user_space_pointer);  // 예를 들어 문자 값 출력
        printk("Value at user space char array data : %s\n", user_space_pointer);  // 예를 들어 문자 값 출력

    }
}
    else if (cmd == FUNC_VA) {
        if (copy_from_user(&user_func_address, (unsigned long long *)arg, sizeof(user_func_address))) {
            return -EFAULT;
        }

        printk("User Func address: 0x%llx\n", user_func_address);  // 16진수로 출력
    }
    
    else if(cmd == EXCUTE_CMD){
    

    if(user_func_address == 0x0){
        printk("Push 4 in User App\n");
    }
    else{
        int (*user_func_pointer)(void);
        user_func_pointer =(int(*)(void))user_func_address;
        
        // user_space_pointer가 실제로 가리키는 주소를 16진수로 출력
        printk("User Func pointer's address : 0x%llx\n", (unsigned long long)user_func_pointer);  // 16진수로 출력
        int num = user_func_pointer();
        printk("User Func execute in Kernel : %d\n", num);  // 16진수로 출력

    }
}

    return 0;
}

static struct file_operations fops = {
    .unlocked_ioctl = comms_ioctl,
};

// 모듈 초기화 함수
static int __init comms_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register char device\n");
        return major;
    }
    printk(KERN_INFO "Comms device registered with major number %d\n", major);
    return 0;
}

// 모듈 종료 함수
static void __exit comms_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Comms device unregistered\n");
}

module_init(comms_init);
module_exit(comms_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cheoljun Park - cheolun99@kw.ac.kr");
MODULE_DESCRIPTION("A simple communication kernel module");

