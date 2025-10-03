     .ORIG x3000
     LEA R0, PT1
     LDW R0, R0, #0
     AND R1, R1, #0
     ADD R1, R1, #1
     STW R1, R0, #0

     LEA R0, PT2
     LDW R0, R0, #0
     AND R1, R1, #0
     AND R2, R2, #0
     ADD R2, R2, #15
     ADD R2, R2, #5

LOOP LDB R3, R0, #0
     ADD R1, R1, R3
     ADD R0, R0, #1
     ADD R2, R2, #-1
     BRp LOOP

     LEA R0, PT3
     LDW R0, R0, #0
     LDW R2, R0, #0
     STW R1, R0, #0
    TRAP x25

     PT1 .FILL x4000
     PT2 .FILL xC000
     PT3 .FILL xC014
     .END