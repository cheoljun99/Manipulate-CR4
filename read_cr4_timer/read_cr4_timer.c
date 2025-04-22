#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>      // 타이머 사용을 위한 헤더
#include <asm/special_insns.h>    // 'read_cr4()' 함수가 정의된 헤더
#include <linux/sched.h>      // 'current' 매크로가 정의된 헤더
#include <linux/pid.h>        // 'task_pid_nr' 함수 사용을 위한 헤더

static struct timer_list cr4_timer;  // 타이머 구조체
static unsigned long cr4_value;     // 읽은 cr4 값을 저장할 변수

void cr4_timer_function(struct timer_list *t);

// 1초마다 cr4 값을 읽고, 프로세스 및 스레드 ID를 출력하는 함수
void cr4_timer_function(struct timer_list *t)
{
    cr4_value = __read_cr4();  // cr4 값 읽기

    // 현재 프로세스 및 스레드 ID 출력 (task_pid_nr를 사용하여 정확한 PID와 TID 출력)
    pr_info("CR4 value: %lx, PID: %d, TID: %d\n", cr4_value, task_pid_nr(current), current->pid);

    // 타이머를 다시 1초 후에 실행되도록 설정
    mod_timer(&cr4_timer, jiffies + msecs_to_jiffies(1000));
}

static int __init cr4_read_init(void)
{
    // 타이머 초기화
    timer_setup(&cr4_timer, cr4_timer_function, 0);

    // 타이머를 1초 후에 첫 실행하도록 설정
    mod_timer(&cr4_timer, jiffies + msecs_to_jiffies(1000));

    pr_info("CR4 read module loaded\n");
    return 0;
}

static void __exit cr4_read_exit(void)
{
    // 타이머 삭제
    del_timer(&cr4_timer);

    pr_info("CR4 read module exited\n");
}

module_init(cr4_read_init);  // 모듈 초기화 함수
module_exit(cr4_read_exit);  // 모듈 종료 함수

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cheoljun Park - cheolun99@kw.ac.kr");
MODULE_DESCRIPTION("A Linux driver to read CR4 value every second and print PID and TID");

