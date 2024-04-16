#include <stdio.h>

int main(void)
{

    if (100 % 11 == 0)
    {
        printf("múltiplo\n");
    }
    else
    {
        printf("no múltiplo\n");
    }

    int n = 100 % 11;

    printf("Resto:  %i \n ", n);

    return 0;
}
