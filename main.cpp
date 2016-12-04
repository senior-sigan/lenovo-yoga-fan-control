#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/syslog.h>

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
#define SLEEP_TIME 10000 // Wait until EC parse command in microseconds
#define UPDATE_TIME 5 // How often check temp and fix fan speed in seconds
#define READ_ERROR_TEMP 80 // Assume temperature to this when read error
#define SYS_HWMON_VALUE_SCALE 1000 // Scale factor for sys hwmon output values

//               TEMP IN CELSIUS, SPEED 255-1 (lower value is faster RPM)
unsigned char temp_table[] = { 0, MIN_SPEED,
                              55, 200,
                              60, 180,
                              65, 160,
                              70, 130,
                              75, 90,
                              80, 20,
                   CRITICAL_TEMP, MAX_SPEED
};

bool has_permissions = false;
bool g_terminated = false;
const char PID_FILE[] = "/var/run/yoga_fan.pid";

unsigned char read_register(unsigned char address) {
    usleep(SLEEP_TIME);
    outb(READ_EC, EC_CMD);
    usleep(SLEEP_TIME);
    outb(address, EC_DATA);
    usleep(SLEEP_TIME);
    unsigned char data = inb(EC_DATA);
    usleep(SLEEP_TIME);

    return data;
}

unsigned char read_temp() {
    return read_register(CPU_TEMP_0);
}

unsigned char read_temp_sys() {
    int res = 0;
    double value;
    FILE *f;

    if((f = fopen("/sys/class/hwmon/hwmon1/temp2_input", "r"))) {
        errno = 0;
        res = fscanf(f, "%lf", &value);

        if (res == EOF && errno == EIO) {
            return READ_ERROR_TEMP;
        }
        else if (res != 1) {
            return READ_ERROR_TEMP;
        }

        res = fclose(f);
        if (res == EOF && errno == EIO) {
            return READ_ERROR_TEMP;
        }

        value /= SYS_HWMON_VALUE_SCALE;
        return (unsigned char)value;
    }
    else {
        return READ_ERROR_TEMP;
    }
}

void write_register(unsigned char value, unsigned char address) {
    usleep(SLEEP_TIME);
    outb(WRITE_EC, EC_CMD);
    usleep(SLEEP_TIME);
    outb(address, EC_DATA);
    usleep(SLEEP_TIME);
    outb(value, EC_DATA);
    usleep(SLEEP_TIME);
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
    unsigned char temp = read_temp_sys();
    // syslog(LOG_NOTICE, "Read temp: %d", temp);
    if (temp < 0) {
        silence_mode();
        return;
    }
    for (unsigned int i = 0; i < sizeof(temp_table); i += 2) {
        if (temp < temp_table[i]) {
            write_register(temp_table[i+1], FAN_SPEED_REG_RIGHT);
            write_register(temp_table[i+1], FAN_SPEED_REG_LEFT);
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

void handler(int signal) {
    syslog(LOG_NOTICE, "Set default fan controlling.");
    if (has_permissions) {
        default_mode();

        if (ioperm(0x62, 5, 0)) {
            syslog(LOG_ERR, "IOperm error. Can't return access on ports 0x62 to 0x66");
        }
        has_permissions = false;
    }

    g_terminated = true;
}

static void start_daemon() {
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0 ) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGCHLD,SIG_IGN); /* ignore child */
    signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP,handler); /* catch hangup signal */
    signal(SIGTERM,handler); /* catch kill signal */

    openlog("Lenovo yoga fan control", LOG_PID, LOG_DAEMON);
}

void SetPidFile(const char* Filename) {
    FILE* f;

    f = fopen(Filename, "w+");
    if (f) {
        fprintf(f, "%u", getpid());
        fclose(f);
    }
}

int main() {
    start_daemon();
    SetPidFile(PID_FILE);

    if (ioperm(0x62, 5, 1)) {
        syslog(LOG_ERR, "IOperm error. Can't get access to ports 0x62 to 0x66");
        exit(EXIT_FAILURE);
    }
    has_permissions = true;
    syslog(LOG_NOTICE, "Open ports");

    while(!g_terminated) {
        auto_mod();
        sleep(UPDATE_TIME);
    }

    syslog(LOG_NOTICE, "Exiting");
    closelog();
    exit(EXIT_SUCCESS);
}
