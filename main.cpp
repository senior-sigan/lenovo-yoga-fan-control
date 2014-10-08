#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FAN_SPEED_REG_LEFT 0xf2
#define FAN_SPEED_REG_RIGHT 0xf3
#define FAN_AUTO_MOD 0x0
#define MIN_SPEED 0xff
#define MAX_SPEED 0x01
#define READ_EC 0x80
#define WRITE_EC 0x81
#define EC_CMD 0x66
#define EC_DATA 0x62
#define CPU_TEMP_0 0x57
#define CRITICAL_TEMP 85

//odd index is fan speed(1..255), even index is temp. Critical temp is 85
unsigned char temp_table[] = {0, MIN_SPEED, 55, 0xE0, 60, 0xAF, 65, 0x7F, 70, 0x4B, 75, 0x33, 80, 0x1A, CRITICAL_TEMP, MAX_SPEED};

unsigned char read_register(unsigned char address) {
    usleep(1000);
    outb(READ_EC, EC_CMD);
    usleep(1000);
    outb(address, EC_DATA);
    usleep(1000);
    unsigned char data = inb(EC_DATA);
    usleep(1000);

    return data;
}

unsigned char read_temp() {
    return read_register(CPU_TEMP_0);
}

void write_register(unsigned char value, unsigned char address) {
    usleep(1000);
    outb(WRITE_EC, EC_CMD);
    usleep(1000);
    outb(address, EC_DATA);
    usleep(1000);
    outb(value, EC_DATA);
    usleep(1000);
}

void default_mode() {
    write_register(FAN_AUTO_MOD, FAN_SPEED_REG_LEFT);
    write_register(FAN_AUTO_MOD, FAN_SPEED_REG_RIGHT);
}

void silence_mode() {
    write_register(MIN_SPEED, FAN_SPEED_REG_LEFT);
    write_register(MIN_SPEED, FAN_SPEED_REG_RIGHT);
}

void auto_mod() {
    unsigned char temp = read_temp();
    if (temp < 0) {
        silence_mode();
        return;
    }
    for (unsigned int i = 0; i < sizeof(temp_table); i += 2) {
        if (temp < temp_table[i]) {
            write_register(temp_table[i-1], FAN_SPEED_REG_RIGHT);
            write_register(temp_table[i-1], FAN_SPEED_REG_LEFT);
            return;
        }
    }
}

void show_registers() {
    for (unsigned char i = 0; i < 255; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }
        printf("%d\t", read_register(i));
    }
    printf("\n");
}

int main() {
    if (ioperm(0x62, 5, 1)) {
        perror("You must run programm as root");
        exit(1);
    }

    //silence_mode();
    //default_mode();
    while(true) {
        auto_mod();
        printf("temp=%d\n%d %d\n", read_temp(), read_register(FAN_SPEED_REG_LEFT), read_register(FAN_SPEED_REG_RIGHT));
        sleep(5);
    }
    printf("Done\n");
    if (ioperm(0x62, 5, 0)) {
        perror("Can't close port");
    }

    return 0;
}
