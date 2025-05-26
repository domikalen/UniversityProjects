#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct market{
    char *name;
    int number_shelf;
}MARKET;

typedef struct spisok{
    char *name;
    char *name_market;
    int number_list;
    int number_shelf;
}SPISOK;

void tolower_use(char *name){
    int lenght = strlen(name);
    for(int i = 0; i < lenght ; i++ ){
        name[i] = tolower(name[i]);
    }
}

int equal_check(char *list_name, char *market_name){
    char *first=(char*)malloc(sizeof(char)*(strlen(list_name)+1));
    char *second=(char*)malloc(sizeof(char)*(strlen(market_name)+1));

    strcpy(first, list_name);
    tolower_use(first);
    strcpy(second, market_name);
    tolower_use(second);

    if(!strcmp(first, second))
    {
        free(first);
        free(second);
        return 1;
    }
    else
    {
        free(first);
        free(second);
        return 0;
    }
}

int equal_check2(char *list_name, char *market_name){
    char *first=(char*)malloc(sizeof(char)*(strlen(list_name)+1));
    char *second=(char*)malloc(sizeof(char)*(strlen(market_name)+1));

    strcpy(first, list_name);
    tolower_use(first);
    strcpy(second, market_name);
    tolower_use(second);

    char *contain = strstr(second, first);
    if(contain != NULL)
    {
        contain = NULL;
        free(first);
        free(second);
        return 1;
    }
    else {
        contain = NULL;
        free(first);
        free(second);
        return 0;
    }
}

int digit(char *vstup){

    int lenght = strlen(vstup)-1;
    int s = 1;
    while(s < lenght)
    {
        if(!isdigit(vstup[s]))
            return -1;

        s++;
    }
    int index = 1;
    int cislo = 0;
    for(;vstup[index] && (vstup[index] >= '0' && vstup[index] <= '9');){
        cislo = cislo * 10 + (vstup[index] - '0');
        index++;
    }
    return cislo;
}

int main(){

    int size = 10;
    int size2 = 10;
    MARKET *shelfs=(MARKET*)malloc(sizeof(MARKET)*size);
    SPISOK *list=(SPISOK*)malloc(sizeof(SPISOK)*size2);
    int index = -1;
    int index2 = -1;
    int flag;
    int number_shelf = -1, number_list = -1;

    size_t lenght;
    char *tmp_product= NULL;
    int ent = 0;
    int kostyl = 0;
    while(getline(&tmp_product,&lenght,stdin)!=EOF){
        if(tmp_product[1]=='#'&&ent==0)
        {
            printf("Nespravny vstup.\n");
            for(int i = 0; i<=index; i++){
                free(shelfs[i].name);
            }
            for(int i = 0; i<=index2; i++){
                free(list[i].name);
                free(list[i].name_market);
            }
            free(tmp_product);
            free(list);
            free(shelfs);
            return 1;

        }

        if(tmp_product[0]!='\n')
            ent=0;
        if(tmp_product[0] == '#'){
            flag = 1;
            number_shelf++;
            int mal = digit(tmp_product);
            if(mal==-1||mal!=number_shelf){
                printf("Nespravny vstup.\n");
                for(int i = 0; i<=index; i++){
                    free(shelfs[i].name);
                }
                for(int i = 0; i<=index2; i++){
                    free(list[i].name);
                    free(list[i].name_market);
                }
                free(tmp_product);
                free(list);
                free(shelfs);
                return 1;
            }
        }
        else if(tmp_product[0]=='\n'){
            flag = 0;
            number_list++;
            ent++;
            if(ent>1||kostyl==0)
            {
                printf("Nespravny vstup.\n");
                for(int i = 0; i<=index; i++){
                    free(shelfs[i].name);
                }
                for(int i = 0; i<=index2; i++){
                    free(list[i].name);
                    free(list[i].name_market);
                }
                free(tmp_product);
                free(list);
                free(shelfs);
                return 1;
            }
        }
        if(number_shelf<0)
        {
            printf("Nespravny vstup.\n");
            for(int i = 0; i<=index; i++){
                free(shelfs[i].name);
            }
            for(int i = 0; i<=index2; i++){
                free(list[i].name);
                free(list[i].name_market);
            }
            free(tmp_product);
            free(list);
            free(shelfs);
            return 1;
        }
        if((flag == 1)&&((tmp_product[0] != '#'))){
            index++;
            if(index==size){
                size *= 2;
                shelfs=(MARKET*)realloc(shelfs,sizeof(*shelfs)*size);
            }
            shelfs[index].name = (char*)malloc(sizeof(char) * (strlen(tmp_product)+1));
            int last = strlen(tmp_product)-1;
            if(tmp_product[last]=='\n')
                tmp_product[last]='\0';
            strcpy(shelfs[index].name, tmp_product);
            shelfs[index].number_shelf = number_shelf;
        }

        else if((flag == 0)&&tmp_product[0]!='\n'){
            index2++;
            if(index2==size2){
                size2 *= 2;
                list=(SPISOK*)realloc(list,sizeof(*list)*size2);
            }
            list[index2].name = (char*)malloc(sizeof(char) * (strlen(tmp_product)+1));
            int last = strlen(tmp_product)-1;
            if(tmp_product[last]=='\n')
                tmp_product[last]='\0';
            strcpy(list[index2].name, tmp_product);
            list[index2].number_list = number_list;
            list[index2].name_market=NULL;
        }
        kostyl++;

    }
    if(number_list<0)
    {
        printf("Nespravny vstup.\n");
        for(int i = 0; i<=index; i++){
            free(shelfs[i].name);
        }
        for(int i = 0; i<=index2; i++){
            free(list[i].name);
            free(list[i].name_market);
        }
        free(tmp_product);
        free(list);
        free(shelfs);
        return 1;

    }
    int flag2 = 0;
    for(int i = 0; i <= index2; i++){
        flag2=0;
        for(int j = 0; j <= index; j++){
            if(equal_check(list[i].name, shelfs[j].name)){
                list[i].number_shelf = shelfs[j].number_shelf;
                list[i].name_market = (char*)malloc(sizeof(char) * (strlen(shelfs[j].name)+1));
                strcpy(list[i].name_market, shelfs[j].name);
                flag2 = 1;
            }
            if(flag2==1)
                break;
        }
        for(int j = 0; j <= index; j++){
            if(equal_check2(list[i].name, shelfs[j].name)&&(list[i].name_market==NULL)){
                list[i].number_shelf = shelfs[j].number_shelf;
                list[i].name_market = (char*)malloc(sizeof(char) * (strlen(shelfs[j].name)+1));
                strcpy(list[i].name_market, shelfs[j].name);
                flag2 = 1;
            }
            if(flag2==1)
                break;
        }

    }

    int count = 0;
    for(int i = 0; i <= number_list ; i++)
    {
        printf("Optimalizovany seznam:\n");
        int flag = 0;
        count = 0;
        for(int j = 0; j <= number_shelf; j++)
        {
            for(int k = 0; k <= index2 ; k++)
            {
                if(list[k].number_list==i&&list[k].name_market!=NULL)
                {
                    if(list[k].number_shelf==j){
                        printf(" %d. %s -> #%d %s\n", count, list[k].name, list[k].number_shelf, list[k].name_market);
                        count++;
                    }
                }
            }
        }
        for(int j = 0; j <= number_shelf; j++)
        {
            for(int k = 0; k <= index2 ; k++)
            {
                if(list[k].number_list==i&&list[k].name_market==NULL)
                {
                    printf(" %d. %s -> N/A\n", count, list[k].name);
                    flag = 1;
                    count++;
                }
            }
            if(flag == 1)
                break;
        }

    }


    for(int i = 0; i<=index; i++){
        free(shelfs[i].name);
    }
    for(int i = 0; i<=index2; i++){
        free(list[i].name);
        free(list[i].name_market);
    }
    free(tmp_product);
    free(list);
    free(shelfs);
    return 0;
}