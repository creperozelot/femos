#!/bin/bash
set -e

# Basis-Pfade
SRC_DIR=src
BUILD_DIR=build
ISO_DIR=iso
LINKER_DIR=linker

KERNEL_ELF="$BUILD_DIR/kernel.elf"
ISO_FILE=femos.iso

CFLAGS="-m32 -ffreestanding -fno-stack-protector -nostdlib -Wall -Wextra -I$SRC_DIR"
ASFLAGS="-f elf32"

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
mkdir -p "$ISO_DIR/boot/grub"


#############################################
# DEPENDENCY CHECK / AUTO-INSTALLER
#############################################

REQUIRED_PACKAGES=(
    "gcc"
    "ld"
    "nasm"
    "grub-mkrescue"
    "grub-file"
    "xorriso"
    "qemu-system-i386"
)

echo "[*] Checking dependencies..."

check_bin() {
    command -v "$1" >/dev/null 2>&1
}

missing=()

for prog in "${REQUIRED_PACKAGES[@]}"; do
    if ! check_bin "$prog"; then
        missing+=("$prog")
    fi
done

if [ ${#missing[@]} -eq 0 ]; then
    echo "[*] All dependencies installed."
else
    echo "[!] Missing packages: ${missing[*]}"
    echo "[*] Attempting automatic installation..."

    # Detect package manager
    if   command -v apt >/dev/null 2>&1; then
        echo "[*] Using apt (Debian/Ubuntu)"
        sudo apt update
        sudo apt install -y build-essential nasm grub-pc-bin xorriso qemu-system-x86

    elif command -v pacman >/dev/null 2>&1; then
        echo "[*] Using pacman (Arch)"
        sudo pacman -S --needed --noconfirm base-devel nasm grub xorriso qemu-full

    elif command -v dnf >/dev/null 2>&1; then
        echo "[*] Using dnf (Fedora)"
        sudo dnf install -y gcc gcc-c++ make nasm grub2-tools xorriso qemu-system-x86

    elif command -v brew >/dev/null 2>&1; then
        echo "[*] Using brew (macOS)"
        brew install nasm xorriso qemu
        echo "[!] macOS braucht zusätzlich GRUB aus dem Linuxbrew-Tap."
        echo "[!] GRUB unter macOS ist tricky. Empfehlung: Linux VM."

    else
        echo "[ERROR] No supported package manager found!"
        echo "Install dependencies manually:"
        echo "gcc nasm xorriso grub qemu"
        exit 1
    fi

    echo "[*] Re-checking installed tools..."

    for prog in "${missing[@]}"; do
        if ! check_bin "$prog"; then
            echo "[ERROR] Still missing: $prog"
            exit 1
        fi
    done

    echo "[*] All missing tools successfully installed."
fi


objects=()

echo "[1] Sammle und assembliere ASM-Dateien"
# Alle .asm unter src/ finden
while IFS= read -r asm_src; do
    # relativer Pfad ohne src/
    rel=${asm_src#"$SRC_DIR"/}
    # / durch _ ersetzen und .asm -> .o
    obj="$BUILD_DIR/${rel//\//_}"
    obj="${obj%.asm}.o"

    echo "  NASM  $asm_src -> $obj"
    nasm $ASFLAGS "$asm_src" -o "$obj"

    objects+=("$obj")
done < <(find "$SRC_DIR" -name '*.asm')

echo "[2] Sammle und kompiliere C-Dateien"
# Alle .c unter src/ finden
while IFS= read -r c_src; do
    rel=${c_src#"$SRC_DIR"/}
    obj="$BUILD_DIR/${rel//\//_}"
    obj="${obj%.c}.o"

    echo "  GCC   $c_src -> $obj"
    gcc $CFLAGS -c "$c_src" -o "$obj"

    objects+=("$obj")
done < <(find "$SRC_DIR" -name '*.c')

echo "[3] Linke Kernel -> $KERNEL_ELF"
ld -m elf_i386 -T "$LINKER_DIR/link.ld" -o "$KERNEL_ELF" \
    "${objects[@]}"

echo "[4] Kopiere Kernel ins ISO-Verzeichnis"
cp "$KERNEL_ELF" "$ISO_DIR/boot/kernel.elf"

echo "[5] Schreibe grub.cfg"
cat > "$ISO_DIR/boot/grub/grub.cfg" <<EOF
set gfxmode=800x600
set gfxpayload=keep
set timeout=3
set default=0

insmod gfxterm
insmod png
insmod gfxmenu

terminal_output gfxterm

background_image /boot/grub/background.png

menuentry "FEMOS (auto-build)" {
    multiboot /boot/kernel.elf
    boot
}
EOF

echo "[6] Erzeuge ISO -> $ISO_FILE"
grub-mkrescue -o "$ISO_FILE" "$ISO_DIR"

echo "[7] Prüfe Multiboot-Header"
if grub-file --is-x86-multiboot "$KERNEL_ELF"; then
    echo "OK: $KERNEL_ELF ist ein gültiger Multiboot-Kernel."
else
    echo "ERROR: $KERNEL_ELF ist KEIN gültiger Multiboot-Kernel!"
    exit 1
fi

echo "[8] Starte QEMU"
qemu-system-i386 -cdrom "$ISO_FILE"
