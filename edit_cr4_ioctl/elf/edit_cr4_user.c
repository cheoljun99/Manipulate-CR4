#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

#define DEVICE_FILE "/dev/edit_cr4_device"
#define IOCTL_CMD_CR4_EDIT 0xFFF1 
#define IOCTL_CMD_CR4_READ 0xFFF2  


typedef struct USERDATA {
    unsigned long input[2];
    unsigned long output[2];
}USERDATA;


typedef struct {
    int bit;
    const char* label;
    const char* description;
} CR4BitInfo;

CR4BitInfo cr4_bits[] = {
    {0,  "VME",        "Virtual 8086 Mode Extensions"},
    {1,  "PVI",        "Protected-mode Virtual Interrupts"},
    {2,  "TSD",        "Time Stamp Disable"},
    {3,  "DE",         "Debugging Extensions"},
    {4,  "PSE",        "Page Size Extension"},
    {5,  "PAE",        "Physical Address Extension"},
    {6,  "MCE",        "Machine Check Exception"},
    {7,  "PGE",        "Page Global Enabled"},
    {8,  "PCE",        "Performance-Monitoring Counter enable"},
    {9,  "OSFXSR",     "OS support for FXSAVE/FXRSTOR"},
    {10, "OSXMMEXCPT", "OS support for unmasked SIMD FP exceptions"},
    {11, "UMIP",       "User-Mode Instruction Prevention"},
    {12, "LA57",       "57-bit linear addresses (5-level paging)"},
    {13, "VMXE",       "Virtual Machine Extensions Enable"},
    {14, "SMXE",       "Safer Mode Extensions Enable"},
    {16, "FSGSBASE",   "RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE enabled"},
    {17, "PCIDE",      "PCID Enable"},
    {18, "OSXSAVE",    "XSAVE and Processor Extended States Enable"},
    {20, "SMEP",       "Supervisor Mode Execution Protection Enable"},
    {21, "SMAP",       "Supervisor Mode Access Prevention Enable"},
    {22, "PKE",        "Protection Key Enable"},
    {23, "CET",        "Control-flow Enforcement Technology"},
    {24, "PKS",        "Enable Protection Keys for Supervisor-Mode Pages"}
};

int num_bits = sizeof(cr4_bits) / sizeof(CR4BitInfo);

int main() {

    int fd = open(DEVICE_FILE, O_RDWR);
    if (fd == -1) {
        perror("Failed to open device file");
        return 1;
    }
    int cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Online CPU count: %d\n", cpu_count);


    while (1) {
        int command_number;
        int ch;
        printf("Enter command number (1 = Edit CR4, 2 = Read current CR4, 0 = Exit) : ");

        if (scanf("%d", &command_number) != 1) {
            printf("Invalid input. Exiting...\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            continue; 
        }
        while ((ch = getchar()) != '\n' && ch != EOF);

        if (command_number == 0) {
            break; 
        }

        else if(command_number ==1){

            printf("═════════CR4 resister field list═════════\n");
            printf("%-4s %-12s %s\n", "Bit", "Label", "Description");
            for (int i = 0; i < num_bits; i++) {
                printf("%-4d %-12s %s\n", cr4_bits[i].bit, cr4_bits[i].label, cr4_bits[i].description);
            }

            unsigned long  field;
            printf("Enter the field number you want to edit : ");
            if (scanf("%lu", &field) != 1) {
                printf("Invalid input. Exiting...\n");
                while ((ch = getchar()) != '\n' && ch != EOF);
                continue; 
            }
            while ((ch = getchar()) != '\n' && ch != EOF);
            if(field<0||field>24){
                printf("Invalid field number.\n");
                continue;
            }
            
            unsigned long  option;
            printf("Enter clear or set (0 = Clear, 1 = Set) : ");
            if (scanf("%lu", &option) != 1) {
                printf("Invalid input. Exiting...\n");
                while ((ch = getchar()) != '\n' && ch != EOF);
                continue; 

            }
            while ((ch = getchar()) != '\n' && ch != EOF);
            if(option != 0 && option !=1){
                printf("Invalid option number.\n");
                continue;
            }

            char buf[4];
            char sure[4];
            printf("Are you sure you want to proceed? Enter yes or no (Note: this operation may cause a kernel panic). : ");
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                printf("Input error\n");
                while ((ch = getchar()) != '\n' && ch != EOF);
                continue;
            }
            if(sscanf(buf,"%s", sure)!=1){
                printf("Invalid number\n");
                continue;
            }
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (strcmp(sure, "yes") == 0) {
                USERDATA user_data_arr[cpu_count];
                for (int i = 0; i < cpu_count; i++) {
                    user_data_arr[i].input[0] = field;
                    user_data_arr[i].input[1] = option;
                    user_data_arr[i].output[0] = 0;
                    user_data_arr[i].output[1] = 0;
                }
                if (ioctl(fd, IOCTL_CMD_CR4_EDIT, user_data_arr) == -1) {
                    perror("ioctl failed to get user address");
                    break;
                }
                printf("Complete editting CR4 register per CPU:\n");
                for (int i = 0; i < cpu_count; i++) {
                    printf("CPU %d : 0x%lX -> 0x%lX\n", 
                        i, 
                        user_data_arr[i].output[0], 
                        user_data_arr[i].output[1]);
                }
            } 
            else {
                continue;
            }
        }
        else if (command_number==2){
            USERDATA user_data_arr[cpu_count];
            for (int i = 0; i < cpu_count; i++) {
                user_data_arr[i].input[0] = 0;
                user_data_arr[i].input[1] = 0;
                user_data_arr[i].output[0] = 0;
                user_data_arr[i].output[1] = 0;
            }

            if (ioctl(fd, IOCTL_CMD_CR4_READ, user_data_arr) == -1) {
                perror("ioctl failed to get user address");
                break;
            }
            printf("Complete reading CR4 register per CPU:\n");
            for (int i = 0; i < cpu_count; i++) {
                printf("CPU %d : 0x%lX\n", 
                    i, 
                    user_data_arr[i].output[1]);
            }

        }
        else{
            printf("Invalid command number.\n");
        }
    }

    return 0;
}
