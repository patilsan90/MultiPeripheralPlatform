#ifndef __JSON_PARSER__
#define __JSON_PARSER__
#include <Arduino.h>
#include <string.h>
typedef struct jason
{
  char *key;
  char *val;
} key_val;

class JSONParser
{
public:
  int total_pairs;
  key_val pairs[20];
  void parse(String input);
  void parse(char input[]);

private:
  char *trimwhitespace(char *str);
};

#endif
