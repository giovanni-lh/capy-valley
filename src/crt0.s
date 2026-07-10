    .arm
    .section .crt0,"ax"
    .global _start
    .type _start, %function

_start:
    b       _gba_start          @ 4-byte branch past 188-byte ROM header

    .fill   156, 1, 0x00        @ Nintendo logo placeholder (gbafix writes real data)
    .ascii  "CAPYVALLEY\0\0"    @ game title (12 bytes)
    .ascii  "CVLY"              @ game code (4 bytes)
    .ascii  "00"                @ maker code (2 bytes)
    .byte   0x96                @ fixed value
    .byte   0x00, 0x00          @ unit code, device type
    .fill   7, 1, 0x00          @ reserved
    .byte   0x00, 0x00          @ version, checksum (gbafix writes checksum)
    .byte   0x00, 0x00          @ reserved

_gba_start:
    msr     cpsr_c, #0x1f       @ switch to system mode
    ldr     sp, =0x03008000     @ stack on top of iwram

    ldr     r0, =__bss_start__   @ zero bss
    ldr     r1, =__bss_end__
    mov     r2, #0
.Lbss:
    cmp     r0, r1
    strlt   r2, [r0], #4
    blt     .Lbss

    bl      main
.Lhang: b .Lhang

    .size _start, . - _start