[BITS 64]

SECTION .text
extern Main


;;Code area
START:
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  ;create stack (1MB)
  mov ss, ax
  mov rsp, 0x6FFFF8
  mov rbp, 0x6FFFF8

  call Main

  jmp $
