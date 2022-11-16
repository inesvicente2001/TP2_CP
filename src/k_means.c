#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <omp.h>
#include "../include/k_means.h"

size_t N;
short int K;
int T;

void init_vector(struct point point[N]) {

    size_t i;

    srand(10);

    for(i = 0; i < N; i++) {
        point[i].x = (float) rand() / (float) RAND_MAX;
        point[i].y = (float) rand() / (float) RAND_MAX;
        point[i].cluster = 0;
    }

}

void init_k_clusters(struct point point[N], struct cluster cluster[K]) {

    int i;

    for(i = 0; i < K; i++) {
        cluster[i].x = point[i].x;
        cluster[i].y = point[i].y;
    }
}

bool atribute_sample(int clN[K], struct point point[N], struct cluster cluster[K]) {
    
    int i;
    //short int j;
    /*variáveis que vão guardar elementos dos arrays e structs para evitar acessos à memória*/
    //float pointix, clusterjx, pointiy, clusterjy;
    /*
    clx = array que vai ter a soma de todas as coordenadas x pertencentes a um determinado cluster
    cly = array que vai ter a soma de todas as coordenadas y pertencentes a um determinado cluster
    clN = array que vai ter o número total de elementos em cada cluster
    */
    float clx[K], cly[K];
    int clNtmp[K];
    //float minor_dist, dist;
    //short int minor_cluster=0;
    /*bool que diz se houve algum elemento que tenha trocado de cluster*/
    bool end = true;

    for (i = 0 ; i < K ; i++) {
        clNtmp[i] = 0;
        clx[i] = 0;
        cly[i] = 0;
    }

    #pragma omp parallel for reduction (+: clNtmp, clx, cly) schedule(guided) num_threads(T)
    for (i = 0; i < N; i++) {

        float minor_dist, dist;
        short int minor_cluster;
        
        float pointix = point[i].x;
        float pointiy = point[i].y;
        float clusterjx = cluster[0].x;
        float clusterjy = cluster[0].y;
        /*cálculo da distância inicial fora do loop para termos um valor inicial para o minor_dist e minor_cluster*/
        minor_dist = (pointix - clusterjx) * (pointix - clusterjx) + (pointiy - clusterjy) * (pointiy - clusterjy);
        //minor_cluster = 0;
        //minor_dist = (point[i].x - cluster[0].x) * (point[i].x - cluster[0].x) + (point[i].y - cluster[0].y) * (point[i].y - cluster[0].y);
        minor_cluster = 0;
        short int j;
        for (j = 1; j < K; j++) {
            clusterjx = cluster[j].x;
            clusterjy = cluster[j].y;
            //dist = (point[i].x - cluster[j].x) * (point[i].x - cluster[j].x) + (point[i].y - cluster[j].y) * (point[i].y - cluster[j].y);
            dist = (pointix - clusterjx) * (pointix - clusterjx) + (pointiy - clusterjy) * (pointiy - clusterjy);
            if(dist < minor_dist){
                minor_dist = dist;
                minor_cluster = j;
            }
        }

        //#pragma omp reduction (+: clN[minor_cluster], clx[minor_cluster], cly[minor_cluster])
        clNtmp[minor_cluster]++;
        //clx[minor_cluster] += point[i].x;
        //cly[minor_cluster] += point[i].y;
        clx[minor_cluster] += pointix;
        cly[minor_cluster] += pointiy;
        /*Caso haja algum ponto que tenha trocado de cluster o bool end passa a false e haverá outra iteração desta função*/
        if (point[i].cluster != minor_cluster) {
            end = false;
            point[i].cluster = minor_cluster;
        }


        //printf("%ld   %d\n",i,omp_get_thread_num());
    }

    /*cálculo do centroide de cada cluster*/
    for (i = 0 ; i < K ; i++) {
        clN[i] = clNtmp[i];
        cluster[i].x = clx[i] / (float)clN[i];
        cluster[i].y = cly[i] / (float)clN[i];
    }

    return end;
}

int main(int argc, char * argv[]) {


    if (argc < 3 || argc > 4 || strcmp(argv[0],"./bin/k_means") != 0) {
            printf("Input error!\n");
            return 1;
    }

    N = atoi(argv[1]);
    K = atoi(argv[2]);

    if ( argc == 3 ) T = 1;
    else T = atoi(argv[3]);

    int i;
    struct point * point = malloc(sizeof(struct point) * N);
    struct cluster * cluster = malloc(sizeof(struct cluster) * K);
    int * clN = malloc(sizeof(int) * K);

    init_vector(point);

    /*coloca os valores x e y dos primeiros K pontos na estrutura dos cluster*/
    init_k_clusters(point, cluster);

    bool end = 0; /*flag que indica se já convergiu*/
    size_t iterations = 0; /*número de iterações*/

    while (!end && iterations < 20) {
        end = atribute_sample(clN, point, cluster);
        if (!end) iterations++;
    }

    printf("N = %ld, K = %d\n", N, K);

    for (i = 0; i < K; i++) {
        printf("Center: (%.3f, %.3f) : Size: %d\n", (double)cluster[i].x, (double)cluster[i].y, clN[i]);
    }

    printf("Iterations: %zu\n", iterations);

    return 0;

}