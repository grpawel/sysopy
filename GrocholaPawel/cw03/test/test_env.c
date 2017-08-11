#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
  if(argc != 2) {
    return -1;
  }
  char* var;
  var = getenv(argv[1]);
  if(var != NULL) {
    printf("Value of '%s': '%s'\n",argv[1], var);
  }
  else {
    printf("There is no variable '%s'.\n", argv[1]);
  }
  return 0;
}
