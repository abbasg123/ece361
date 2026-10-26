#include "tally.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    tally_t *t = tally_create();

    if (t == NULL)
    { //tallyCreate returns null 
        fprintf(stderr, "Could not create a tally.\n");
        return 1;
    }

    //use 15 for \n or for tallyAdd to reject
    char line[TALLY_LEN + 2];
    int skipped = 0;

    while (fgets(line, sizeof(line), stdin) != NULL)
    {
        size_t length = strlen(line);

        if (length > 0 && line[length - 1] == '\n')
        {
            //add null term char to end of cstrin
            line[length - 1] = '\0';
        }
        else
        {
            //not using rest of line
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {}
        }

        //blank & # used as comments are ignored but not counted */
        if (line[0] == '\0' || line[0] == '#')
        {
            continue;
        }

//one line per tally
        if (tally_add(t, line) == -1)
        {
            skipped++;
        }
    }

//write error as opposed to no output
    if (ferror(stdin))
    {
        fprintf(stderr, "Could not read standard input.\n");
        tally_destroy(t);
        return 1;
    }

//sorted
    tally_print(t, stdout);
    printf("names: %d, lines skipped: %d\n", tally_size(t), skipped);

    tally_destroy(t);
    return 0;
}
