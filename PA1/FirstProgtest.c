#include <stdio.h>
#include <math.h>

int main() {

    printf("Zadejte kreditni urok [%%]:\n");

    double kredit = 0, debet = 0;

    if((scanf("%lf",&kredit)!=1 || (kredit<=0)))
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    printf("Zadejte debetni urok [%%]:\n");

    if((scanf("%lf",&debet)!=1 || (debet<=0)))
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    printf("Zadejte transakce:\n");

    int day = 0, money = 0;
    double  balance = 0;
    if((scanf("%d, %d",&day, &money)!=2 || (day<0)))
    {
        printf("Nespravny vstup.\n");
        return 1;
    }
    balance += money;
    int day2 = 0;

    while(money!=0)
    {
        if(scanf("%d, %d", &day2, &money) !=2)
        {
            printf("Nespravny vstup.\n");
            return 1;
        }
        if(day2 <= day)
        {
            printf("Nespravny vstup.\n");
            return 1;
        }

        double change;

        if(balance < 0)
        {
            for(int i = day; i < day2 ; i++ )
            {
                change = trunc((balance * (debet/100))*100)/100;
                balance += change;
            }
        }

        else if(balance > 0)
        {
            for(int i = day; i < day2 ; i++ )
            {
                change = trunc((balance * (kredit/100))*100)/100;
                balance += change;
            }
        }

        balance += money;
        day = day2;
    }

    printf("Zustatek: %.2f\n", balance);
    return 0;
}
