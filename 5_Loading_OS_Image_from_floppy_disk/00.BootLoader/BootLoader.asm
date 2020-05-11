[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07C0:START

TOTALSECTORCOUNT: dw 1024

;CODE AREA
START:
  mov ax, 0x07C0
  mov ds, ax
  mov ax, 0xB800
  mov es, ax

  ;STACK INIT
  mov ax, 0x0000
  mov ss, ax
  mov sp, 0xFFFE
  mov bp, 0xFFFE

  mov si, 0

.SCREENCLEARLOOP:
  mov byte [es: si], 0
  mov byte [es: si+1], 0x0A

  add si, 2
  cmp si, 80*25*2
  jl .SCREENCLEARLOOP


  ;PRINT Start Message
  push MESSAGE1
  push 0              ;Y
  push 0              ;X
  call PRINTMESSAGE
  add sp, 6

  ;PRINT OS Loading Message
  push IMAGELOADINGMESSAGE
  push 1
  push 0
  call PRINTMESSAGE
  add sp, 6


  ;DISK RESET
RESETDISK:
  ;call BIOS Reset Function
  mov ax, 0
  mov dl, 0
  int 0x13

  jc HANDLEDISKERROR


  ;디스크 내용을 메모리로 복사할 어드레스(ES:BX)를 0x10000으로 설정
  mov si, 0x1000
  mov es, si
  mov bx, 0x0000

  mov di, word [TOTALSECTORCOUNT]

READDATA:
  ;모든 섹터를 다 읽었는지 확인
  cmp di, 0
  je READEND
  sub di, 0x1

  ;Call BIOS Read Function
  mov ah, 0x02                ;BIOS 서비스 번호 2(Read Sector)
  mov al, 0x1
  mov ch, byte [TRACKNUMBER]
  mov cl, byte [SECTORNUMBER]
  mov dh, byte [HEADNUMBER]
  mov dl, 0x00
  int 0x13
  jc HANDLEDISKERROR

  ;복사할 어드레스, 트랙, 헤드, 섹터 어드레스 계산
  add si, 0x0020  ;512(0x2000) 바이트 만큼 읽었으므로, 이를 세그먼트 레지스터 값으로 반환
  mov es, si

  ;한 섹터 읽었으므로 섹터 번호 증가, 마지막 섹터(18)까지 읽었는지 판단.
  mov al, byte [SECTORNUMBER]
  add al, 0x01
  mov byte [SECTORNUMBER], al
  cmp al, 19
  jl READDATA                 ;19 미만이면 READDATA로 이동

  xor byte [HEADNUMBER], 0x01       ;마지막 섹터까지 읽었으면 헤드 토글(0->1, 1->0)
  mov byte [SECTORNUMBER], 0x01     ; 섹터 번호 다시 1로 설정

  ;만약 헤드가 1->0 으로 바뀌었으면 양쪽 헤드 모두 읽은것 -> 트랙 번호 1 증가
  cmp byte [HEADNUMBER], 0x00
  jne READDATA                      ;헤드 번호 1 아니면 READDATA로 이동

  ;트랙 1 증가시킨 후 다시 섹터 읽기로 이동
  add byte [TRACKNUMBER], 0x01
  jmp READDATA
READEND:
  ;Print OS IMAGE COMPLETE
  push LOADINGCOMPLETEMESSAGE
  push 1
  push 20
  call PRINTMESSAGE
  add sp, 6

  ;Execute Loaded Virtual OS IMAGE
  jmp 0x1000:0x0000


;Function Code AREA
HANDLEDISKERROR:
  push DISKERRORMESSAGE
  push 1
  push 20
  call PRINTMESSAGE

  jmp $

;Measage Print Function
PRINTMESSAGE:
  push bp
  mov bp, sp

  push es
  push si
  push di
  push ax
  push cx
  push dx

  ;ES 세그먼트 레지스터에 비디오 모드 어드레스 설정
  mov ax, 0xB800
  mov es, ax


  ;X, Y 좌표로 비도오 메모리의 어드레스 계산
  mov ax, word [bp+6]
  mov si, 160
  mul si
  mov di, ax

  mov ax, word [bp+4]
  mov si, 2
  mul si
  add di, ax

  mov si, word [bp+8]

.MESSAGELOOP:
  mov cl, byte [si]
  cmp cl, 0
  je .MESSAGEEND

  mov byte [es: di], cl

  add si, 1
  add di, 2

  jmp .MESSAGELOOP

.MESSAGEEND:
  pop dx
  pop cx
  pop ax
  pop di
  pop si
  pop es
  pop bp
  ret

;Data AREA
MESSAGE1: db 'RYAN64 OS Boot Loader Start!!', 0
DISKERRORMESSAGE: db 'OS Image Loading...', 0
IMAGELOADINGMESSAGE: db 'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db 'Complete!!!', 0

;디스크 읽기에 관련된 변수들
SECTORNUMBER: db 0x02
HEADNUMBER: db 0x00
TRACKNUMBER: db 0x00

times 510 - ($ - $$) db 0x00

db 0x55
db 0xAA
