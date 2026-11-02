#include "lock.h"
#include <stdio.h>

/* Read characters until EOF, report each lock event, and remember its result. */
int main(void)
{
    state_t state = LOCKED;
    int input;

    /* getchar returns int so EOF stays distinguishable from input characters. */
    while ((input = getchar()) != EOF)
    {
        int event = lock_event(state, (char)input);

        /* Spaces, newlines, and other unrecognized characters are not events. */
        if (event == -1)
        {
            continue;
        }

        action_t action;
        state_t next = lock_step(state, (event_t)event, &action);

        /* Print the old state before replacing it with the returned state. */
        printf("%s %c %s -> %s %s\n",
               state_name(state), input, event_name((event_t)event),
               state_name(next), action_name(action));

        state = next;
    }

    if (ferror(stdin))
    {
        fprintf(stderr, "Error reading standard input.\n");
        return 1;
    }

    printf("final: %s\n", state_name(state));
    return 0;
}