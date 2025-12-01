/* linker/link.ld */
OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

SECTIONS
{
    . = 1M;

    .text : ALIGN(4) {
        *(.multiboot)
        *(.text*)
    }

    .rodata : ALIGN(4) {
        *(.rodata*)
    }

    .data : ALIGN(4) {
        *(.data*)
    }

    .bss : ALIGN(4) {
        *(.bss*)
        *(COMMON)
    }
}
