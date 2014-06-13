#ifndef RESETBUTTON_H
#define RESETBUTTON_H

#define RB_SYS_PATH "/sys/class/gpio/export"
#define RB_DIR_PATH "/sys/class/gpio/gpio9_pg0/direction"
#define RB_VAL_PATH "/sys/class/gpio/gpio9_pg0/value"

#define RB_BTTN_STATE_RELEASED 49
#define RB_BTTN_STATE_PRESSED  48

int rb_init();
void* rb_handler();

#endif
