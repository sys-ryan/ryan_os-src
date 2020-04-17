### 8.1 IA-32e 모드 커널과 메모리 맵

IA-32e 모드 커널일 실행하기 위한 준비 작업  
  - PC에 설치된 메모리가 64MB 이상인지 검사   
  - IA-32e 모드 커널이 위치할 영역을 모드 0으로 초기화   
  - 부팅 과정을 완료하고 나서 1MB 이상의 메모리에 정상 접근 되는지 확인   

IA-32e 모드 커널이 위치할 영역을 0으로 초기화 하는 이유.  
  이미지를 옮길 영역을 미리 0으로 초기화하지 않는다면 어떤 임의의 값이  
  들어 있을 것임. 이러한 상태에서 IA-32e 모드 커널이 실행되면 0으로  
  참조되어야 할 변수들이 0이 아닌 값으로 설정되어, 루프를 빠져 나오지 못한다든지  
  잘못된 조건문이 실행된다든지 하는 문제가 발생할 수 있음.  
  이러한 사태를 미연에 방지하기 위해 먼저 0으로 초기화.  



### 8.2 IA-32e 모드 커널의 메모리 공간을 0으로 초기화
- 초기화 방법
- 초기화 코드를 추가하여 빌드한 뒤 OS 실행했을 때 발생하는 문제와 그 원인


### 8.2.1 메모리 초기화 기능 추가
1MB ~ 6MB 영역 중에서 1MB 영역은 IA-32e 모드 커널이 위치할 곳은 아니지만,  
IA-32e 모드 커널을 위한 자료구조가 위치할 영역이므로 같이 초기화.   

초기화를 수행하는 함수 `kInitializeKernel64Area()` 함수 추가.   

01.Kernel32/Source/Main.c
```
void kPrintString(int iX, int iY, const char* pcString);
BOOL kInitializeKernel64Area(void);

void Main(void){
  DWORD i;

  kPrintString(0, 3, "C Language Kernel Started!");

  //IA-32e 모드의 커널 영역을 초기화
  kInitializeKernel64Area();
  kPrintString(0, 4, "IA-32e Kernel Area Initialization Complete!");

  while(1);
}

... 생략 ...

//IA-32e 모드 커널 영역을 0으로 초기화
BOOL kInitializeKernel64Area(void){
  DWORD* ppdwCurrentAddress;

  //초기화를 시작할 어드레스인 0x100000(1MB)을 설정
  pdwCurrentAddress = (DWORD*) 0x100000;

  //마지막 어드레스인 0x600000(6MB)까지 루프를 돌면서 4바이트씩 0으로 채움
  while((DWORD) pdwCurrentAddress < 0x600000){
    *pdwCurrentAddress = 0x00;

    //0으로 저장한 후 다시 읽었을 때 0이 나오지 않으면 해당 어드레스를
    //사용하는데 문제가 생긴 것이므로 더이상 진행하지 않고 종료
    if(*pdwCurrentAddress != 0){
      return false;
    }

    //다음 어드레스로 이동
    pdwCurrentAddress++;
  }

  return TRUE;
}
```


### 빌드와 실행
정상적으로 초기화가 수행되었다는 'IA-32e Kernel Area Initialization Complete' 메시지 확인  

<img src="./img/.jpeg" title="IA-32e Kernel Area Initialization"></img><br/>
