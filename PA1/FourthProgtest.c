#include <stdio.h>
#include <stdlib.h>

int main(){
    printf("Posloupnost:\n");
    int size=10, size_2 = 10000;
    int * array_Numbers=(int*)malloc(sizeof(*array_Numbers)*size);
    int * array_Intervals=(int*)calloc(sizeof(*array_Intervals)*size_2, sizeof(*array_Intervals));
    int Number, Index = -1;
    int lol, count_dvojic = 0;
    while((lol = (scanf("%d", &Number)))!= EOF){
        if(lol != 1 || lol ==- 1){
            printf("Nespravny vstup.\n");
            free(array_Numbers);
            free(array_Intervals);
            return 1;
        }
        Index++;
        if(Index==2000){
            printf("Nespravny vstup.\n");
            free(array_Numbers);
            free(array_Intervals);
            return 1;
        }
        if(Index==size){
            size=size*3+10;
            array_Numbers=(int *)realloc(array_Numbers,sizeof(*array_Numbers)*size);
        };
        array_Numbers[Index]=Number;
        if(Index!=0){
            int sum = Number;
            for(int i = Index; i >= 1; i--){
                sum += array_Numbers[i-1];
                if(sum >= size_2){
                    size_2 = sum + 10000;
                    array_Intervals = (int *) realloc(array_Intervals, sizeof(*array_Intervals) * size_2);
                }
                array_Intervals[sum]++;
                if(array_Intervals[sum] > 1)
                    count_dvojic+=array_Intervals[sum]-1;
            }
        }
    }
    if(Index==-1){
        printf("Nespravny vstup.\n");
        free(array_Numbers);
        free(array_Intervals);
        return 1;
    }
    printf("Pocet dvojic: %d\n", count_dvojic);
    free(array_Numbers);
    free(array_Intervals);
    return 0;
}
