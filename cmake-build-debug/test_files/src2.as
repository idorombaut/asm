; file src2.as

.entry LENGTH
.extern W
MAIN:   mov @r1 , , LENGTH
LOOP:   jmp 8
        prn -5,999
        bne W $
        sub @r1, 4
        bne ,L3
L1      inc K
.entry LOOP
        add @r3
END:    stop
STR:    .string "abcdef
LENGTH: .data 6,-9i+2,15.1
K:      .str "ERROR"
.extern L3 BLABLA