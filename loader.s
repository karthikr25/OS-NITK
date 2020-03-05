section .data
	KERNEL_STACK_SIZE equ 4096

    dd 0x1BADB002 	;Magic Number
    dd 0x0 			;Flags for GRUB Multiboot options
    dd -0x1BADB002 	;Checksum value = -(Magic Number + Flags)

section .bss
    kernel_stack resb KERNEL_STACK_SIZE

section .text:
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

.loop:
    jmp .loop