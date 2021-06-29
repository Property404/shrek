#include "serial.h"
SerialDriver serial_driver;

void putchar(char c){
    serial_driver.putchar(c);
};

bool testchar(void) {
    return serial_driver.testchar();
}

int getchar(void){
    return serial_driver.getchar();
}
