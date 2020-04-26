#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define BYTESOFSECTOR 512

//define functions
int AdjustInSectorSize(int iFd, int iSourceSize);
void WriteKernelInformation(int iTargetFd, int iTotalKernelSectorCount,
          int iKernel32SectorCount);
int CopyFile(int iSourceFd, int iTargetFd);


//Main function
int main(int argc, char* argv[]){
  int iSourceFd;
  int iTargetFd;
  int iBootLoaderSize;
  int iKernel32SectorCount;
  int iKernel64SectorCount;
  int iSourceSize;

  //check command line option
  if(argc < 4){
    fprintf(stderr, "[ERROR] ImageMaker BootLoader.bin Kernel32.bin Kernel64.bin\n");
    exit(-1);
  }

  //Create Disk.img file
  if((iTargetFd = open("Disk.img", O_RDWR | O_CREAT| O_TRUNC, 0664)) == -1){
    fprintf(stderr, "[ERROR] Disk.img open fail.\n");
    exit(-1);
  }

  //open boot loader file and copy all of the contents to disk image file
  printf("[INFO] Copy boot loader to image file\n");
  if((iSourceFd = open(argv[1], O_RDONLY)) == -1){
    fprintf(stderr, "[ERROR] %s open fail\n", argv[1]);
    exit(-1);
  }

  iSourceSize = CopyFile(iSourceFd, iTargetFd);
  close(iSourceFd);

  //fill the remaining part with 0x00 to make the file size adjust to 512 byte which is the sector size.
  iBootLoaderSize = AdjustInSectorSize(iTargetFd, iSourceSize);
  printf("[INFO] %s size = [%d] and sector count = [%d]\n",
          argv[1], iSourceSize, iBootLoaderSize);

  //Open 32bit Kernel file and copy all of the contents to Disk image file
  printf("[INFO] Copy protected mode kernel to image file\n");
  if((iSourceFd = open(argv[2], O_RDONLY)) == -1){
    fprintf(stderr, "[ERROR] %s open fail\n", argv[2]);
    exit(-1);
  }

  iSourceSize = CopyFile(iSourceFd, iTargetFd);
  close(iSourceFd);



  //fill the remaining part with 0x00 to make the file size adjust to 512 byte which is the sector size.
  iKernel32SectorCount = AdjustInSectorSize(iTargetFd, iSourceSize);
  printf("[INFO] %s size = [%d] and sector count = [%d]\n",
          argv[2], iSourceSize, iKernel32SectorCount);

  //open 64bit kernel and copy all of the contents to the Disk image file.
  print("[INFO] Copy IA-32e mode kernel to image file\n");
  if((iSourceFd = open(argv[3], O_RDONLY)) == -1){
    fprintf(stderr, "[ERROR] %s open fail\n", argv[3]);
    exit(-1);
  }

  iSourceSize = CopyFile(iSourceFd, iTargetFd);
  close(iSourceFd);

  //fill the remaining parts with 0x00 to fit to 512 byte which is the size of the sector.
  iKernel64SectorCount = AdjustInSectorSize(iTargetFd, iSourceSize);
  printf("[INFO] %s size = [%d] and sector count = [%d]\n",
          argv[3], iSourceSize, iKernel64SectorCount);

  //Update Kernel information to disk image
  printf("[INFO] Start to write kernel information\n");

  //insert kernel information from 5th byte of the boot sector.
  WriteKernelInformation(iTargetFd, iKernel32SectorCount + iKernel64SectorCount,
                        iKernel32SectorCount);
  printf("[INFO] Image file create complete\n");

  close(iTargetFd);
  return 0;
}


//Fill with 0x00 from the current position to the location of the multiples of 512 bytes.
int AdjustInSectorSize(int iFd, int iSourceSize){
  int i;
  int iAdjustSizeToSector;
  char cCh;
  int iSectorCount;

  iAdjustSizeToSector = iSourceSize % BYTESOFSECTOR;
  cCh = 0x00;

  if(iAdjustSizeToSector != 0){
    iAdjustSizeToSector = 512 - iAdjustSizeToSector;
    printf("[INFO] File size [%lu] and fill [%u] byte\n",
            iSourceSize, iAdjustSizeToSector);
    for(i = 0; i < iAdjustSizeToSector; i++){
      write(iFd, &cCh, 1);
    }
  }else{
    printf("[INFO] Fiel size is aligned 512 byte\n");
  }

  iSectorCount = (iSourceSize + iAdjustSizeToSector) / BYTESOFSECTOR;
  return iSectorCount;
}

//insert kernel information into boot loader.
void WriteKernelInformation(int iTargetFd, int iTotalKernelSectorCount,
        int iKernel32SectorCount){
  unsigned short usData;
  long lPosition;

  //Location 5 bytes away from the beginning of the file represents the total sector count of the kernel.
  lPosition = lseek(iTargetFd, (off_t)5, SEEK_SET);
  if(lPosition == -1){
    fprintf(stderr, "lseek fail. Return value = %d, errno = %d, %d\n",
              lPosition, errno, SEEK_SET);
    exit(-1);
  }

  usData = (unsigned short) iTotalKernelSectorCount;
  write(iTargetFd, &usData, 2);
  usData = (unsigned short) iKernel32SectorCount;
  write(iTargetFd, &usData, 2);

  printf("[INFO] Total sector count except boot loader [%d]\n", iKernel32SectorCount);
  printf("[INFO] Total sector count of protected mode kernel [%d]\n",
          iKernel32SectorCount)
}

//copy the contents of Source file(Source Fd) to target file(Target FD) and return the size
int CopyFile(int iSourceFd, int iTargetFd){
  int iSourceFileSize;
  int iRead;
  int iWrite;
  char vcBuffer[BYTESOFSECTOR];

  iSourceFileSize = 0;
  while(1){
    iRead = read(iSourceFd, vcBuffer, sizeof(vcBuffer));
    iWrite = write(iTargetFd, vcBuffer, iRead);

    if(iRead != iWrite){
      fprintf(stderr, "[ERROR] iRead != iWrite.. \n");
      exit(-1);
    }
    iSourceFileSize += iRead;

    if(iRead != sizeof(vcBuffer)){
      break;
    }
  }

  return iSourceFileSize;
}
