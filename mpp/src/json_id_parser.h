#ifndef __JSON_ID_PARSER__
#define __JSON_ID_PARSER__

#ifdef __cplusplus
extern "C" {
#endif

#define PER_ID_STRING "pid"

struct peripheral
{
    char *json_data;
    char *id;
};

extern struct peripheral peripherals[1];
extern int total_peripherals;
extern char *key;

int parse(const char str[]);

#ifdef __cplusplus
}
#endif

#endif