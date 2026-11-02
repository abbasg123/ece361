#ifndef LOCK_H
#define LOCK_H
/**
 *          eRight    eRight    eRight   (LOCKED)
 *  (LOCKED) -> (GOT1) -> (GOT2) -> (OPEN)_|eClose
 *         |__<___|eWrong__<_|eWrong 
 *                 eTime      eTime               
 * 
 * 
 */
typedef enum 
{ 
    LOCKED, 
    GOT1,
    GOT2, 
    OPEN, 
    N_STATES 
} state_t;

typedef enum 
{ 
    E_RIGHT, 
    E_WRONG, 
    E_CLOSE, 
    E_TIMEOUT, 
    N_EVENTS 
} event_t;

typedef enum 
{ 
    A_UNDEFINED, 
    A_NONE, 
    A_UNLOCK, 
    A_LOCK, 
    A_BEEP, 
    N_ACTIONS 
} action_t;

typedef struct 
{ 
    state_t next; 
    action_t action; 
} transition_t;

//row = curr state, col = events
extern const transition_t lock_table[N_STATES][N_EVENTS];

/*
s, e, must be valid , *action point to action_t
return next state and write action to *action, 
caller saves returned state & determines how to act
*/
state_t lock_step(state_t s, event_t e, action_t *action);

/*
Convert c to and event for state s, 
expected digit = eRight
!""            = eWrong
when (OPEN)    = any dig. eWrong
c = close, t = timeout
else return -1; no state is changed   
*/
int lock_event(state_t s, char c);

//return labels 
const char *state_name(state_t s); 
const char *event_name(event_t e);
const char *action_name(action_t a);

#endif