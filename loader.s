section .data
	KERNEL_STACK_SIZE equ 4096

    MAGIC_NUMBER equ 0x1BADB002
    FLAGS equ  0							;Flags for GRUB Multiboot options
    CHECKSUM equ -(MAGIC_NUMBER + FLAGS)

section .bss
    kernel_stack resb KERNEL_STACK_SIZE

section .text:
	
	align 4
        dd MAGIC_NUMBER
        dd FLAGS
        dd CHECKSUM

    mov esp, kernel_stack + KERNEL_STACK_SIZE

	global outb
	global inb

	extern mainFunc

outb:
	mov al, [esp + 8]
    mov dx, [esp + 4]
    out dx, al
    ret

inb:
	mov dx, [esp + 4]
    in  al, dx
    ret

loader:
    call mainFunc