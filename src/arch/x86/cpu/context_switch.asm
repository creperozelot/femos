bits 32

; void context_switch(uint32_t** old_sp, uint32_t* new_sp);

global context_switch

context_switch:
    ; Stack beim Eintritt:
    ; [esp]    return address
    ; [esp+4]  &old_sp
    ; [esp+8]  new_sp

    ; callee-saved Register sichern
    push ebp
    push ebx
    push esi
    push edi

    ; *old_sp = aktueller ESP
    mov eax, [esp + 20]   ; &old_sp (4 args weiter unten wg 4 pushes)
    mov [eax], esp

    ; ESP = new_sp
    mov eax, [esp + 24]   ; new_sp
    mov esp, eax

    ; Register für neuen Task wieder herstellen
    pop edi
    pop esi
    pop ebx
    pop ebp

    ret
