#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "json_id_parser.h"

int total_peripherals = 0;
char *key = PER_ID_STRING;
struct peripheral peripherals[1];

int parse(const char str[])
{

    int i = 0;
    int k = 0;
    int jd_st;
    int jd_end;

    int key_st;
    int key_end;

    int val_st;
    int val_end;

    char *temp;
    total_peripherals = 0;

    while (str[i] != '\0')
    {
        if (str[i] == '{') // start of json data
        {
            jd_st = i++;

            while (str[i] != '}')
            {
                if (str[i] == '\"')
                { //start of key
                    i++;
                    while (str[i] == ' ')
                        i++;
                    key_st = i;
                    while (str[i] != '\"')
                        i++;
                    key_end = i;
                }

                if (str[i] == ':')
                {
                    i++;
                    while (str[i] == ' ' || str[i] == '"')
                        i++;
                    val_st = i;

                    while (str[i] != '"' && str[i] != ',' && str[i] != '}')
                        i++;
                    val_end = i;
                }

                if (str[i] == ',' || str[i] == '}')
                {
                    temp = (char *)malloc(sizeof(char) * (key_end - key_st));
                    if (temp == NULL)
                        return -100;

                    strncpy(temp, str + key_st, (key_end - key_st));
                    temp[(key_end - key_st)]='\0';

                    if (!strcmp(temp, key))
                    {
                        peripherals[k].id = (char *)malloc(sizeof(char) * ((val_end - val_st) + 1));
                        if (peripherals[k].id == NULL)
                            return -100;

                        strncpy(peripherals[k].id, str + val_st, (val_end - val_st));
                        peripherals[k].id[(val_end - val_st)] = '\0';
                    }
                }

                if (str[i] != '}')
                    i++;
            }
            jd_end = i;
            peripherals[k].json_data = (char *)malloc(sizeof(char) * ((jd_end - jd_st + 2) + 1));
            strncpy(peripherals[k].json_data, str + jd_st, (jd_end - jd_st + 1));
            peripherals[k].json_data[(jd_end - jd_st + 1)] = '\0';
        }

        if (str[i] == ',')
        {
            k++;
            total_peripherals++;
        }
        i++;
    }

    total_peripherals += 1;

    return 0;
}