#include <stdio.h>
#include <math.h>

int digit_is_Prime(int digit)
{
    if(digit < 2)
        return 0;
    else if(digit % 2 ==0)
        return (digit == 0);

    for(int i = 3; i <= sqrt(digit); i+=2)
        if (digit % i == 0)
            return 0;
    return 1;
}

int find_Divider(int digit)
{
    int sum = 1;
    int number = sqrt(digit);


    for (int i = 2; i <= number; i++) {
        if (digit % i == 0) {
            sum += i;

            if(i != digit/i)
                sum = sum + digit / i;
        }
    }

    if(digit_is_Prime(sum))
        return 1;
    else
        return 0;
}

int main()
{
    printf ( "Intervaly:\n");

    char Znak;
    int first_digit, second_digit;
    while((scanf(" %c", &Znak)!=EOF))
    {
        if((scanf(" %d %d", &first_digit, &second_digit) !=2 ||first_digit < 1 || second_digit < first_digit))
        {
            printf("Nespravny vstup.\n");
            return 1;
        }
        int total = 0;
        if(Znak == '?')
        {
            total = 0;
            for(int i = first_digit; i <= second_digit; i++)
            {
                if(find_Divider(i)) {
                    printf("%d\n", i);
                    total++;
                }
            }
        }
        else if(Znak == '#')
        {
            total = 0;
            for(int i = first_digit; i <= second_digit; i++)
            {
                if(find_Divider(i)) {
                    total++;
                }
            }
        }
        else{
            printf("Nespravny vstup.\n");
            return 1;
        }

        printf("Celkem: %d\n", total);

    }

    return 0;
}
