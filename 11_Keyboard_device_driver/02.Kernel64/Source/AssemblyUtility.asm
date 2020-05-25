[BITS 64]

SECTION .text

global kInPortByte, kOutPortByte

;read 1 byte from port
;PARAM : port num
kInPortByte:
  push rdx;

  mov rdx, rdi
  mov rax, 0
  in al, dx

  pop rdx
  ret


;write 1 byte to the port
; PARAM : port num, Data
kOutPortByte:
  push rdx
  push rax

  mov rdx, rdi
  mov rax, rsi
  out dx, al

  pop rax
  pop rdx
  ret

  
