### 리얼모드 -> 보호모드 전환  6단계

1. 세그먼트 디스크립터 생성
    보호용 코드와 데이터용 세그먼트 디스크립터 생성

2. GDT 정보 생성
    세그먼트 디스크립터의 시작 어드레스와 디스크립터의 전체 크기 저장

3. 프로세서에 GDT 정보 설정
    GDTR 레지스터에 GDT의 시작 어드레스와 크기 설정

4. CR0 컨트롤 레지스터 설정
    CT0 컨트롤 레지스터의 PE 비트 = 1, PG 비트 = 0

5. jmp 명령으로 CS 세그먼트 셀렉터 변경과 보호 모드로 전환
    jmp 0x08 : 보호 모드 커널의 시작 어드레스

                                           16 비트 리얼 모드
---------------------------------------------------------
                                           32 비트 보호 모드

6. 각종 세그먼트 셀렉터 및 스택 초기화
    DS, ES, FS, GS, SS 세그먼트 셀렉터와 ESP, EBP 레지스터 초기화

--> 보호 모드 커널 실행


### 6.1 세그먼트 디스크립터 생성
세그먼트 디스크립터는 크게 `코드 세그먼트 디스크립터`와 `데이터 세그먼트 디스크립터`로 나누어진다.
- 코드 세그먼트 디스크립터: 실행 가능한 코드가 포함된 세그먼트에 대한 정보를 나타냄.
                      CS 세그먼트 셀렉터에 사용

- 데이터 세그먼트 디스크립터: 데이터가 포함된 세그먼트에 대한 정보를 나타냄
                      CS 세그먼트 셀렉터를 제외한 나머지 셀렉터에 사용할 수 있음
                      스택 영역 또한 데이터를 읽고 쓰는 데이터 영역의 한 종류이므로 데이터 세그먼트 디스크립터 사용

커널 코드 세그먼트와 데이터 세그먼트 디스크립터 생성 코드

```
CODEDESCRIPTOR:
  dw 0xFFFF   ;Limit
  dw 0x0000   ;Base [15:0]
  db 0x00     ;Base [23:16]
  db 0x9A     ;P=1, DPL=0, Code Segment, Execute/Read
  db 0xCF     ;G=1, D=1, L=0, Limit[19:16]
  db 0x00     ;Base[31:24]

DATADESCRIPTOR:
  dw 0xFFFF
  dw 0x0000
  db 0x00
  db 0x92
  db 0xCF
  db 0x00
```


### 6.2 GDT 정보 생성  
GDT(Global Desciptor Table)   
 연속된 디스크립터의 집합  
 코드 세그먼트 디스크립터와 데이터 세그먼트 디스크립터를 연속된 어셈블리어 코드로 나타내면 그 전체 영역이 GDT가 됨  

 GDT는 디스크립터의 집합이므로 프로세서에 GDT의 `시작 어드레스`와 `크기 정보`를 로딩해야 함.  
 -> 이것을 저장하는 자료구조가 필요.

 GDT 선형 주소  
  현재 세그먼트의 시작을 기준으로 GDT의 오프셋을 구하고  
  세그먼트 기준 주소를 더해주면 구할 수 있음  

  현재 코드는 부트 로더에 의해 0x10000에 로딩되어 실행되고 있으므로,  
  자료구조 생성시 GDT 오프셋에 0x10000을 더해주면 선형 주소가 됨.  


GDTR 및 GDT 정보 생성 코드
```
;GDTR 자료구조 정의
GDTR:
  dw GDTEND - GDT - 1
  dd (GDT - $$ + 0x10000)

;GDT 테이블 정의
GDT:
  NULLDescriptor:
    dw 0x0000
    dw 0x0000
    db 0x00
    db 0x00
    db 0x00
    db 0x00

... 생략 ...

;보호 모드 커널용 데이터 세그먼트 디스크립터
DATADESCRIPTOR:
  dw 0xFFFF
  dw 0x0000
  db 0x00
  db 0x92
  db 0xCF
  db 0x00
GDTEND:
```


### 6.3 보호모드로 전환
- GDTR 레지스터 설정
- CR0 컨트롤 레지스터 설정
- jmp 명령 수행


프로세서에 GDT 정보 설정
```
lgdt [GDTR] ;GDTR 자료구조를 프로세서에 설정하여 GDT 테이블을 로드
```


CR0 컨트롤 레지스터 설정
```
mov eax, 0x4000003B ;PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
mov cr0, eax  ;CR0 컨트롤 레지스터에 플래그 설정하여 보호 모드로 전환
```


보호 모드로 전환과 세그먼트 셀렉터 초기화
```
;커널 코드 세그먼트를 0x00을 기준으로 하는 것으로 교체하고 EIP의 값을 0x00을 기준으로 재설정
;CS 세그먼트 셀렉터: EIP
jmp dword 0x08: (PROTECTEDMODE - $$ + 0x10000)
;커널 코드 세그먼트가 0x00을 기준으로 하는 반면, 실제 코드는 0x10000을 기준으로 실행되고 있으므로
;오프셋에 0x10000을 더해서 세그먼트 교체 후에도 같은 선형 주소를 가리키게 함

[BITS 32]
PROTECTEDMODE:
  mov ax 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  ;스택을 0x00000000 ~ 0x0000FFFF 영역에 64KB 크기로 생성
  mov ss, ax
  mov esp, 0xFFFE
  mov dbp, 0xFFFE
```


#### 보호 모드용 PRINTSTRING 함수
리얼 모드용 함수를 보호 모드로 변환
- 스택의 크기가 2바이트에서 4바이트로 증가
- 범용 레지스터의 크기가 32비트로 커짐


보호 모드용 PRINTSTRING 함수인 PRINTMESSAGE함수

```
PRINTMESSAGE:
  push ebp
  mov ebp, esp
  push esi
  push edi
  push eax
  push ecx
  push edx

  ;X, Y 좌표로 비디오 메모리 어드레스 계산
  mov eax, dword [ebp + 12]
  mov esi, 160
  mov esi
  mov edi, eax

  mov eax, dword [ebp + 8]
  mov esi, 2
  mul esi
  add edi, eax

  ;출력할 문자열 어드레스
  mov esi, dword [ebp + 16]

.MESSAGELOOP:
  mov cl, byte [esi]
  cmp cl, 0
  je .MESSAGEEND

  mov byte [edi + 0xB8000], cl
  ;보호 모드에서는 32비트 오프셋을 상요할 수 있으므로, 리얼 모드처럼
  ;별도의 세그먼트 셀렉터를 사용하지 않고 바로 접근 가능

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
```



### 6.4 보호 모드용 커널 이미지 빌드와 가상 OS 이미지 교체

### 커널 엔트리 포인트 파일 생성
01.Kernel32/Source 에 EntryPoint.s 파일 추가.
보호 모드 커널의 가장 앞부분에 위치하는 코드로,  
보호 모드 전환과 초기화를 수행하여 이후에 위치하는 코드를 위한 환경 제공


### makefile 수정과 가상 OS 파일 교체
`01.Kernel32/makefile`  
커널 이미지 생성을 위해 엔트리 포인트 파일 빌드  

01.Kernel32/makefile
```
all: Kernel32.bin

Kernel32.bin: Source/EntryPoint.s
  nasm -o Kernel32.bin $<

clean:
  rm -f Kernel32.bin
```


`$<` : Dependency(:의 오른쪽)의 첫 번째 파일
따라서, 'Source/EntryPoint.s'로 치환되고, 이 엔트리 파일은 빌드되어 Kernel32.bin 파일로 생성됨


최상위 디렉터리의 makefile
```
... 이상 동일 ...

Disk.img: 00.BootLoader/BootLoader.bin 01.Kernel32/Kernel32.bin
  @echo
  @echo === Disk Image Build Start ===
  @echo

  cat $^ > Disk.img

  @echo
  @echo === All Build Complete ===
  @echo

... 이하 동일 ...
```

`$^`: Dependency(:의 오른쪽)에 나열된 전체 파일을 의미.  
두 가지 파일을 합쳐서 디스크 이미지를 생성.
