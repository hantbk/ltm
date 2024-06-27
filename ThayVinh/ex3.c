#include <stdio.h>
#include <string.h>
#include <malloc.h>
typedef char* STRING;

STRING s = NULL;

void Append(STRING* v, const STRING x)
{
    STRING tmp = *v;
    int oldLen = tmp == NULL ? 0 : strlen(tmp);
    int newLen = oldLen + strlen(x) + 1;
    tmp = (STRING)realloc(tmp, newLen);
    memset(tmp + oldLen, 0, newLen - oldLen);
    sprintf(tmp + strlen(tmp), "%s", x);
    *v = tmp;
}

int main()
{
    Append(&s, "Xin ");
    Append(&s, "chao ");
    Append(&s, "cac ");
    Append(&s, "ban!");
    printf("%s", s);
    free(s);
    s = NULL;
}