#include "readings.h"

#define LINE_LEN 128

int reading_read(FILE *input, int ticks[], float temps[], float hums[],
                 int capacity, int *skipped)
{
    char line[LINE_LEN];
    int count = 0;
    *skipped = 0;

    while (fgets(line, sizeof line, input) != NULL)
    {
        int i = 0;

        while (line[i] == ' ' || line[i] == '\t')
        {
            i++;
        }

        if (line[i] == '\n' || line[i] == '\0' || line[i] == '#')
        {
            continue;
        }

        if (count == capacity)
        {
            fprintf(stderr,
                    "warning: more than %d readings, the rest are ignored\n",
                    capacity);
            break;
        }

        if (sscanf(line, "%d %f %f",
                   &ticks[count], &temps[count], &hums[count]) == 3)
        {
            count++;
        }
        else
        {
            (*skipped)++;
        }
    }

    return count;
}