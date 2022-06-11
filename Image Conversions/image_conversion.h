#ifndef _IMAGE_CONVERSION_H_
#define _IMAGE_CONVERSION_H_

#include <stdio.h>
#include <stdint.h>

struct rgb {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

struct image {
  uint32_t w;
  uint32_t h;
  struct rgb *pixels; 
};

#endif
