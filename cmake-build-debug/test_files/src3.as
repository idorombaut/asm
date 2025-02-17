; file src3.as

.entry LENGTH
.extern W
MAIN:   mov @r3 ,LENGTH
LOOP:   jmp L1
        prn -5
        bne W
        sub @r1, @r4
        bne L3
L1:     inc K
.entry LOOP
.entry L3
        jmp W
END:    stop
STR:    .string "abcdef"
K:  .data 90
.extern L3