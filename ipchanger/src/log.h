#ifndef LOG_H
#define LOG_H

#define LOG_PATH  "/usr/local/lielas/logs/ipchanger.log"

#define LOG_LEVEL_DEBUG   2
#define LOG_LEVEL_WARN    1
#define LOG_LEVEL_ERROR   0

#define LOG_BUF_LEN     5000

void write_log(char* s, int level);


#endif
