[BITS 32]
;export the name to make it possible for C to call it.
global kReadCPUID, kSwitchAndExecute64bitKernel

SECTION .text

;return CPUID
;PARAM: DWORD dwEAX, DWORD* pdwEAX, *PDWEBX, *pdwECX, *pdwEDX
kReadCPUID:
	push ebp
	mov ebp, esp
	push eax
	push ebx
	push ecx
	push edx
	push esi


	;execute CPUID via EAX
	mov eax, dword [ebp + 8] ;parameter 1
	cpuid

	;;;save return value in parameter
	;*pdwEAX
	mov esi, dword [ebp + 12]	;parameter 2
	mov dword [esi], eax

	;*pdwEBX
	mov esi, dword [ebp + 16]	;parameter 3
	mov dword [esi], ebx

	;*pdwECX
	mov esi, dword [ebp + 20]	;parameter 4
	mov dword [esi], ecx

	;*pdwEDX
	mov esi, dword [ebp + 24] ;parameter 5
	mov dword [esi], edx

	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
	pop ebp
	ret

;convert to IA-32e mode and run 64bit kernel
;PARAM : None
kSwitchAndExecute64bitKernel:
	;set PAE bit of CR4 control register to 1
	mov eax, cr4
	or eax, 0x20
	mov cr4, eax

	;activate PML4 table address and cache in CR3
	mov eax, 0x100000
	mov cr3, eax

	;set IA32_EFER.LME to 1 to activate IA-32e mode
	mov ecx, 0xC0000080
	rdmsr

	or eax, 0x0100
	wrmsr


	;NW(bit 29) = 0, CD(bit 30) = 0, PG(bit 31) = 1
	;-> activate paging and caching
	mov eax, cr0
	or eax, 0xE0000000
	xor eax, 0x60000000
	mov cr0, eax

	jmp 0x08:0x200000

	;not executed
	jmp $
