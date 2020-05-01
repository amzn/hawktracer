#include "internal/command_line_parser.h"
#include "internal/global_timeline.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef HT_ErrorCode(*HT_CommandLineArgumentParser)(int, char**, int);

typedef struct
{
    const char* argument;
    const char* help;
    HT_CommandLineArgumentParser parser;
    HT_Boolean is_flag;
} HT_CommandLineArgument;

static HT_ErrorCode print_help(int argc, char** argv, int pos);
static HT_ErrorCode set_global_timeline_buffer_size(int argc, char** argv, int pos);

HT_CommandLineArgument arguments[] = {
    {
        "--ht-global-timeline-buffer-size",
        "Set Global Timeline buffer size",
        set_global_timeline_buffer_size,
        HT_FALSE
    },
    {
        "--ht-help", "Print this help and exits the process",
        print_help,
        HT_TRUE
    }
};

static HT_ErrorCode
set_global_timeline_buffer_size(int argc, char** argv, int pos)
{
    if (pos + 1 >= argc)
    {
        return HT_ERR_MISSING_ARGUMENT;
    }

    const char* s = argv[pos + 1];
    char* end;
    errno = 0;
    unsigned long value = strtoul(s, &end, 10);

    if (errno == ERANGE ||
        (sizeof(size_t) < sizeof(unsigned long) && (unsigned long)HT_SIZE_MAX < value))
    {
        return HT_ERR_OUT_OF_RANGE;
    }

    if (end - s == 0)
    {
        return HT_ERR_INVALID_FORMAT;
    }

    ht_global_timeline_set_buffer_size((size_t) value);
    return HT_ERR_OK;
}

static HT_ErrorCode
print_help(int argc, char** argv, int pos)
{
    HT_UNUSED(argc);
    HT_UNUSED(argv);
    HT_UNUSED(pos);

    printf("HawkTracer options:\n");

    for (unsigned int x = 0; x < sizeof(arguments) / sizeof(arguments[0]); x++)
    {
        printf("  %s %s   %s\n",
            arguments[x].argument,
            arguments[x].is_flag ? "      " : "VALUE",
            arguments[x].help);
    }

    exit(0);

    return HT_ERR_OK;
}

void
ht_command_line_parse_args(int argc, char** argv)
{
    for (int i = 1; i < argc; i++)
    {
        for (unsigned int x = 0; x < sizeof(arguments) / sizeof(arguments[0]); x++)
        {
            const char* argname = arguments[x].argument;
            if (strcmp(argname, argv[i]) != 0)
            {
                continue;
            }

            HT_ErrorCode error = arguments[x].parser(argc, argv, i);

            if (error != HT_ERR_OK)
            {
                printf("Failed to process argument %s. Error code: %d\n",
                    argname, error);
            }

            if (arguments[x].is_flag == HT_FALSE)
            {
                i++;
            }
            break;
        }
    }
}
