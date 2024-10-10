#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int RandomInt(int low, int high)
{
  return rand() % (high - low + 1) + low;
}

int main(int argc, char **argv)
{
  int i, width, from, to;
  
  if (argc < 4)
  {
    printf("Usage: gen_matrix {width} {from} {to}\n");
    exit(-1);
  }
  
  width = atoi(argv[1]);
  from = atoi(argv[2]);
  to = atoi(argv[3]);
  
  srand(time(NULL));
  
  printf("%i\n", width);
  for (i = 0; i < width * width; i++)
  {
    int value = RandomInt(from, to);
    printf("%8i", value);
    if ( (i + 1) % width == 0)
      printf("\n");
  }
  printf("\n");
}
