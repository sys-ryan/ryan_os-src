#include "Page.h"

//create page table for IA-32e mode kernel
void kInitializePageTables(void){
  PML4TENTRY* pstPML4TEntry;
  PDPTENTRY* pstPDPTEntry;
  PDENTRY* pstPDEntry;
  DWORD dwMappingAddress;
  int i;

  //create PML4 Table
  //initialize all to 0 except for the first entry
  pstPML4TEntry = (PML4TENTRY*) 0x100000;
  kSetPageEntryData( &(pstPML4TEntry[0]), 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0);
  for(i=0; i<PAGE_MAXENTRYCOUNT; i++){
    kSetPageEntryData(&(pstPML4TEntry[i]), 0, 0, 0, 0);
  }

  //create page directory pointer table
  //one PDPT can map up to 512 GB.
  //make 64 entry to map 64GB
  pstPDPTEntry = (PDPTENTRY*) 0x101000;
  for(i=0; i<64; i++){
    kSetPageEntryData(&(pstPDPTEntry[i]), 0, 0x102000 + (i + PAGE_TABLESIZE),
      PAGE_FLAGS_DEFAULT, 0);
  }
  for(i=64; i<PAGE_MAXENTRYCOUNT; i++){
      kSetPageEntryData(&(pstPDPTEntry[i]), 0, 0, 0, 0);
  }

  //create page directory Table
  //one page directory can map 1GB
  //create 64 page to enoughly support 64 GB
  pstPDEntry = (PDENTRY*) 0x102000;
  dwMappingAddress = 0;
  for(i=0; i<PAGE_MAXENTRYCOUNT * 64; i++){
    //Since it is impossible to express upper address with 32bit,
    //calculate in MB and then devide it again with 4KB
    //to get the address over 32 32bit
    kSetPageEntryData(&(pstPDEntry[i]), (i * (PAGE_DEFAULTSIZE >> 20)) >> 12,
              dwMappingAddress, PAGE_DEFAULTSIZE | PAGE_FLAGS_PS, 0);
    dwMappingAddress += PAGE_DEFAULTSIZE;
  }
}

//set base address and attribute flags for the page entry.
void kSetPageEntryData(PTENTRY* pstEntry, DWORD dwUpperBaseAddress,
            DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags){
  pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
  pstEntry->dwUpperBaseAddressAndEXB = (dwUpperBaseAddress & 0xFF) | dwUpperFlags;
}
