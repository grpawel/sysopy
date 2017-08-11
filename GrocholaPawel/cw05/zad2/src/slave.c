#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <time.h>
#include <math.h>

double frandom_incl(double min, double max)
{
    return rand() / (RAND_MAX / (fabs(min) + fabs(max))) - fabs(min);
}

int iters(int K, double complex start)
{
    double complex current = 0;
    int iters = 0;
    while(cabs(current) < 2 && iters < K) {
        current = cpow(current, 2+0*I);
        current += start;
        iters++;
    }
    return iters;
}

double complex generate_point()
{
    double re = frandom_incl(-2., 1.);
    double im = frandom_incl(-1., 1.);
    return re + im * I;
}
int main(int argc, char* argv[])
{
    if(argc != 4) {
        perror("3 arguments required: path, N, K.\n");
        exit(EXIT_FAILURE);
    }
    int N = atoi(argv[2]);
    int K = atoi(argv[3]);
    char* path = argv[1];
    srand(time(NULL));
    FILE* pipe = fopen(path, "w");
    for(int i=0; i < N; i++) {
        double complex point = generate_point();
        int it = iters(K, point);
        fprintf(pipe, "%.16f %.16f %d\n", creal(point), cimag(point), it);
        fflush(pipe);
    }
    fclose(pipe);
    return 0;
}
