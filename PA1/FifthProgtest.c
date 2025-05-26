#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

typedef struct Flight{
    char Name[199];
    double x;
    double y;
}FLIGHT;
typedef struct Distance{
    int index_1;
    int index_2;
    double dis;
}DISTANCE;

void output_funct(DISTANCE *data_2, FLIGHT *data_1, int count){
    printf("Vzdalenost nejblizsich letadel: %0.6f\n", data_2[0].dis);
    printf("Nalezenych dvojic: %d\n", count);
    for(int i = 0; i < count; i++)
        printf("%s - %s\n", data_1[data_2[i].index_1].Name, data_1[data_2[i].index_2].Name);
}

int main(){
    printf("Pozice letadel:\n");
    int size_2 = 2;
    FLIGHT* data_1 = (FLIGHT*)malloc(size_2*sizeof(FLIGHT));
    double x, y;
    char name[200];
    int index = -1;
    int check;
    while((check=(scanf(" %lf , %lf : %199s", &x, &y, name))) != EOF) {
        if(check != 3 || check == -1){
            printf("Nespravny vstup.\n");
            free(data_1);
            return 1;
        }

        index++;
        if(index==size_2){
            size_2 *= 2;
            data_1 = (FLIGHT *) realloc(data_1, size_2 * sizeof(*data_1));
        }
        strcpy(data_1[index].Name, name);
        data_1[index].x = x;
        data_1[index].y = y;
    }
    float min = sqrt(pow((data_1[1].x-data_1[0].x), 2)+pow((data_1[1].y-data_1[0].y), 2));
    for(int i = 0; i <= index; i++){
        for(int j = i+1, k = index; j <= index ; j++, k--){

            float number = sqrt(pow((data_1[j].x-data_1[i].x), 2)+pow((data_1[j].y-data_1[i].y), 2));
            if(number < min){
                min = number;
            }
            if(j==k)
                break;

            number = sqrt(pow((data_1[k].x - data_1[i].x), 2) + pow((data_1[k].y - data_1[i].y), 2));
            if(number < min){
                min = number;
            }
            if(k-j==1)
                break;

        }
    }

    int index2 = -1, size = 2;
    int count = 0;
    DISTANCE* data_2 = (DISTANCE*)malloc(size*sizeof(DISTANCE));
    for(int i = 0; i <= index; i++){
        for(int j = i+1, k = index; j <= index ; j++, k--){

            float number = sqrt(pow((data_1[j].x-data_1[i].x), 2)+pow((data_1[j].y-data_1[i].y), 2));
            if(fabs(min-number)<=DBL_EPSILON*1e-100) {
                index2++;
                if (index2 >= size) {
                    size *= 2;
                    data_2 = (DISTANCE *) realloc(data_2, size * sizeof(*data_2));
                }
                data_2[index2].index_1 = i;
                data_2[index2].index_2 = j;
                data_2[index2].dis = number;
                count++;
            }
            if(j==k)
                break;
            number = sqrt(pow((data_1[k].x - data_1[i].x), 2) + pow((data_1[k].y - data_1[i].y), 2));
            if(fabs(min-number)<=DBL_EPSILON*1e-100) {
                index2++;
                if (index2 >= size) {
                    size *= 2;
                    data_2 = (DISTANCE *) realloc(data_2, size * sizeof(*data_2));
                }
                data_2[index2].index_1 = i;
                data_2[index2].index_2 = k;
                data_2[index2].dis = number;
                count++;
            }
            if(k-j==1)
                break;
        }
    }

    if(index<2){
        printf("Nespravny vstup.\n");
        free(data_2);
        free(data_1);
        return 1;
    }
    output_funct(data_2, data_1, index2+1);
    free(data_2);
    free(data_1);
    return 0;
}