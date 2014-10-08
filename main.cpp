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

unsigned char read_fan_speed(unsigned char address) {
    usleep(1000);
    outb(READ_EC, EC_CMD);
    usleep(1000);
    outb(address, EC_DATA);
    usleep(1000);
    unsigned char data = inb(EC_DATA);
    usleep(1000);

    return data;
}

void write_fan_speed(unsigned char value, unsigned char address) {
    usleep(1000);
    outb(WRITE_EC, EC_CMD);
    usleep(1000);
    outb(address, EC_DATA);
    usleep(1000);
    outb(value, EC_DATA);
    usleep(1000);
}

void default_mode() {
    write_fan_speed(FAN_AUTO_MOD, FAN_SPEED_REG_LEFT);
    write_fan_speed(FAN_AUTO_MOD, FAN_SPEED_REG_RIGHT);
}

void silence_mode() {
    write_fan_speed(MIN_SPEED, FAN_SPEED_REG_LEFT);
    write_fan_speed(MIN_SPEED, FAN_SPEED_REG_RIGHT);
}

void show_registers() {
    for (unsigned char i = 0; i < 255; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }
        printf("%d\t",read_fan_speed(i));
    }
    printf("\n");
}

int main() {
    if (ioperm(0x62, 5, 1)) {
        perror("You must run programm as root");
        exit(1);
    }

    //silence_mode();
    default_mode();
    printf("%d %d\n",read_fan_speed(FAN_SPEED_REG_LEFT), read_fan_speed(FAN_SPEED_REG_RIGHT));
    printf("Done\n");
    if (ioperm(0x62, 5, 0)) {
        perror("Can't close port");
    }

    return 0;
}