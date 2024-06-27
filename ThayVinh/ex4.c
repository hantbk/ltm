#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>

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

int compare(const struct dirent** item1, const struct dirent** item2)
{
    if ((*item1)->d_type == (*item2)->d_type)
    {
        return 0;
    }else
    {
        if ((*item1)->d_type == DT_DIR)
            return -1;
        else
            return 1;
    }
}

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        Append(&s, "<html>");
        struct dirent** output = NULL;
        int n = scandir(argv[1], &output, NULL, compare);
        if (n > 0)
        {
            //FILE* f = fopen("myscan.html", "wt");
            FILE* f = fopen("myscan.html","wb");
            for (int i = 0;i < n;i++)
            {
                if (output[i]->d_type == DT_REG)
                {
                    Append(&s, "<i>");
                    Append(&s, output[i]->d_name);
                    Append(&s, "</i><br>");
                }
                if (output[i]->d_type == DT_DIR)
                {
                    Append(&s, "<b>");
                    Append(&s, output[i]->d_name);
                    Append(&s, "</b><br>");
                }
            }
            Append(&s, "</html>");
            //fprintf(f, "%s", s);
            fwrite(s, sizeof(char), strlen(s), f);
            fclose(f);
            free(s);
            s = NULL;
        }else
            printf("No item found!\n");
    }else
        printf("Missing parameter!\n");
}