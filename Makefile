# --- Configuration du compilateur ---
CC = i686-elf-gcc
AS = nasm
LD = i686-elf-ld
CFLAGS = -std=c11 -ffreestanding -O2 -Wall -Wextra -nostdlib -fno-exceptions -nodefaultlibs
ASFLAGS = -f elf32
LDFLAGS = -n -T targets/i386/linker.ld

# --- Chemins Mandatory ---
SRC_DIR = src/impl/i386
BUILD_DIR = build/mandatory
DIST_DIR = dist/mandatory

# --- Chemins Bonus ---
BONUS_DIR = bonus
BUILD_BONUS_DIR = build/bonus
DIST_BONUS_DIR = dist/bonus

# --- Détection des sources ---
# Mandatory
ASM_SOURCES := $(shell find $(SRC_DIR) -name "*.asm")
C_SOURCES := $(shell find $(SRC_DIR) -name "*.c")
MANDATORY_OBJS := $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES)) \
                  $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

# Bonus
BONUS_ASM_SOURCES := $(shell find $(BONUS_DIR) -name "*.asm")
BONUS_C_SOURCES := $(shell find $(BONUS_DIR) -name "*.c")
BONUS_OBJS := $(patsubst $(BONUS_DIR)/%.asm, $(BUILD_BONUS_DIR)/%.o, $(BONUS_ASM_SOURCES)) \
              $(patsubst $(BONUS_DIR)/%.c, $(BUILD_BONUS_DIR)/%.o, $(BONUS_C_SOURCES))

# --- Règles de compilation ---

.PHONY: all clean build-mandatory build-bonus

all: build-mandatory build-bonus

# --- Build Mandatory ---
build-mandatory: $(MANDATORY_OBJS)
	@mkdir -p $(DIST_DIR)
	$(LD) $(LDFLAGS) -o $(DIST_DIR)/kernel.bin $(MANDATORY_OBJS)
	@mkdir -p targets/i386/iso/boot
	cp $(DIST_DIR)/kernel.bin targets/i386/iso/boot/kernel.bin
	grub-mkrescue /usr/lib/grub/i386-pc -o $(DIST_DIR)/kernel.iso targets/i386/iso

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

# --- Build Bonus ---
build-bonus: $(BONUS_OBJS)
	@mkdir -p $(DIST_BONUS_DIR)
	$(LD) $(LDFLAGS) -o $(DIST_BONUS_DIR)/kernel.bin $(BONUS_OBJS)
	@mkdir -p targets/i386/iso-bonus/boot/grub
	# On copie le grub.cfg existant vers le dossier bonus si nécessaire
	cp targets/i386/iso/boot/grub/grub.cfg targets/i386/iso-bonus/boot/grub/grub.cfg
	cp $(DIST_BONUS_DIR)/kernel.bin targets/i386/iso-bonus/boot/kernel.bin
	grub-mkrescue /usr/lib/grub/i386-pc -o $(DIST_BONUS_DIR)/kernel_bonus.iso targets/i386/iso-bonus

$(BUILD_BONUS_DIR)/%.o: $(BONUS_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_BONUS_DIR)/%.o: $(BONUS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf build dist