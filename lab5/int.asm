    .ORIG x1200
     ;PUSH R0        ; Save the registers used.
     ADD R6, R6, #-2
     STW R0, R6, #0
     ;PUSH R1
     ADD R6, R6, #-2
     STW R1, R6, #0
     ; PUSH R2
     ADD R6, R6, #-2
     STW R2, R6, #0
     ;PUSH R3
     ADD R6, R6, #-2
     STW R3, R6, #0

     LEA R0, PTBR
     LDW R0, R0, #0 ; R0 is base pointer to val that needs to have ref cleared. ptbr
     LEA R1, CNT
     LDW R1, R1, #0
     LEA R2, MUX
     LDW R2, R2, #0

LOOP LDW R3, R0, #0;
     AND R3, R3, R2;
     STW R3, R0, #0;
     ADD R0, R0, #2
     ADD R1, R1, #-1
     BRp LOOP 
     
     LDW R3, R6, #0
     ADD R6, R6, #2
     ;POP R2
     LDW R2, R6, #0
     ADD R6, R6, #2
     ;POP R1        ; Restore the registers used. 
     LDW R1, R6, #0
     ADD R6, R6, #2
     ;POP R0
     LDW R0, R6, #0
     ADD R6, R6, #2
     RTI            ; Return from interrupt. 

PTBR .FILL x1000 ; PTBR
CNT  .FILL x0080 ; 128 counter
MUX  .FILL xFFFE ; 

     .END