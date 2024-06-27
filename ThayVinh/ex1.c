#include <stdio.h>
#include <string.h>

int main()
{
    char s[1024];
    fgets(s, sizeof(s) - 1, stdin);
    //char* s = "The house  nextdoor is   very old";
    int i = 0;
    while (0 == 0)
    {
        while (i < strlen(s) && s[i] == ' ') i++;
        if (i < strlen(s))
        {
            while (i < strlen(s) && s[i] != ' ')
            {
                printf("%c", s[i]);
                i++;
            }
            printf("\n");
        }else
            break;
    }
}