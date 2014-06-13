#ifndef POWERBUTTON_H
#define POWERBUTTON_H

#define PB_SYS_PATH "/sys/class/gpio/export"
#define PB_DIR_PATH "/sys/class/gpio/gpio8_pg2/direction"
#define PB_VAL_PATH "/sys/class/gpio/gpio8_pg2/value"

#define PB_BTTN_STATE_RELEASED 49
#define PB_BTTN_STATE_PRESSED  48

int pb_init();
void* pb_handler();

#endif
