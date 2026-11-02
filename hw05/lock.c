#include "lock.h"

const transition_t lock_table[N_STATES][N_EVENTS] =
{

    [LOCKED] =
    {
        [E_RIGHT]   = { GOT1,   A_NONE },
        [E_WRONG]   = { LOCKED, A_BEEP },
        [E_CLOSE]   = { LOCKED, A_NONE },
        [E_TIMEOUT] = { LOCKED, A_NONE }
    },
    [GOT1]=
    {
        [E_RIGHT]   = { GOT2,   A_NONE },
        [E_WRONG]   = { LOCKED, A_BEEP },
        [E_CLOSE]   = { GOT1,   A_NONE },
        [E_TIMEOUT] = { LOCKED, A_NONE }
    },
    [GOT2]=
    {
        [E_RIGHT]   = { OPEN, A_UNLOCK },
        [E_WRONG]   = { LOCKED, A_BEEP },
        [E_CLOSE]   = { GOT2,   A_NONE },
        [E_TIMEOUT] = { LOCKED, A_NONE }
    },
    [OPEN]=
    {
        [E_RIGHT]   = { OPEN,   A_NONE },
        [E_WRONG]   = { OPEN,   A_NONE },
        [E_CLOSE]   = { LOCKED, A_LOCK },
        [E_TIMEOUT] = { LOCKED, A_LOCK }
    }
};

state_t lock_step(state_t s, event_t e, action_t *action)
{
    transition_t r = lock_table[s][e];
    *action = r.action;

    return r.next;
}

int lock_event(state_t s, char c)
{
    static const char expectedDig[N_STATES]=
    {
        [LOCKED] =  '3',
        [GOT1]   =  '1',
        [GOT2]   =  '4',
        [OPEN]   =  '\0',
    };

    if(s < LOCKED || s >= N_STATES)
        return -1;
    if(c == 'c')
        return E_CLOSE;
    if(c == 't')
        return E_TIMEOUT;
    if(c >= '0' && c <= '9')
    {
        if(c == expectedDig[s])
            return E_RIGHT;
        return E_WRONG;
    }

    return -1;
}

//return labels 
const char *state_name(state_t s)
{
    static const char *const names[N_STATES] =
    {
        [LOCKED] = "LOCKED",
        [GOT1] = "GOT1",
        [GOT2] = "GOT2",
        [OPEN] = "OPEN"
    };

    if (s < LOCKED || s >= N_STATES)
    {
        return "?";
    }

    return names[s];
}

const char *event_name(event_t e)
{
    static const char *const names[N_EVENTS] =
    {
        [E_RIGHT] = "RIGHT",
        [E_WRONG] = "WRONG",
        [E_CLOSE] = "CLOSE",
        [E_TIMEOUT] = "TIMEOUT"
    };

    if (e < E_RIGHT || e >= N_EVENTS)
    {
        return "?";
    }

    return names[e];
}

const char *action_name(action_t a)
{
    static const char *const names[N_ACTIONS] =
    {
        [A_UNDEFINED] = "UNDEFINED",
        [A_NONE] = "NONE",
        [A_UNLOCK] = "UNLOCK",
        [A_LOCK] = "LOCK",
        [A_BEEP] = "BEEP"
    };

    if (a < A_UNDEFINED || a >= N_ACTIONS)
    {
        return "?";
    }

    return names[a];
}
