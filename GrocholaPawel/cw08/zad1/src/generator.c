#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RECORD_SIZE 1024
#define RECORD_TEXT_LEN ((RECORD_SIZE) - sizeof(int))

struct record {
    int id;
    char text[RECORD_TEXT_LEN];
};

int random_incl(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

int main(int argc, char* argv[])
{
    if(argc != 3) {
        fprintf(stderr, "Required arguments: file name, number of records");
        exit(EXIT_FAILURE);
    }
    char* filename = argv[1];
    int records_num = atoi(argv[2]);
    FILE* fp = fopen(filename, "w+");
    if(!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    struct record record;
    srand(time(NULL));
    for(int i = 0; i < records_num; i++) {
        record.id = i;
        for(size_t i = 0; i < sizeof(record.text); i++) {
            record.text[i] = random_incl('a', 'z');
        }
        fwrite(&record, sizeof(char), RECORD_SIZE, fp);
    }
    fclose(fp);
    return EXIT_SUCCESS;
}
