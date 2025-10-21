
BUILD_DIR=build
OUTPUT_OS=os.img

#Disk Configuration
#FAT
RESERVED_SECTOR=2048
BSS_COUNT=1M
SIZE_DISK=128
DEV_LOOP:=

# Compiler Configuration
LD_LINKER=ld
LD_TYPE:=elf_i386
C_COMPILER=gcc
C_FLAGS:=-m32 -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -Wall -Wextra
NASM_COMPILER=nasm
NASM_FLAGS:=-f elf32

KERNEL_ARCHITECTURE=i386
ifeq (${KERNEL_ARCHITECTURE}, x86_64)
	C_FLAGS:=-m64 -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -Wall -Wextra -DARCHX86_64=1
	NASM_FLAGS:=-f elf64 -DARCHX86_64=1
	LD_TYPE:=elf_x86_64
endif

# Boot File Configuration
BOOT_DIR=boot
BOOT_STAGE_DIR=$(BOOT_DIR)/boot_stage
BOOT_STAGE_C_SOURCES=$(wildcard $(BOOT_STAGE_DIR)/*.c \
								$(BOOT_STAGE_DIR)/src/*.c)
BOOT_STAGE_ASM_SOURCES=$(wildcard $(BOOT_STAGE_DIR)/*.asm \
									$(BOOT_STAGE_DIR)/src/*.asm)
BOOT_STAGE_C_OBJECTS=$(patsubst $(BOOT_STAGE_DIR)/%.c, $(BUILD_DIR)/boot.stage.dir/%.c.o, $(BOOT_STAGE_C_SOURCES))
BOOT_STAGE_ASM_OBJECTS=$(patsubst $(BOOT_STAGE_DIR)/%.asm, $(BUILD_DIR)/boot.stage.dir/%.asm.o, $(BOOT_STAGE_ASM_SOURCES))
# Kernel File Configuration
KERNEL_DIR=kernel
KERNEL_INCLUDEDIR=-I"kernel/"
KERNEL_C_SOURCES=$(wildcard ${KERNEL_DIR}/*.c \
							$(KERNEL_DIR)/driver/*.c \
							$(KERNEL_DIR)/cpu/*.c \
							$(KERNEL_DIR)/fs/*.c )
KERNEL_C_OBJECTS=$(patsubst $(KERNEL_DIR)/%.c, $(BUILD_DIR)/kernel.dir/%.c.o, $(KERNEL_C_SOURCES))

KERNEL_ASM_SOURCES=$(wildcard 	${KERNEL_DIR}/*.asm \
								$(KERNEL_DIR)/driver/*.asm \
								$(KERNEL_DIR)/cpu/*.asm \
								$(KERNEL_DIR)/fs/*.asm )
KERNEL_ASM_OBJECTS=$(patsubst $(KERNEL_DIR)/%.asm,${BUILD_DIR}/kernel.dir/%.asm.o,${KERNEL_ASM_SOURCES})

build: make_dir compile_kernel boot_builder boot_stage_build
	make -C ./tools/disk/
#	make mbr_build_disk
	make build_disk
	make run_test

build_os:
#	dd if=/dev/zero of=$(OUTPUT_OS) bs=$(BSS_COUNT) count=$(SIZE_DISK)
	grub2-mkrescue -o $(OUTPUT_OS) os

compile_kernel: $(KERNEL_ASM_OBJECTS) $(KERNEL_C_OBJECTS)
	$(LD_LINKER) -m $(LD_TYPE) -nostdlib -T kernel/linker.ld -o build/kernel.elf $(KERNEL_C_OBJECTS) $(KERNEL_ASM_OBJECTS)

$(BUILD_DIR)/kernel.dir/%.c.o: $(KERNEL_DIR)/%.c
	$(C_COMPILER) $(C_FLAGS) $(KERNEL_INCLUDEDIR) -o $@ -c $<
$(BUILD_DIR)/kernel.dir/%.asm.o: $(KERNEL_DIR)/%.asm
	$(NASM_COMPILER) $(NASM_FLAGS) -o $@ $<
	
make_dir: clean
	mkdir -p build
	mkdir -p build/kernel.dir
	mkdir -p build/boot.dir

	mkdir -p build/boot.stage.dir
	mkdir -p build/boot.stage.dir/src

	mkdir -p build/uefi.dir
	mkdir -p build/mnt/efi
	mkdir -p build/mnt/data
	make make_build_kernel_dir
make_build_kernel_dir:
	mkdir -p $(BUILD_DIR)/kernel.dir/driver
	mkdir -p $(BUILD_DIR)/kernel.dir/cpu
	mkdir -p $(BUILD_DIR)/kernel.dir/fs

clean:
	rm -rf build
	rm -rf data.img bootvol.img os.img
	make -C ./tools/disk clean

# BOOT Builder
boot_builder:
	nasm -f bin -o $(BUILD_DIR)/boot.dir/mbr.bin $(BOOT_DIR)/mbr.asm
	nasm -f bin -o $(BUILD_DIR)/boot.dir/boot.bin $(BOOT_DIR)/boot.asm
	nasm -f elf32 -o $(BUILD_DIR)/boot.dir/loadstage.asm.o $(BOOT_DIR)/load_stage.asm
	$(C_COMPILER) $(C_FLAGS) -m32 -o $(BUILD_DIR)/boot.dir/loadstage.c.o -c $(BOOT_DIR)/loadstage.c
	ld -nostdlib -m elf_i386 -T boot/loadstage.ld -o $(BUILD_DIR)/boot.dir/loadstage.bin $(BUILD_DIR)/boot.dir/loadstage.asm.o $(BUILD_DIR)/boot.dir/loadstage.c.o --oformat binary
# BOOT Stage
boot_stage_build: $(BOOT_STAGE_C_OBJECTS) $(BOOT_STAGE_ASM_OBJECTS)
	ld -m elf_i386 -T boot/bootx32.ld -o $(BUILD_DIR)/boot.stage.dir/bootstage.bin $(BOOT_STAGE_ASM_OBJECTS) $(BOOT_STAGE_C_OBJECTS) --oformat binary
$(BUILD_DIR)/boot.stage.dir/%.c.o: $(BOOT_STAGE_DIR)/%.c
	$(C_COMPILER) $(C_FLAGS) -o $@ -c $<
$(BUILD_DIR)/boot.stage.dir/%.asm.o: $(BOOT_STAGE_DIR)/%.asm
	$(NASM_COMPILER) $(NASM_FLAGS) -o $@ $<

build_disk:
	./tools/disk/disk --create --bs 1048576 --count 128 --output ./os.img
	dd if=/dev/zero of=./bootvol.img bs=512 count=40960
	dd if=/dev/zero of=./data.img bs=512 count=131720

	mkfs.fat -F 16 -R 5 -n "BootVol" ./bootvol.img
	mkfs.fat -F 32 -n "Data" ./data.img

	dd if=$(BUILD_DIR)/boot.dir/boot.bin of=./bootvol.img bs=1 seek=62 skip=62 count=448 conv=notrunc
	dd if=$(BUILD_DIR)/boot.dir/loadstage.bin of=./bootvol.img bs=512 seek=3 conv=notrunc
	mcopy -i ./bootvol.img $(BUILD_DIR)/boot.stage.dir/bootstage.bin ::BOOTX32.BIN
	mcopy -i ./bootvol.img build/kernel.elf ::KERNEL.ELF

	./tools/disk/disk --open ./os.img --part boot fat16 -bs 512 -st 1 -et 40960 --data ./bootvol.img
	./tools/disk/disk --open ./os.img --part fat32 -bs 512 -st 40961 -et 242144 --data ./data.img
	dd if=$(BUILD_DIR)/boot.dir/mbr.bin of=$(OUTPUT_OS) bs=1 count=440 conv=notrunc


mbr_build_disk:
	dd if=/dev/zero of=$(OUTPUT_OS) bs=$(BSS_COUNT) count=$(SIZE_DISK)
	parted $(OUTPUT_OS) --script -- mklabel msdos
	parted $(OUTPUT_OS) --script -- mkpart primary fat16 1MiB 20MiB
	parted $(OUTPUT_OS) --script -- set 1 boot on
	parted $(OUTPUT_OS) --script -- mkpart primary fat32 20MiB 100%
	dd if=$(BUILD_DIR)/boot.dir/mbr.bin of=$(OUTPUT_OS) bs=1 count=440 conv=notrunc

	$(eval DEV_LOOP:=$(shell sudo losetup -f --show -P $(OUTPUT_OS)))
	
	sudo mkfs.fat -F 16 -R 5 $(DEV_LOOP)p1
	sudo mkfs.fat -F 32 $(DEV_LOOP)p2

	sudo dd if=$(BUILD_DIR)/boot.dir/boot.bin of=$(DEV_LOOP)p1 bs=1 seek=62 skip=62 count=448 conv=notrunc
	sudo dd if=$(BUILD_DIR)/boot.dir/loadstage.bin of=$(DEV_LOOP)p1 bs=512 seek=3 conv=notrunc
	sudo mcopy -i $(DEV_LOOP)p1 $(BUILD_DIR)/boot.stage.dir/bootstage.bin ::BOOTX32.BIN
	sudo mcopy -i $(DEV_LOOP)p1 build/kernel.elf ::KERNEL.ELF
	
	sudo losetup -d $(DEV_LOOP)

grub_build_disk:
	dd if=/dev/zero of=$(OUTPUT_OS) bs=$(BSS_COUNT) count=$(SIZE_DISK)
	parted $(OUTPUT_OS) --script -- mklabel gpt
	parted $(OUTPUT_OS) --script -- mkpart EFI fat32 1MiB 64MiB
	parted $(OUTPUT_OS) --script -- set 1 boot on
	parted $(OUTPUT_OS) --script -- mkpart DATA fat32 65MiB 100%

	$(eval DEV_LOOP:=$(shell sudo losetup -f --show -P $(OUTPUT_OS)))
	
	sudo mkfs.fat -F 32 $(DEV_LOOP)p1
	sudo mkfs.fat -F 32 $(DEV_LOOP)p2

	sudo mount $(DEV_LOOP)p1 build/mnt/efi
	sudo mount $(DEV_LOOP)p2 build/mnt/data
	sudo mkdir -p build/mnt/efi/EFI/BOOT 
	sudo cp -rf os/boot build/mnt/efi/boot

	sudo grub2-mkstandalone \
		-O x86_64-efi \
		-o build/mnt/efi/EFI/BOOT/BOOTX64.EFI \
		--modules="part_gpt part_msdos all_video fat ext2 normal" \
		"boot/grub/grub.cfg=os/boot/grub/grub.cfg"
	
	sudo losetup -d $(DEV_LOOP)
	sudo umount build/mnt/efi
	sudo umount build/mnt/data
unmount_disk:
	sudo losetup -d $(shell cat build/dev.tmp)
	sudo umount build/mnt

# Custom UEFI
uefi_build:
	gcc boot/uefi/main.c \
			-c \
			-fno-stack-protector \
			-fpic \
			-fshort-wchar                      \
      		-mno-red-zone                      \
      		-I /usr/include/efi        \
      		-I /usr/include/efi/x86_64 \
      		-DEFI_FUNCTION_WRAPPER             \
      		-o build/uefi.dir/main.o
	ld build/uefi.dir/main.o \
			/usr/lib/crt0-efi-x86_64.o     \
     		-nostdlib                      \
     		-znocombreloc                  \
     		-T /usr/lib/elf_x86_64_efi.lds \
     		-shared                        \
     		-Bsymbolic                     \
     		-L /usr/lib               \
     		-l:libgnuefi.a                 \
     		-l:libefi.a                    \
     		-o build/uefi.dir/main.so
	objcopy -j .text                \
			-j .rodata \
          	-j .sdata               \
          	-j .data                \
          	-j .dynamic             \
          	-j .dynsym              \
          	-j .rel                 \
          	-j .rela                \
          	-j .reloc               \
          	--target=efi-app-x86_64 \
          	build/uefi.dir/main.so                 \
          	build/uefi.dir/main.efi
uefi_build_test: uefi_build
	dd if=/dev/zero of=$(OUTPUT_OS) bs=$(BSS_COUNT) count=$(SIZE_DISK)
	parted $(OUTPUT_OS) -s -a minimal mklabel gpt
	parted $(OUTPUT_OS) -s -a minimal mkpart EFI FAT32 2048s 65536s
	parted $(OUTPUT_OS) -s -a minimal set 1 esp on
	parted $(OUTPUT_OS) -s -a minimal mkpart DATA FAT32 65537s 100%

	$(eval DEV_LOOP:=$(shell sudo losetup -f --show -P $(OUTPUT_OS)))
	sudo mkfs.fat -F 32 $(DEV_LOOP)p1
	sudo mkfs.fat -F 32 $(DEV_LOOP)p2
	sudo mount $(DEV_LOOP)p1 build/mnt
	sudo mkdir -p build/mnt/efi/boot
	sudo cp -r build/uefi.dir/main.efi build/mnt/efi/boot/bootx64.efi
	sudo umount build/mnt
	sudo losetup -d $(DEV_LOOP)
#	dd if=/dev/zero of=$(BUILD_DIR)/efi.img bs=512 count=63489
#	mkfs.fat -F 32 $(BUILD_DIR)/efi.img
#	mcopy -i $(BUILD_DIR)/efi.img $(BUILD_DIR)/uefi.dir/main.efi ::
#	dd if=$(BUILD_DIR)/efi.img of=$(OUTPUT_OS) bs=512 seek=2048 count=63489 conv=notrunc

uefi_run_test:
	qemu-system-x86_64 -bios /usr/share/edk2/ovmf/OVMF_CODE.fd -hda $(OUTPUT_OS)
# Running Test using QEMU
run_test:
	qemu-system-x86_64 -hda $(OUTPUT_OS)
