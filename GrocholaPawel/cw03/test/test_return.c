#include <stdio.h>

int main(int argc, char* argv[])
{
  printf("Args:");
  for(int i=1; i < argc; i++) {
    printf(" %s", argv[i]);
	}
	return argc - 1;
  printf("\n");
}
