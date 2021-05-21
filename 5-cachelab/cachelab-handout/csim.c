/*19307130248 刘慕梵*/
#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include <unistd.h>

typedef struct cache_index//cache index
{
    unsigned long addr;
    int active, hittime;//recent hit or miss or eviction time
}ci_t;

unsigned long xatol(char* s)//get address in file
{
    unsigned long num = 0, i = 3;
    unsigned char c;
    if(s[0] != ' ')
        i = 2;
    for(; (c = s[i]) != ','; i++)
    {
        if('0' <= c && c <= '9')
            num = num * 16 + c - '0';
        else
            num = num * 16 + c - 'a' + 10;
    }
    return num;
}


int detail = 0, s = 0, E = 0, b = 0;

int main(int argc, char* argv[])
{
    FILE* file;
    opterr = 0;
    int ch;
    while((ch = getopt(argc, argv, "vs:E:b:t:")) != -1)//cmd line
    {
        switch(ch)
        {
            case 'v':detail = 1;break;
            case 's':s = atoi(optarg);break;
            case 'E':E = atoi(optarg);break;
            case 'b':b = atoi(optarg);break;
            case 't':file = fopen(optarg,"r");break;
        }
    }

    int S = pow(2, s);//the number of line
    unsigned long smask = (S - 1) << b, tmask = 0xffffffff << (s + b); //mask off code
    int miss = 0, hit = 0, eviction = 0, count = 0;//count
    ci_t* CI = (ci_t*)malloc(sizeof(ci_t) * S * E);//cache index
    memset(CI, 0, S * E * sizeof(ci_t));
    char content[20];//content of file
    unsigned long address;//address in file
    int line = 0;

    while(fgets(content, 20, file))
    {
        int match = 0, op_count = 1;//if_natch and the different number of operation of 'L' 'S' 'M'
        count++;//total times
        address = xatol(content);//get address from file
        line = (address & smask) >> b;//line
        int start = line * E, end = (line + 1) * E;//start of line with E
        if(content[0] != ' ')//ignore 'I'
            continue;
        else if(content[1] == 'M')//M mean a data load followed by a data store, two steps
            op_count = 2;
        if(detail)//with -v argument
        {
            char* find = strchr(content, '\n');
            *find = '\0';
            printf("%s",content + 1);
        }
        for(int j = 0; j < op_count; j++)//step loop
        {
            for(int i = start; i < end; i++)//hit loop
            {
                if(CI[i].active)//if effective
                {
                    if((CI[i].addr & tmask) == (address & tmask))
                    {
                        hit++;
                        match = 1;
                        CI[i].hittime = count;
                        if(detail)
                            printf(" hit");
                    }
                }
            }
            if(!match)//miss
            {
                miss++;
                if(detail)
                    printf(" miss");
                int full = 1, i = start;
                for(; i < end; i++)
                {
                    if(!CI[i].active)
                    {
                        full = 0;
                        break;
                    }
                }
                if(!full)
                {
                    CI[i].active = 1;
                    CI[i].addr = address;
                    CI[i].hittime = count;
                }
                else//eviction
                {
                    eviction++;
                    if(detail)
                        printf(" eviction");
                    int min = CI[start].hittime, min_line = start;
                    for(i = start + 1; i < end; i++)
                    {
                        if(CI[i].hittime < min)
                        {
                            min = CI[i].hittime;
                            min_line = i;
                        }
                    }
                    CI[min_line].active = 1;
                    CI[min_line].addr = address;
                    CI[min_line].hittime = count;
                }
            }
        }
        if(detail)
            printf("\n");
    }

    free(CI);
    fclose(file);

    printSummary(hit, miss, eviction);
    return 0;
}
