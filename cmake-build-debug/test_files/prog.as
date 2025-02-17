; file prog.as

.entry HELLO
.extern C
STR:        .string "wxyz"
HELLO:      add @r7 , A
            mcro m1
                inc @r2
                mov C,@r1
            endmcro
.data 7 , -57, +17 ,9
LINE:       inc @r2
            mcro m2
                add B, @r0
                bne END
            endmcro
            jmp LINE
            m2
            mov @r1,@r2
.extern A
STRING: .string "Hello, World!"
            m1
END:        stop
.extern B
.entry LINE