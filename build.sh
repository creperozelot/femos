#!/bin/bash
set -e

SRC_DIR=src
LINKER_DIR=linker
ISO_DIR=iso
BUILD_DIR=build

rm -f kernel.elf
rm -rf "$ISO_DIR/boot"
rm -rf  "$BUILD_DIR"

mkdir -p "$ISO_DIR/boot/grub"
mkdir -p "./$BUILD_DIR"

echo "Assemble kernel entry..."
nasm -f elf32 "$SRC_DIR/kernel.asm" -o "$BUILD_DIR/kernel_entry.o"

echo "Compile C kernel..."
gcc -m32 -ffreestanding -c "$SRC_DIR/kernel.c" -o "$BUILD_DIR/kernel.o"

echo "Linking kernel..."
ld -m elf_i386 -T "$LINKER_DIR/link.ld" -o "$BUILD_DIR/kernel.elf" "$BUILD_DIR/kernel_entry.o" "$BUILD_DIR/kernel.o"

echo "Prepeair GRUB ISO structure..."
cp "$BUILD_DIR/kernel.elf" "$ISO_DIR/boot/kernel.elf"

cat > "$ISO_DIR/boot/grub/grub.cfg" <<EOF
set timeout=0
set default=0

menuentry "Start Flames Enterprise Management Operating System" {
    multiboot /boot/kernel.elf
    boot
}
EOF

echo "Creating ISO img..."
grub-mkrescue -o femos.iso "$ISO_DIR"

echo "Starting System..."
qemu-system-i386 -cdrom femos.iso