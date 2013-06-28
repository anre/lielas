#ifndef RTC_H
#define RTC_H

#define RTC_ERROR -1;
#define RTC_TIME_NOT_VALID  -2


#define RTC_STATE_UNINITIALIZED 0
#define RTC_STATE_OK            1
#define RTC_STATE_NOT_SYNCED    2

int rtc_init();
int rtc_get_state();

#endif

