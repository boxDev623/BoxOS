ASM = nasm
CC = gcc
LD = ld

ASM_FLAGS = -felf32
C_FLAGS = -c -m32 -march=i686 -std=gnu99 -Wall -Wextra -fno-stack-protector
LD_FLAGS = -m elf_i386

SOURCE = source

ASM_OBJ = obj/asm
OBJ = obj

INCLUDE = include

BIN = bin

ASM_OBJECTS  =	$(ASM_OBJ)/entry.o\
				$(ASM_OBJ)/exception.o\
				$(ASM_OBJ)/irq.o\
				$(ASM_OBJ)/load_gdt.o\
				$(ASM_OBJ)/load_idt.o\
				$(ASM_OBJ)/bios32_call.o

OBJECTS  =	$(OBJ)/kernel.o\
			$(OBJ)/io_ports.o\
			$(OBJ)/string.o\
			$(OBJ)/vga.o\
			$(OBJ)/kcsl.o\
			$(OBJ)/gdt.o\
			$(OBJ)/8259_pic.o\
			$(OBJ)/idt.o\
			$(OBJ)/isr.o\
			$(OBJ)/dalloc.o\
			$(OBJ)/kheap.o\
			$(OBJ)/pit.o\
			$(OBJ)/ide.o\
			$(OBJ)/keyboard.o\
			$(OBJ)/mouse.o\
			$(OBJ)/kshell.o\
			$(OBJ)/bios32.o\
			$(OBJ)/vesa.o\
			$(OBJ)/rtc.o\
			$(OBJ)/window.o\
			$(OBJ)/button.o\
			$(OBJ)/gui.o

all: 	boot\
		kernel\
		interrupt\
		memory\
		drivers\
		gui\
		link

		grub-mkrescue -o boxos.iso bin

boot:
	$(ASM) $(ASM_FLAGS) $(SOURCE)/asm/entry.asm -o $(ASM_OBJ)/entry.o
	$(ASM) $(ASM_FLAGS) $(SOURCE)/asm/exception.asm -o $(ASM_OBJ)/exception.o
	$(ASM) $(ASM_FLAGS) $(SOURCE)/asm/irq.asm -o $(ASM_OBJ)/irq.o
	$(ASM) $(ASM_FLAGS) $(SOURCE)/asm/load_gdt.asm -o $(ASM_OBJ)/load_gdt.o
	$(ASM) $(ASM_FLAGS) $(SOURCE)/asm/load_idt.asm -o $(ASM_OBJ)/load_idt.o
	$(ASM) $(ASM_FLAGS) $(SOURCE)/asm/bios32_call.asm -o $(ASM_OBJ)/bios32_call.o

kernel:
	$(CC) -w $(C_FLAGS) $(SOURCE)/kernel.c -o $(OBJ)/kernel.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/io_ports.c -o $(OBJ)/io_ports.o -I$(INCLUDE)

	$(CC) -w $(C_FLAGS) $(SOURCE)/string.c -o $(OBJ)/string.o -I$(INCLUDE)

	$(CC) -w $(C_FLAGS) $(SOURCE)/kcsl.c -o $(OBJ)/kcsl.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/kshell.c -o $(OBJ)/kshell.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/bios32.c -o $(OBJ)/bios32.o -I$(INCLUDE)

interrupt:
	$(CC) -w $(C_FLAGS) $(SOURCE)/interrupt/8259_pic.c -o $(OBJ)/8259_pic.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/interrupt/gdt.c -o $(OBJ)/gdt.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/interrupt/idt.c -o $(OBJ)/idt.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/interrupt/isr.c -o $(OBJ)/isr.o -I$(INCLUDE)

memory:
	$(CC) -w $(C_FLAGS) $(SOURCE)/mm/dalloc.c -o $(OBJ)/dalloc.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/mm/kheap.c -o $(OBJ)/kheap.o -I$(INCLUDE)

drivers:
	$(CC) -w $(C_FLAGS) $(SOURCE)/drivers/keyboard.c -o $(OBJ)/keyboard.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/drivers/mouse.c -o $(OBJ)/mouse.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/drivers/pit.c -o $(OBJ)/pit.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/drivers/ide.c -o $(OBJ)/ide.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/drivers/vga.c -o $(OBJ)/vga.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/drivers/vesa.c -o $(OBJ)/vesa.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/drivers/rtc.c -o $(OBJ)/rtc.o -I$(INCLUDE)

gui:
	$(CC) -w $(C_FLAGS) $(SOURCE)/gui/window.c -o $(OBJ)/window.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/gui/button.c -o $(OBJ)/button.o -I$(INCLUDE)
	$(CC) -w $(C_FLAGS) $(SOURCE)/gui/gui.c -o $(OBJ)/gui.o -I$(INCLUDE)

link:
	$(LD) $(LD_FLAGS) -T linker.ld -o $(BIN)/boot/boxos.bin $(ASM_OBJECTS) $(OBJECTS)