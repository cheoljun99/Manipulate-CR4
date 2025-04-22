#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

#define DEVICE_FILE "/dev/comms_device"
#define IOCTL_CMD 0xFFA1  // 커널 모듈과 일치하는 ioctl 명령어
#define USER_VA 0xFFA2    // 사용자 주소 공간에서 특정 주소를 받는 ioctl 명령어 (예시)
#define ACCESS_CMD 0xFFA3  // 새로운 ACCESS_CMD 명령어
#define FUNC_VA 0xFFA4  // 새로운 FUNC_VA 명령어
#define EXCUTE_CMD 0xFFA5 
#define MESSAGE_SIZE 256  // 커널에서 받을 메시지 버퍼 크기

char temp[2] = "HI";

int user_func(){
   int a=1;
   int b=2;
   int c= a+b;
   return c;
}

int main() {
    int fd = open(DEVICE_FILE, O_RDWR);
    if (fd == -1) {
        perror("Failed to open device file");
        return 1;
    }

    // 커널로부터 메시지를 받을 버퍼
    char buffer[MESSAGE_SIZE];

    while (1) {
        // 사용자로부터 명령어 번호 입력 받기
        int command_number;
        printf("Enter command number (1, 2, 3, 4, 5) : ");
        if (scanf("%d", &command_number) != 1) {
            printf("Invalid input. Exiting...\n");
            break;  // 잘못된 입력이 들어오면 종료
        }

        if (command_number == 0) {
            break;  // 0 입력 시 루프 종료
        }

        // 커맨드 번호에 따라 적절한 ioctl 명령어 실행
        if (command_number == 1) {
            // 커널로부터 메시지 받기 (IOCTL_CMD 명령어 사용)
            if (ioctl(fd, IOCTL_CMD, buffer) == -1) {
                perror("ioctl failed");
                break;  // 오류 발생 시 루프 종료
            }

            printf("Received message from kernel: %s\n", buffer);
        } else if (command_number == 2) {
            // 사용자 주소 공간에서 특정 주소를 받아오기 위한 코드
            unsigned long long user_var_address = (unsigned long long)temp;  // temp의 주소를 숫자로 변환
            if (ioctl(fd, USER_VA, &user_var_address) == -1) {
                perror("ioctl failed to get user address");
                break;  // 오류 발생 시 루프 종료
            }

            printf("Sent user address: 0x%llx\n", user_var_address);
        } else if (command_number == 3) {
            // ACCESS_CMD를 커널로 전달 (유저에서 따로 전달할 값 없음)
            
            
            if (ioctl(fd, ACCESS_CMD, NULL) == -1) {
                perror("ioctl failed to send ACCES_CMD");
                break;  // 오류 발생 시 루프 종료
            }
            printf("Sent ACCES_CMD to kernel\n");
        }
        else if (command_number == 4) {
            // FUNC_VA를 커널로 전달 (유저에서 따로 전달할 값 없음)
            
            unsigned long long user_func_addr = (unsigned long long)user_func;  // temp의 주소를 숫자로 변환
            if (ioctl(fd, FUNC_VA, &user_func_addr) == -1) {
                perror("ioctl failed to get user address");
                break;  // 오류 발생 시 루프 종료
            }
            printf("Sent user address: 0x%llx\n", user_func_addr);
        } else if (command_number == 5) {
                
            //int (*test)(void);
            //test =(int(*)(void))print_func;
            //int num= test();
            //printf("%d\n",num);
            
            if (ioctl(fd, EXCUTE_CMD, NULL) == -1) {
                perror("ioctl failed to send EXCUTE_CMD");
                break;  // 오류 발생 시 루프 종료
            }
            printf("Sent EXCUTE_CMD to kernel\n");
        }
        
        else {
            printf("Invalid command number. Please enter 1, 2, 3, 4, 5.\n");
        }
    }

    close(fd);
    return 0;
}

