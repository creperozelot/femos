; src/arch/x86/cpu/test_isr.asm
bits 32
global isr_test_stub

; Handler für int 0x30:
; schreibt ein 'I' an Position (0,1) und kehrt zurück

isr_test_stub:
    pusha

    mov dword [0xB8002], 0x07490049   ; 0x07 Attr, 0x49 = 'I'

    popa
    iretd
