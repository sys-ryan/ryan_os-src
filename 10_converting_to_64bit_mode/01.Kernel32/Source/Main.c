#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString(int iX, int iY, const char* pcString);
BOOL kInitializeKernel64Area(void);
BOOL kIsMemoryEnough(void);

void Main(void){
  DWORD i;
	DWORD dwEAX, dwEBX, dwECX, dwEDX;
	char vcVendorString[13] = {0, };

  kPrintString(0, 3, "C Language Kernel Started.....................[PASS]");

  //Check if the minimum memory size is met
  kPrintString(0, 4, "Minimum Memory Size Check...................[    ]");
  if(kIsMemoryEnough() == FALSE){
    kPrintString(45, 4, "Fail]");
    kPrintString(0, 5, "Not Enough Memory! RYAN64 OS Requires Over 64Mbyte Memory!");

    while(1);
  }else{
    kPrintString(45, 4, "Pass");
  }

  //IA-32e 모드의 커널 영역을 초기화
  kPrintString(0, 5, "IA-32e Kernel Area Initialize...............[    ]");
  if(kInitializeKernel64Area() == FALSE){
    kPrintString(45, 5, "Fail]");
    kPrintString(0, 6, "Kernel Area Initialization Fail!");
    while(1);
  }
  kPrintString(45, 5, "Pass");

  //create page table for IA-32e mode kernel
  kPrintString(0, 6, "IA-32e Page Tables Initialize...............[    ]");
  kInitializePageTables();
  kPrintString(45, 6, "Pass");

	//reading process vendor
	kReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
	*(DWORD*) vcVendorString = dwEBX;
	*((DWORD*) vcVendorString + 1) = dwEDX;
	*((DWORD*) vcVendorString + 2) = dwECX;
	kPrintString(0, 7, "Processor Vendor String.....................[            ]");
	kPrintString(45, 7, vcVendorString);

	//confirm 64bit support
	kReadCPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);
	kPrintString(0, 8, "64bit Mode Support Check....................[    ]");
	if(dwEDX & (1 << 29)){
		kPrintString(45, 8, "Pass");
	}else{
		kPrintString(45, 8, "Fail");
		kPrintString(0, 9, "This Processor does not support 64bit mode!!");

		while(1);
	}

	//Converto to IA-32e mode
	kPrintString(0, 9, "Switch To IA-32e Mode");
  //kSwitchAndExecute64bitKernel();

  while(1);
}

void kPrintString(int iX, int iY, const char* pcString){
  CHARACTER* pstScreen = (CHARACTER*) 0xB8000;
  int i;

  pstScreen += (iY * 80) + iX;
  for(i=0; pcString[i] != 0; i++){
    pstScreen[i].bCharactor = pcString[i];
  }
}


//IA-32e 모드 커널 영역을 0으로 초기화
BOOL kInitializeKernel64Area(void){
  DWORD* pdwCurrentAddress;

  //초기화를 시작할 어드레스인 0x100000(1MB)을 설정
  pdwCurrentAddress = (DWORD*) 0x100000;

  //마지막 어드레스인 0x600000(6MB)까지 루프를 돌면서 4바이트씩 0으로 채움
  while((DWORD) pdwCurrentAddress < 0x600000){
    *pdwCurrentAddress = 0x00;

    //0으로 저장한 후 다시 읽었을 때 0이 나오지 않으면 해당 어드레스를
    //사용하는데 문제가 생긴 것이므로 더이상 진행하지 않고 종료
    if(*pdwCurrentAddress != 0){
      return FALSE;
    }

    //다음 어드레스로 이동
    pdwCurrentAddress++;
  }

  return TRUE;
}


//Check if PC has enough memory for executing RYAN64 OS
BOOL kIsMemoryEnough(void){
  DWORD* pdwCurrentAddress;

  //start check from 0x100000(1MB)
  pdwCurrentAddress = (DWORD*) 0x100000;

  //check through loop until 0x4000000(64MB)
  while((DWORD) pdwCurrentAddress < 0x4000000){
    *pdwCurrentAddress = 0x12345678;

    //If the result of reading is not 0x12345678 after storing 0x12345678,
    //it means that address has a problem, which makes it stop proceeding.
    if(*pdwCurrentAddress != 0x12345678){
      return FALSE;
    }
    //Move 1MB at a time to check
    pdwCurrentAddress += (0x100000 / 4);
  }
  return TRUE;
}
