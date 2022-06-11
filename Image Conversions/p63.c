#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "image_conversion.h"

int main(int argc, char *argv[])
{
  char format[3];
  scanf("%s", format);
  if(strcmp(format, "P6") != 0){
    fprintf(stderr, "Invalid header: File type is different from P6");
    exit(1);
  }

  fprintf(stdout,"P3\n");

  unsigned int width;
  unsigned int height;
  unsigned int depth;

  scanf("%u %u", &width, &height);
  scanf("%u\n", &depth);
  if(depth > 65536 || depth <= 0){
    fprintf(stderr, "Invalid depth value: %u\n", depth);
    exit(1);
  }
  fprintf(stdout,"%u %u\n", width, height);
  fprintf(stdout,"%u\n", depth);
  
  unsigned char temp;
  for(unsigned int accumulator = 0; accumulator < (3 * width * height);
    accumulator++){
    scanf("%c", &temp);
    temp = temp % 256;
    fprintf(stdout,"%hhu ", temp);
  }

  return 0;
}
