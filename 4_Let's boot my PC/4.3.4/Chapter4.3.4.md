# 4.3.4 세그먼트 레지스터 초기화와 Hello, World!

### 세그먼트 레지스터 초기화
세그먼트 레지스터를 초기화하는 코드가 필요함.  
BIOS가 부트 로더를 실행했을 때 세그먼트 레지스터에는 BIOS가 사용하던 값이 들어 있음
세그먼트 레지스터를 초기화하지 않으면 엉뚱한 어드레스에 접근할 수 있으므로 미리 초기화 필요.

* CS 세그먼트 레지스터를 제외한 세그먼트 레지스터는 mov명령으로 처리 가능.
* CS 세그먼트는 jmp 명령과 세그먼트 레지스터 접두사 이용하여 수정

세그먼트 레지스터 초기화
```
SECTION .text

jmp 0x07C0:START  ;CS 세그먼트 레지스터에 0x07C0을 복사하면서 START 레이블로 이동
START:
  mov ax, 0x07C0
  mov ds, ax
  mov ax, 0xB800
  mov es, ax
```

세그먼트 레지스터 접두사를 사용해서 화면 맨 위에 M을 표시하는 소스 코드
```
mov byte [es: 0x00], 'M'
mov byte [ex: 0x01], 0x4A
```

### 화면 정리 및 부팅 메시지 출력
QEMU 실행시 BIOS가 출력한 메시지 지우기.
0xB8000 어드레스부터 80*25*2 바이트 중 문자 부분만 0으로 채우고  
속성값은 0이 아닌 다른 값으로 채움. (여기선 밝은 녹색 0x0A로 결정)


화면을 지우는 소스 코드(어셈블리어)
```
mov si, 0               ;SI 레지스터(문자열 원본 인덱스 레지스터) 초기화
.SCREENCLEARLOOP:
  mov byte[es: si], 0
  mov byte[es: si+1], 0x0A
  add si, 2
  cmp si, 80*25*2
  jl .SCREENCLEARLOOP
```

메시지를 출력하는 소스 코드(어셈블리어)
```
mov si, 0             ;SI 레지스터(문자열 원본 인덱스 레지스터) 초기화
mov di, 0             ;DI 레지스터(문자열 대상 인덱스 레지스터) 초기화

.MESSAGELOOP:
  mov cl, btye [si + MESSAGE1]
  cmp cl, 0
  je .MESSAGEEND

  mov byte [es: di], cl

  add si, 1
  add di, 2

  jmp .MESSAGELOOP
.MESSAGEEND:

MESSAGE1: db 'RYAN64 OS Boot Loader Start!', 0
;마지막은 0으로 설정하여, .MESSAGELOOP 에서 처리할 수 있게 함.

```  


### 실행
<img src="./img/4.3.4 terminal.png" title="4.3.4 terminal"></img><br/>
<img src="./img/4.3.4 qemu.png" title="4.3.4 qemu"></img><br/>
