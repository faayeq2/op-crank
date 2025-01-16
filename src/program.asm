; Sample Assembly Program
START:
    MOV AX, BX     ; Move BX into AX
    ADD AX, 5      ; Add 5 to AX
    SUB BX, CX     ; Subtract CX from BX
    JMP LOOP       ; Jump to LOOP label

DATA_SECTION:
    DB 0x10        ; Define a byte with value 0x10
    DW 0x1234      ; Define a word with value 0x1234

LOOP:
    CMP AX, BX     ; Compare AX with BX
    JE END         ; Jump to END if equal
    INC AX         ; Increment AX
    JMP LOOP       ; Repeat loop

ARRAY_START:
    DB 0x01, 0x02, 0x03, 0x04, 0x05 ; Array of 5 bytes
    TIMES 3 DB 0x00                 ; Repeat 0x00 three times

END:
    HLT            ; Halt the program
