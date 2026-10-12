# Project 16: Automatic door

## Homework 2 scope

The five modules are stubs. `main.c` initializes iom361 with 2 switches and
3 LEDs, prints `Automatic door`, and exits with 0 on success or 1 on
initialization failure. The executable is `door`.

This is the initial specification. The transition table and actions are
added in week 5; the remaining implementation follows the project schedule.

## Inputs and outputs

Both switches are levels, evaluated each tick rather than treated as presses.

| Register | Bit | Name | Meaning when on |
|---|---|---|---|
| Switches | 0 | `MOTION` | Someone approaches. |
| Switches | 1 | `SAFETY_BEAM` | Something occupies the doorway. |
| LEDs | 0 | `MOTOR_OPEN` | Motor drives the door open. |
| LEDs | 1 | `MOTOR_CLOSE` | Motor drives the door closed. |
| LEDs | 2 | `FIRE_MODE` | Fire mode is active. |

The RGB LED will identify the state with one distinct color per state.
Temperature and humidity are safety inputs, and every reading is recorded.

## States

| State | Meaning |
|---|---|
| `CLOSED` | Door shut; initial state. |
| `OPENING` | Motor driving open. |
| `HELD_OPEN` | Door fully open. |
| `CLOSING` | Motor driving closed. |
| `SAFETY_STOP` | Door motion stopped because the safety beam was interrupted. |

Fire mode is a separate flag, not a sixth state.

## Planned events

Event generation will live in `fsm.c` so that switch interpretation,
timer rules, and fire hysteresis can be tested together. `main.c` will
handle scenario replay and pass readings, switches, and time to the FSM.

| Event | Condition |
|---|---|
| `MOTION_PRESENT` | Switch bit 0 is 1. |
| `MOTION_ABSENT` | Switch bit 0 is 0; track uninterrupted absence. |
| `BEAM_BLOCKED` | Switch bit 1 is 1. |
| `BEAM_CLEAR` | Switch bit 1 is 0. |
| `OPENING_COMPLETE` | 3 ticks have elapsed since opening began. |
| `CLOSING_COMPLETE` | 3 ticks have elapsed since closing began. |
| `HOLD_EXPIRED` | In `HELD_OPEN`, motion has been absent for 5 ticks. |
| `SAFETY_WAIT_COMPLETE` | 2 ticks have elapsed since entering `SAFETY_STOP`. |
| `FIRE_ENTER` | Fire mode is off and temperature is at least 57.0 degrees C. |
| `FIRE_CLEAR` | Fire mode is on and temperature is below 45.0 degrees C. |
| `ENVIRONMENT_UNSAFE` | Temperature or humidity is outside the normal safe range. |
| `ENVIRONMENT_RECOVERED` | Both readings return to the normal safe range. |
| `NO_EVENT` | No applicable event requires a transition. |

## Required timing and behavior

- One tick is one second; readings use `time_t` timestamps: start time + tick.
- All displayed timestamps use UTC through `gmtime()` and `strftime()`.
- Opening and closing each take 3 ticks.
- The door remains `HELD_OPEN` until motion has been absent for 5 ticks.
- A blocked beam during `CLOSING` causes `SAFETY_STOP` for 2 ticks,
  followed by reopening.
- At 57.0 degrees C or higher, fire mode opens the door and keeps it open.
  Fire mode remains active at 45.0 degrees C and clears only below it.
- Sensor values are rounded to 0.1 degrees C and 0.1 %RH before comparisons.

## Decisions to complete with the transition table

- Define the normal safe temperature/humidity ranges and recovery behavior.
  The project sheet supplies fire thresholds, but no normal safe ranges.
- Define event priority when switches, timer deadlines, and safety inputs coincide.
- Decide how motion during closing, a blocked beam while closed or held open,
  and fire mode during a safety stop affect the door.
- Define when the five-tick motion timer begins and resets.

Later updates will specify duplicate storage, temperature quantization,
array insertion/search and percentile rules, the report format, status bit
positions, RGB colors, the state diagram, and the required design discussions.
