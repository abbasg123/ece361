#include "../lock.h"
#include <stdio.h>
#include <string.h>

static int passed = 0;
static int failed = 0;

/* Print one result and count failures so main can return a failing exit code. */
static void check(int condition, const char *description)
{
    if (condition)
    {
        printf("PASS: %s\n", description);
        passed++;
    }
    else
    {
        printf("FAIL: %s\n", description);
        failed++;
    }
}

/*
 * Compare every table cell and lock_step result with the assignment's rules.
 * Expected results are written here independently, not read from lock_table.
 */
static void test_transitions(void)
{
    const struct
    {
        state_t state;
        event_t event;
        state_t next;
        action_t action;
    } cases[] =
    {
        { LOCKED, E_RIGHT,   GOT1,   A_NONE },
        { LOCKED, E_WRONG,   LOCKED, A_BEEP },
        { LOCKED, E_CLOSE,   LOCKED, A_NONE },
        { LOCKED, E_TIMEOUT, LOCKED, A_NONE },
        { GOT1,   E_RIGHT,   GOT2,   A_NONE },
        { GOT1,   E_WRONG,   LOCKED, A_BEEP },
        { GOT1,   E_CLOSE,   GOT1,   A_NONE },
        { GOT1,   E_TIMEOUT, LOCKED, A_NONE },
        { GOT2,   E_RIGHT,   OPEN,   A_UNLOCK },
        { GOT2,   E_WRONG,   LOCKED, A_BEEP },
        { GOT2,   E_CLOSE,   GOT2,   A_NONE },
        { GOT2,   E_TIMEOUT, LOCKED, A_NONE },
        { OPEN,   E_RIGHT,   OPEN,   A_NONE },
        { OPEN,   E_WRONG,   OPEN,   A_NONE },
        { OPEN,   E_CLOSE,   LOCKED, A_LOCK },
        { OPEN,   E_TIMEOUT, LOCKED, A_LOCK }
    };

    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++)
    {
        state_t state = cases[i].state;
        event_t event = cases[i].event;
        action_t action = A_UNDEFINED;
        state_t next = lock_step(state, event, &action);
        char description[100];

        snprintf(description, sizeof description, "%s + %s -> %s / %s",
                 state_name(state), event_name(event),
                 state_name(cases[i].next), action_name(cases[i].action));

        check(lock_table[state][event].next == cases[i].next &&
              lock_table[state][event].action == cases[i].action &&
              next == cases[i].next && action == cases[i].action,
              description);
    }
}

/*
 * Test every digit in every state, plus closing, timeout, and ignored input.
 * OPEN has no expected digit: all digits are WRONG events, ignored by its row.
 */
static void test_events(void)
{
    const char expected_digit[N_STATES] = { '3', '1', '4', '\0' };
    const struct
    {
        char input;
        int event;
        const char *description;
    } other_inputs[] =
    {
        { 'c',  E_CLOSE,   "c gives CLOSE" },
        { 't',  E_TIMEOUT, "t gives TIMEOUT" },
        { 'x',  -1,        "other letter is ignored" },
        { ' ',  -1,        "space is ignored" },
        { '\n', -1,        "newline is ignored" },
        { '\0', -1,        "null character is ignored" },
        { '/',  -1,        "character below '0' is ignored" },
        { ':',  -1,        "character above '9' is ignored" }
    };

    for (int s = 0; s < N_STATES; s++)
    {
        state_t state = (state_t)s;
        char description[100];

        for (char digit = '0'; digit <= '9'; digit++)
        {
            int expected = E_WRONG;

            if (digit == expected_digit[s])
            {
                expected = E_RIGHT;
            }

            snprintf(description, sizeof description, "%s: digit %c gives %s",
                     state_name(state), digit, event_name((event_t)expected));
            check(lock_event(state, digit) == expected, description);
        }

        for (size_t i = 0; i < sizeof other_inputs / sizeof other_inputs[0]; i++)
        {
            snprintf(description, sizeof description, "%s: %s",
                     state_name(state), other_inputs[i].description);
            check(lock_event(state, other_inputs[i].input) == other_inputs[i].event,
                  description);
        }
    }
}

/*
 * Run each required sequence from LOCKED using both lock_event and lock_step.
 * Check every intermediate state and action so an incorrect early unlock
 * cannot be hidden by a later return to LOCKED. Spaces are ignored as in main.
 */
static void test_sequences(void)
{
    const struct
    {
        const char *input;
        size_t steps;
        state_t states[4];
        action_t actions[4];
    } cases[] =
    {
        { "3 1 4", 3,
          { GOT1, GOT2, OPEN },
          { A_NONE, A_NONE, A_UNLOCK } },
        { "3 3 1 4", 4,
          { GOT1, LOCKED, LOCKED, LOCKED },
          { A_NONE, A_BEEP, A_BEEP, A_BEEP } },
        { "3 1 t 4", 4,
          { GOT1, GOT2, LOCKED, LOCKED },
          { A_NONE, A_NONE, A_NONE, A_BEEP } },
        { "3 1 4 c", 4,
          { GOT1, GOT2, OPEN, LOCKED },
          { A_NONE, A_NONE, A_UNLOCK, A_LOCK } }
    };

    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++)
    {
        state_t state = LOCKED;
        size_t step = 0;
        int correct = 1;
        char description[100];

        for (size_t j = 0; cases[i].input[j] != '\0'; j++)
        {
            int event = lock_event(state, cases[i].input[j]);

            if (event == -1)
            {
                continue;
            }

            action_t action = A_UNDEFINED;
            state = lock_step(state, (event_t)event, &action);

            /* Check the count before indexing the expected results. */
            if (step >= cases[i].steps)
            {
                correct = 0;
                break;
            }

            if (state != cases[i].states[step] ||
                action != cases[i].actions[step])
            {
                correct = 0;
                break;
            }

            step++;
        }

        snprintf(description, sizeof description, "sequence %s", cases[i].input);
        check(correct && step == cases[i].steps, description);
    }
}

/* Verify the labels main prints, including the required invalid-value result. */
static void test_names(void)
{
    const char *const states[] = { "LOCKED", "GOT1", "GOT2", "OPEN" };
    const char *const events[] = { "RIGHT", "WRONG", "CLOSE", "TIMEOUT" };
    const char *const actions[] = { "UNDEFINED", "NONE", "UNLOCK", "LOCK", "BEEP" };
    char description[100];

    for (int i = 0; i < N_STATES; i++)
    {
        snprintf(description, sizeof description, "state_name(%d) is %s", i, states[i]);
        check(strcmp(state_name((state_t)i), states[i]) == 0, description);
    }

    for (int i = 0; i < N_EVENTS; i++)
    {
        snprintf(description, sizeof description, "event_name(%d) is %s", i, events[i]);
        check(strcmp(event_name((event_t)i), events[i]) == 0, description);
    }

    for (int i = 0; i < N_ACTIONS; i++)
    {
        snprintf(description, sizeof description, "action_name(%d) is %s", i, actions[i]);
        check(strcmp(action_name((action_t)i), actions[i]) == 0, description);
    }

    check(strcmp(state_name((state_t)-1), "?") == 0, "state_name(-1) is ?");
    check(strcmp(state_name(N_STATES), "?") == 0, "state_name(N_STATES) is ?");
    check(strcmp(event_name((event_t)-1), "?") == 0, "event_name(-1) is ?");
    check(strcmp(event_name(N_EVENTS), "?") == 0, "event_name(N_EVENTS) is ?");
    check(strcmp(action_name((action_t)-1), "?") == 0, "action_name(-1) is ?");
    check(strcmp(action_name(N_ACTIONS), "?") == 0, "action_name(N_ACTIONS) is ?");
}

/* Run all checks, print totals, and make any failure visible to make test. */
int main(void)
{
    test_transitions();
    test_events();
    test_sequences();
    test_names();

    printf("\nSummary: %d passed, %d failed, %d total\n",
           passed, failed, passed + failed);

    if (failed != 0)
    {
        return 1;
    }

    return 0;
}
