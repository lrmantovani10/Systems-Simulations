#include "image_conversion.h"

int rgbEquals(struct rgb c1, struct rgb c2)
{
  if(c1.r == c2.r && c1.g == c2.g && c1.b == c2.b)
    return 1;
  else
    return 0;
}

int checkGrayscale(struct image *img)
{
  for(unsigned int i = 0; i < ((unsigned int)
    ((img->w) * (img->h))); i++){
      struct rgb current_col = img->pixels[i];
      if(!(current_col.r == current_col.g && 
        current_col.g == current_col.b))
          return 0;
  }

  return 1;
}

void skipCharacters(unsigned int a, unsigned int b){
  for(unsigned int i = a; i < b; i++)
    getchar();
}

unsigned int get4bytes(){
  unsigned int run_num1 = getchar();
  unsigned int run_num2 = getchar();
  unsigned int run_num3 = getchar();
  unsigned int run_num4 = getchar();
  unsigned int run_num = ((run_num1 << 24) & 0Xff000000) + 
    ((run_num2 << 16) & 0Xff0000) + ((run_num3 << 8) & 0Xff00) +
    (run_num4 & 0Xff);
  
  return run_num;
}

void print4bytes(unsigned int n){
  fprintf(stdout, "%c", (n >> 24) & 0Xff);
  fprintf(stdout, "%c", (n >> 16) & 0Xff);
  fprintf(stdout, "%c", (n >> 8) & 0Xff);
  fprintf(stdout, "%c", n & 0Xff);
}

void print2bytes(short unsigned int n){
  fprintf(stdout, "%c", (n >> 8) & 0Xff);
  fprintf(stdout, "%c", n & 0Xff);
}
