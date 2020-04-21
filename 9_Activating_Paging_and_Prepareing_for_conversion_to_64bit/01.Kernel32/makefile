#set build environment and rules
NASM32 = nasm
GCC32 = gcc -c -m32 -ffreestanding
LD32 = ld -melf_i386 -T ../elf_i386.x -nostdlib -e Main -Ttext 0x10200

#dfine OBJETCOPY Option for generating binary file image`
OBJCOPY32 = objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

#Define directory
OBJECTDIRECTORY = Temp
SOURCEDIRECTORY = Source



##Set build items and methods
all: prepare Kernel32.bin

#create directory for object files
prepare:
	mkdir -p $(OBJECTDIRECTORY)

#Build kernel Entry point
$(OBJECTDIRECTORY)/EntryPoint.bin: $(SOURCEDIRECTORY)/EntryPoint.s
	$(NASM32) -o $@ $<

#create information on dependency for C source files of Kernel.
dep:
	@echo === Make Dependency File ===
	make -C $(OBJECTDIRECTORY) -f ../makefile InternalDependency
	@echo === Dependency Search Complete ===

#move to object file directory and create a dependency file and executable file
ExecuteInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../makefile Kernel32.elf

#Covert Kernel image to Binary file
$(OBJECTDIRECTORY)/Kernel32.elf.bin: ExecuteInternalBuild
	$(OBJCOPY32) $(OBJECTDIRECTORY)/Kernel32.elf $@


#Create protected mode binary by joining C entry point and Kernel
Kernel32.bin: $(OBJECTDIRECTORY)/EntryPoint.bin $(OBJECTDIRECTORY)/Kernel32.elf.bin
	cat $^ > $@

#소스 파일을 체외한 나머지 파일 정리
#clear the remaining files except for source files.
clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*




## The section called again by 'make'. Modifeid based on Temp directory.

#define C source file to build, set Temp directory as base point.
CENTRYPOINTOBJECTFILE = Main.o
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
COBJECTFIELS = $(subst Main.o, , $(notdir $(patsubst %.c, %.o, $(CSOURCEFILES))))
ASSEMBLYOBEJCTFIELS = $(notdir $(patsubst %.asm, %.o, $(ASSEMBLYSOURCEFILES)))


#define a rule for converting .c file to .o
%.o: ../$(SOURCEDIRECTORY)/%.c
	$(GCC32) -c $<

#define a rules for converting .asm file to .o
%.o: ../SOURCEDIRECTORY/%.asm
	$(NASM32) -f elf32 -o $@ $<

#Create a file regarding the dependency.
InternalDependency:
	$(GCC32) -MM $(CSOURCEFILES) > Dependency.dep

#Build Kernel image
Kernel32.elf: $(CENTRYPOINTOBJECTFILE) $(COBJECTFIELS) $(ASSEMBLYOBEJCTFIELS)
	$(LD32) -o $@ $^

#If there is a dependency file among the files in the current directory, include it in 'make'
ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif
