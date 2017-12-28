
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <JSONParser.h>

void JSONParser ::parse(String input)
{
        char str[input.length()];
        memset(str, '\0', sizeof(char) * input.length());
        str[input.length() + 1] = '\0';
        input.toCharArray(str, input.length());
        JSONParser ::parse(str);
}

void JSONParser ::parse(char str[])
{

        int i = 0;
        int len = 0;
        int k = 0;
        int start_index;
        while ('\0' != str[i])
        {

                if ('\"' == str[i]) //start of key
                {
                        i++;
                        while (str[i] == ' ')
                                i++;

                        start_index = i;
                        while (str[i] != '\"')
                        {
                                len++;
                                i++;
                        }
                        JSONParser::pairs[k].key = (char *)malloc((len + 1) * sizeof(char));
                        strncpy(JSONParser::pairs[k].key, str + start_index, len);
                        JSONParser::pairs[k].key[len] = '\0';
                        len = 0;
                }

                if (str[i] == ':') //start of value
                {
                        i++;
                        while (str[i] == ' ' || str[i] == '"')
                                i++;
                        start_index = i;
                        while (str[i] != '"' && str[i] != ',' && str[i] != '}')
                        {
                                len++;
                                i++;
                        }

                        JSONParser::pairs[k].val = (char *)malloc((len + 1) * sizeof(char));
                        strncpy(JSONParser::pairs[k].val, str + start_index, len);
                        JSONParser::pairs[k].val[len] = '\0';
                        len = 0;
                }

                if (',' == str[i])
                {
                        k++;
                        JSONParser::total_pairs++;
                }
                i++;
        }

        JSONParser::total_pairs += 1;

        for (i = 0; i < JSONParser::total_pairs; i++)
        {
                pairs[i].val = trimwhitespace(pairs[i].val);
                pairs[i].key = trimwhitespace(pairs[i].key);
        }
}

char *JSONParser ::trimwhitespace(char *str)
{
        char *end;

        // Trim leading space
        while (isspace((unsigned char)*str))
                str++;

        if (*str == 0) // All spaces?
                return str;

        // Trim trailing space
        end = str + strlen(str) - 1;
        while (end > str && isspace((unsigned char)*end))
                end--;

        // Write new null terminator
        *(end + 1) = 0;

        return str;
}