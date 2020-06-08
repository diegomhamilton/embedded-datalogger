#include "mbed.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"

#define RETRY_SD_INIT_MS 1000

#define ENABLED 1
#define DISABLED 0

/* Comment/Uncomment the following lines to enable/disable debug messages at the console */
// #define DEBUG_PRINT ENABLED
#define DEBUG_PRINT DISABLED

/* LED is currently being used as SD_SCLK */
DigitalOut led(PC_7);
BufferedSerial pc(USBTX, USBRX, 115200);
SDBlockDevice sd(D11, D12, D13, D10);
FATFileSystem fs("sd", &sd);

/* Attach printf() to serial */
FileHandle *mbed::mbed_override_console(int fd)
{
    return &pc;
}

int main() {
    led = 0;
    FILE* fp;
    int32_t sd_init_failed = -1;

    while (sd_init_failed) {
        sd_init_failed = sd.init();
        led = !led;
        if (DEBUG_PRINT) { printf("Trying to initialize SD\t\n"); }
        ThisThread::sleep_for(RETRY_SD_INIT_MS);
    }
    if (DEBUG_PRINT) { printf("SD initialized succesfully\t\n"); }

    while(true) {
        led = 1;
        sleep();
    }

    return 0;
}