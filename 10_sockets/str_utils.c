#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h>

char * strupr (char * str) 
{
    int len = strlen(str);
    char * res = malloc(len + 1);
    for (int i = 0; i < len; i++)
        res[i] = toupper(str[i]);
    res[len] = 0;
    return res;
}

char * strtrim(char * str)
{
  char * end;
  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;
  // Write new null terminator character
  end[1] = '\0';

  return str;
}

char* itoa(int val, int base)
{
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i+1];	
}