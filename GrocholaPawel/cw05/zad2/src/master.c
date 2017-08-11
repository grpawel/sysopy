#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#define OUT
void read_pipe(FILE* pipe, OUT int** T, int R)
{
    while(!feof(pipe)) {
        double re, im;
        int val;
        fscanf(pipe, "%lf %lf %d\n", &re, &im, &val);
        int x = (re + 2) / 3 * R;
        int y = (im + 1) / 2 * R;
        if(x >= R) {
            x = R - 1;
        }
        if(y >= R) {
            y = R - 1;
        }
        T[x][y] = val;
    }
}
void print_data(FILE* filedata, int** T, int R)
{
    for(int i=0; i < R; i++) {
        for(int j = 0; j < R; j++) {
            fprintf(filedata, "%d %d %d\n", i, j, T[i][j]);
        }
    }
}

void show_gnuplot(int R)
{
    FILE* gnuplot = popen("gnuplot", "w");
    fprintf(gnuplot, "set view map\n");
    fprintf(gnuplot, "set xrange [0:%d]\n", R);
    fprintf(gnuplot, "set yrange [0:%d]\n", R);
    fprintf(gnuplot, "plot 'data' with image\n");
    fflush(gnuplot);
    getc(stdin);
    fclose(gnuplot);
}
int main(int argc, char* argv[])
{
    if(argc != 3) {
        perror("Required 2 arguments: path, R\n");
        exit(EXIT_FAILURE);
    }
    char* path = argv[1];
    int R = atoi(argv[2]);
    int** T = malloc(sizeof(int*) * R);
    for(int i=0; i < R; i++) {
        T[i] = calloc(R, sizeof(int));
    }
    /*
    if(mkfifo(path, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    */
    FILE* pipe = fopen(path, "r");
    if(pipe == NULL) {
        perror("fopen\n");
        exit(EXIT_FAILURE);
    }
    read_pipe(pipe, T, R);
    fclose(pipe);
    FILE* filedata = fopen("data", "w");
    print_data(filedata, T, R);
    fclose(filedata);
    show_gnuplot(R);

}
