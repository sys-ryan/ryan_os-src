[ORG 0x00]
[BITS 16]

SECTION .text

;code area
START:
  mov ax, 0x1000 ;entry point start addr
  mov ds, ax
  mov es, ax

  cli ;interrupt off
  lgdt [GDTR]

  ;enter PROTECTEDMODE
  mov eax, 0x4000003B
  mov cr0, eax ;cr0 <- flag

  jmp dword 0x08: (PROTECTEDMODE - $$ + 0x10000)


[BITS 32]
PROTECTEDMODE:
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  ;stack
  mov ss, ax
  mov esp, 0xFFFE
  mov ebp, 0xFFFE


  push (SWITCHSUCCESSMESSAGE - $$ + 0x10000)
  push 2
  push 0
  call PRINTMESSAGE
  add esp, 12

  jmp dword 0x08: 0x10200
  ;CS 세그먼트 셀렉터를 커널 코드 디스크립터(0x08)로 변경하면서
  ;0x10200 어드레스(C언어 커널이 있는 어드레스)로 이동


PRINTMESSAGE:
  push ebp
  mov ebp, esp
  push esi
  push edi
  push eax
  push ecx
  push edx

  mov eax, dword [ebp + 12]
  mov esi, 160
  mul esi
  mov edi, eax

  mov eax, dword [ebp + 8]
  mov esi, 2
  mul esi
  add edi, eax

  mov esi, dword [ebp + 16]

.MESSAGELOOP:
  mov cl, byte [esi]
  cmp cl, 0
  je .MESSAGEEND

  mov byte [edi + 0xB8000], cl  ;0이 아니라면 비디오 메모리 어드레스
  ;0xB8000 + EDI 에 문자 출력

  add esi, 1
  add edi, 2

  jmp .MESSAGELOOP

.MESSAGEEND:
  pop edx
  pop ecx
  pop eax
  pop edi
  pop esi
  pop ebp
  ret



;data area
align 8, db 0

dw 0x0000

GDTR:
  dw GDTEND - GDT - 1
  dd (GDT - $$ + 0x10000)

GDT:
  NULLDescriptor:
    dw 0x0000
    dw 0x0000
    db 0x00
    db 0x00
    db 0x00
    db 0x00

  ;code segment descriptor
  CODEDESCRIPTOR:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00

  ;data segment descriptor
  DATADESCRIPTOR:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00
GDTEND:

SWITCHSUCCESSMESSAGE: db 'Switch To Protected Mode Success!!', 0

times 512 - ($ - $$) db 0x00
