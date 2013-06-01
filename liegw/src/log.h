#ifndef LOG_H
#define LOG_H

#define LOG_LEVEL_ERROR		0
#define LOG_LEVEL_WARN		1
#define LOG_LEVEL_DEBUG		2


#define LOG_TO_STDOUT		0
#define LOG_TO_FILE			1
#define LOG_TO_SYSLOG		2

#define LOG_BUF_LEN     5000

void lielas_log(unsigned char *msg, int level);

#endif
