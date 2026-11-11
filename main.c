#include "headers.h"

int main(int argc, char *argv[])
{
    if (argc == 1 && strcasecmp(argv[0], "./a.out") == 0)
    {
        printf("\n===================================================================================\n");
        printf("ERROR : ./a.out : INVALID ARGUMENTS\n");
        printf("USAGE :\n");
        printf("For help please pass like : ./a.out -h\n");
        printf("To view please pass like : ./a.out -v <filename>\n");
        printf("To edit please pass like : ./a.out -e -t/-a/-A/-y/-m/-c <changing text> <filename>\n");
        printf("===================================================================================\n");
        return 0;
    }

    if (argc == 2 && strcasecmp(argv[1], "-h") == 0)
    {
        help();
        return 0;
    }

    if (argc == 2 && strcasecmp(argv[1], "-v") == 0)
    {
        printf("USAGE : ./a.out -v <filename>\n");
        return 0;
    }

    if (argc == 3 && strcasecmp(argv[1], "-v") == 0)
    {
        if ((strstr(argv[2], ".mp3")) == NULL)
        {
            printf("mp3 file only accpetable\n");
            return 0;
        }
        view(argv[2]);
        return 0;
    }

    if (argc >= 2 && argc < 5 && strcasecmp(argv[1], "-e") ==  0)
    {
        printf("For Edit :\n");
        printf("USAGE : ./a.out -e <edit option> <new value> <filename>\n");
        return 0;
    }

    if (argc == 5 && strcasecmp(argv[1], "-e") == 0)
    {
        if ((strstr(argv[4], ".mp3")) == NULL)
        {
            printf("MP3 file only accpetable\n");
            return 0;
        }
        edit(argv[2], argv[3], argv[4]);
        return 0;
    }

    return 0;
}