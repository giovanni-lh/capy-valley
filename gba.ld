OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY {
    rom     (rx)    :   ORIGIN = 0x08000000, LENGTH = 32M
    ewram   (rw)    :   ORIGIN = 0x02000000, LENGTH = 256K
    iwram   (rw)    :   ORIGIN = 0x03000000, LENGTH = 32K
}

SECTIONS {
    .crt0   :   { *(.crt0) }                > rom
    .text   :   { *(.text*) *(.rodata*) }   > rom
    .crt0   :   { *(.data*) }               > iwram AT > rom
    .bss (NOLOAD)   :   {
        __bs_start__ = .;
        *(bss*) *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > iwram
}