#!/bin/bash
set -e

SRC_DIR=src
LINKER_DIR=linker
ISO_DIR=iso
BUILD_DIR=build

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
mkdir -p "$ISO_DIR/boot/grub"

echo "[1] Assemble kernel_entry.asm"
nasm -f elf32 "$SRC_DIR/arch/x86/boot/kernel_entry.asm" -o "$BUILD_DIR/kernel_entry.o"

echo "[2a] Assemble gdt_flush.asm"
nasm -f elf32 "$SRC_DIR/arch/x86/cpu/gdt_flush.asm" -o "$BUILD_DIR/gdt_flush.o"

echo "[2b] Assemble idt_flush.asm"
nasm -f elf32 "$SRC_DIR/arch/x86/cpu/idt_flush.asm" -o "$BUILD_DIR/idt_flush.o"


echo "[3] Compile C sources"
gcc -m32 -ffreestanding -fno-stack-protector -nostdlib -Wall -Wextra -I"$SRC_DIR" \
    -c "$SRC_DIR/kernel/kmain.c"      -o "$BUILD_DIR/kmain.o"

gcc -m32 -ffreestanding -fno-stack-protector -nostdlib -Wall -Wextra -I"$SRC_DIR" \
    -c "$SRC_DIR/arch/x86/cpu/gdt.c"  -o "$BUILD_DIR/gdt.o"

gcc -m32 -ffreestanding -fno-stack-protector -nostdlib -Wall -Wextra -I"$SRC_DIR" \
    -c "$SRC_DIR/arch/x86/cpu/idt.c"  -o "$BUILD_DIR/idt.o"

gcc -m32 -ffreestanding -fno-stack-protector -nostdlib -Wall -Wextra -I"$SRC_DIR" \
    -c "$SRC_DIR/drivers/vga.c"       -o "$BUILD_DIR/vga.o"

echo "[4] Link kernel.elf"
ld -m elf_i386 -T "$LINKER_DIR/link.ld" -o "$BUILD_DIR/kernel.elf" \
    "$BUILD_DIR/kernel_entry.o" \
    "$BUILD_DIR/gdt_flush.o" \
    "$BUILD_DIR/kmain.o" \
    "$BUILD_DIR/gdt.o" \
    "$BUILD_DIR/vga.o" \
    "$BUILD_DIR/idt_flush.o" \
    "$BUILD_DIR/idt.o"

echo "[5] Copy kernel.elf to ISO"
cp "$BUILD_DIR/kernel.elf" "$ISO_DIR/boot/kernel.elf"

echo "[6] Write grub.cfg"
cat > "$ISO_DIR/boot/grub/grub.cfg" <<EOF
set timeout=5
set default=0

menuentry "Flames Enterprise Management Operating Systems" {
    multiboot /boot/kernel.elf
    boot
}
EOF

echo "[7] Create ISO"
grub-mkrescue -o femos.iso "$ISO_DIR"

echo "[8] Check multiboot header"
if grub-file --is-x86-multiboot "$BUILD_DIR/kernel.elf"; then
    echo "kernel.elf is a valid Multiboot kernel."
else
    echo "ERROR: kernel.elf is NOT Multiboot!"
    exit 1
fi

echo "[9] Run QEMU"
qemu-system-i386 -cdrom femos.iso
