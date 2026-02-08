#!/bin/bash

# Cores para o terminal (opcional, mas ajuda a ver erros)
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${GREEN}Iniciando Build do potatOS...${NC}"

# 1. Limpeza total
rm -rf *.o kernel.bin potatos.iso iso
mkdir -p iso/boot/grub

# 2. Compilar o Boot (Assembly)
echo "Compilando boot.asm..."
nasm -f elf32 kernel/boot.asm -o boot.o
if [ $? -ne 0 ]; then echo -e "${RED}Erro no Assembly!${NC}"; exit 1; fi

# 3. Compilar o Kernel (C)
echo "Compilando main.c..."
gcc -m32 -ffreestanding -c kernel/main.c -o kernel.o
if [ $? -ne 0 ]; then echo -e "${RED}Erro no GCC!${NC}"; exit 1; fi

# 4. Linkar (Unir os arquivos)
echo "Linkando kernel.bin..."
ld -m elf_i386 -T linker.ld -o kernel.bin boot.o kernel.o
if [ $? -ne 0 ]; then echo -e "${RED}Erro no Linker! Verifique o linker.ld${NC}"; exit 1; fi

# 5. Configurar ISO e GRUB
echo "Preparando imagem ISO..."
cp kernel.bin iso/boot/

# Criando o arquivo de configuração do GRUB (Multiboot 1)
cat << EOF > iso/boot/grub/grub.cfg
set timeout=0
set default=0

menuentry "potatOS" {
    multiboot /boot/kernel.bin
    boot
}
EOF

# 6. Gerar a ISO
grub-mkrescue -o potatos.iso iso
if [ $? -ne 0 ]; then echo -e "${RED}Erro ao criar a ISO!${NC}"; exit 1; fi

echo -e "${GREEN}Build concluído com sucesso!${NC}"
echo "Rode: qemu-system-i386 -cdrom potatos.iso"
