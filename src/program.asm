; Sample Assembly Program
START:
    MOV AX, BX     ; Move BX into AX
    ADD AX, 5      ; Add 5 to AX
    SUB BX, CX     ; Subtract CX from BX
    JMP LOOP       ; Jump to LOOP label
LOOP:
    CMP AX, BX     ; Compare AX with BX
    JE END         ; Jump to END if equal
    INC AX         ; Increment AX
    JMP LOOP       ; Repeat loop
LOOP:
MOV AX, 3