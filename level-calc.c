#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>

#define COLOR_LIME "\e[92m"
#define COLOR_RESET "\e[0m"

typedef struct argInfo
{
    bool levels2xp;
    bool xp2levels;
    int levels;
    int xp;
    char * errorString;

}   *argInfo_s;

int vasprintf(char ** str, const char * fmt, va_list args);
int asprintf(char ** str, const char * fmt, ...);

void printUsage();
argInfo_s getArguments(int argc, char * argv[]);

int calculateXP(int levels);
float calculateLevels(int xp);

int main(int argc, char * argv[])
{
    argInfo_s info = getArguments(argc, argv);
    if(!info)
    {
        fprintf(stderr,"Memory error!\n");
        return EXIT_FAILURE;
    }
    else if(info->errorString)
    {
        fprintf(stderr,"Error getting arguments: %s\n",info->errorString);
        return EXIT_FAILURE;
    }

    if(info->levels2xp)
    {
        if(info->levels > 0)
        {
            printf("%s%d levels == %d experience%s\n",COLOR_LIME, info->levels, calculateXP(info->levels), COLOR_RESET);
        }
        else
        {
            fprintf(stderr,"Expected a positive integer as a level amount.");
            return EXIT_FAILURE;
        }
    }

    if(info->xp2levels)
    {
        if(info->xp > 0)
        {
            printf("%s%d experience == %f levels%s\n",COLOR_LIME, info->xp, calculateLevels(info->xp), COLOR_RESET);
        }
        else
        {
            fprintf(stderr,"Expected a positive integer as an experience amount.");
            return EXIT_FAILURE;
        }
    }
}

int calculateXP(int levels)
{
    double xp = 0;
    if(levels > 31)
    {
        xp = (4.5 * pow(levels,2)) - (162.5 * levels) + 2220;
    }
    else if(levels > 16)
    {
        xp = (2.5 * pow(levels,2)) - (40.5 * levels) + 360;
    }
    else if(levels > 0)
    {
        xp = pow(levels,2) + (6 * levels);
    }
    return (int)xp;
}

float calculateLevels(int xp)
{
    double levels = 0;
    if(xp > 1507)
    {
        levels  = (325.0 / 18.0) + sqrt((2.0/9.0) * (xp - (54215.0/72.0)));
    }
    else if(xp > 352)
    {
        levels  = (8.1) + sqrt((0.4) * (xp - (195.975)));
    }
    else if(xp > 0)
    {
        levels = sqrt(xp + 9) - 3;
    }
    return (float)levels;
}

argInfo_s getArguments(int argc, char * argv[])
{
    argInfo_s info = calloc(1,sizeof(struct argInfo));
    if(!info) return NULL;
    char * programName = argv[0];

    if(argc <= 1)
    {
        printUsage(programName);
        exit(0);
    }

    char c = '\0';
    while( (c = getopt(argc, argv, "hl:x:") ) != -1)
    {
        switch(c)
        {
            case 'h':
                printUsage(programName);
                exit(0);
            case 'l':
                info->levels2xp = true;
                info->levels = atoi(optarg);
                break;
            case 'x':
                info->xp2levels = true;
                info->xp = atoi(optarg);
                break;
            case '?':
                if(optopt == 'l' || optopt == 'x')
                {
                    asprintf(&(info->errorString),"Option '%c' expects an integer amount.\n",optopt);
                    return info;
                }
                else
                {
                    asprintf(&(info->errorString),"Unexpected option '%c'!\n",optopt);
                    return info;
                }
                break;
            default:
                fprintf(stderr,"Developer bug. Please report to maintainer with info: (%02x, %02x)\n",c,optopt);
                return NULL;
        }
    }

    return info;
}

void printUsage(char * programName)
{
    char usage[] =
    "\n%sMinecraft Levels and XP Calculator%s\n\n"
    "\tUsage: %s [options] <amount>\n\n"
    "Options:\n\n"
    "\t-h\t\tPrint help dialogue (what you see now) and exit.\n\n"
    "\t-l <levels>\tExchange levels into experience. Must provide integer level count.\n\n"
    "\t-x <experience>\tExchange experience into levels. Must provide integer experience amount.\n\n";

    printf(usage, COLOR_LIME, COLOR_RESET, programName);
}

int asprintf(char ** str, const char * fmt, ...)
{
        va_list args;
            va_start(args,fmt);
                int retval = vasprintf(str,fmt,args);
                    va_end(args);
                        return retval;
}

int vasprintf(char ** str, const char * fmt, va_list args)
{
        return __mingw_vasprintf(str,fmt,args);
}
