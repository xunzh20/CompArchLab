.ORIG x3000

LEA R0, A ;load location of label
LDW R0, R0, #0 ; load the address first of integer
LDB R1, R0, #0 ; load the integer
LEA R0, B
LDW R0, R0, #0
LDB R2, R0, #0
AND R3, R1, R1
BRN FIRSTN
BRP FIRSTP
BR FINE

FIRSTN AND R3, R2, R2
BRN DOUBLEN
BR FINE

FIRSTP AND R3, R2, R2
BRP DOUBLEP
BR FINE

DOUBLEN ADD R3, R1, R2
        BRP OVERFLOW
        BR STORE
DOUBLEP ADD R3, R1, R2
        BRN OVERFLOW
        BR STORE

STORE   LEA R4, S
        LDW R4, R4, #0
        STB R3, R4, #0
        
        LEA R4, OVER
        LDW R4, R4, #0
        AND R5, R5, #0
        STB R5, R4, #0
        BR DONE        

FINE ADD R3, R2, R1
     BR STORE

OVERFLOW LEA R4, S
        LDW R4, R4, #0
        STB R3, R4, #0
        LEA R4, OVER
        LDW R4, R4, #0
        AND R5, R5, #0
        ADD R5, R5, #1
        STB R5, R4, #0

DONE TRAP x25
A .FILL x3100
B .FILL x3101
S .FILL x3102
OVER .FILL x3103
.END