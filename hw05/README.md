`make`                    # Build
`make test`              # Run tests
`./lock < data/codes.txt`  # Run with input file
`make clean`       # Remove build files

State Duagram (Organized by Event/Action)
(LOCKED) -- RIGHT/NONE ----> (GOT1)
(GOT1)   -- RIGHT/NONE ----> (GOT2)
(GOT2)   -- RIGHT/UNLOCK --> (OPEN)

(LOCKED) -- WRONG/BEEP ----> (LOCKED)
(GOT1)   -- WRONG/BEEP ----> (LOCKED)
(GOT2)   -- WRONG/BEEP ----> (LOCKED)

(LOCKED) -- TIMEOUT/NONE --> (LOCKED)
(GOT1)   -- TIMEOUT/NONE --> (LOCKED)
(GOT2)   -- TIMEOUT/NONE --> (LOCKED)
(OPEN)   -- TIMEOUT/LOCK --> (LOCKED)

(LOCKED) -- CLOSE/NONE ----> (LOCKED)
(GOT1)   -- CLOSE/NONE ----> (GOT1)
(GOT2)   -- CLOSE/NONE ----> (GOT2)
(OPEN)   -- CLOSE/LOCK ----> (LOCKED)

(OPEN)   -- RIGHT/NONE ----> (OPEN)
(OPEN)   -- WRONG/NONE ----> (OPEN)

State Transitions Table

| Header 1 | Header 2 | Header 3 |
| -------- | -------- | -------- |
| Cell A1  | Cell B1  | Cell C1  |
| Cell A2  | Cell B2  | Cell C2  |

| Current state | RIGHT | WRONG | CLOSE | TIMEOUT |
|---------------|-------|-------|-------|---------|
| LOCKED | GOT1 / NONE | LOCKED / BEEP | LOCKED / NONE | LOCKED / NONE |
| GOT1 | GOT2 / NONE | LOCKED / BEEP | GOT1 / NONE | LOCKED / NONE |
| GOT2 | OPEN / UNLOCK | LOCKED / BEEP | GOT2 / NONE | LOCKED / NONE |
| OPEN | OPEN / NONE | OPEN / NONE | LOCKED / LOCK | LOCKED / LOCK |