# =============================================================================
# GBA Makefile — C only
# Compatible with GNU Make 3.81+
# =============================================================================

# ─────────────────────────────────────────────────────────────────────────────
# CONFIGURATION — edit only this section
# ─────────────────────────────────────────────────────────────────────────────
TARGET := $(notdir $(CURDIR))
BUILD  := build

SOURCE_ROOT  := source
INCLUDE_ROOT := include
GRAPHICS     := graphics
DATA         :=
MUSIC        :=

# ROM metadata for gbafix (leave empty for defaults)
ROM_TITLE    :=
ROM_GAMECODE :=
ROM_MAKER    :=

# ─────────────────────────────────────────────────────────────────────────────
# TOOLCHAIN
# ─────────────────────────────────────────────────────────────────────────────
ifeq ($(strip $(DEVKITARM)),)
  $(error "Set DEVKITARM in your environment: export DEVKITARM=<path>")
endif
ifeq ($(strip $(DEVKITPRO)),)
  $(error "Set DEVKITPRO in your environment: export DEVKITPRO=<path>")
endif

PREFIX  := $(DEVKITARM)/bin/arm-none-eabi-
CC      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy

BIN2S  := $(DEVKITPRO)/tools/bin/bin2s
GBAFIX := $(DEVKITPRO)/tools/bin/gbafix
MMUTIL := $(DEVKITPRO)/tools/bin/mmutil
GRIT   := $(DEVKITPRO)/tools/bin/grit

LIBGBA := $(DEVKITPRO)/libgba

# ─────────────────────────────────────────────────────────────────────────────
# DIRECTORY AUTO-DISCOVERY
#
# $(shell CMD) runs CMD and returns its output. find -type d lists every
# subdirectory recursively. 2>/dev/null silences the error if the folder
# does not exist yet.
# ─────────────────────────────────────────────────────────────────────────────
INCLUDEDIRS := $(shell find $(INCLUDE_ROOT) -type d 2>/dev/null)

# ─────────────────────────────────────────────────────────────────────────────
# FLAGS
#
# -mthumb:          16-bit Thumb code (the GBA default, to save ROM space)
# -mcpu=arm7tdmi:   the exact GBA processor
# -mabi=aapcs:      standard ARM ABI (replaces -mthumb-interwork)
# -mfloat-abi=soft: no hardware FPU, floating point done in software
# ─────────────────────────────────────────────────────────────────────────────
ARCH := -mthumb -mcpu=arm7tdmi -mabi=aapcs -mfloat-abi=soft

INCLUDE_FLAGS := $(INCLUDEDIRS:%=-iquote $(CURDIR)/%) \
                 -I$(LIBGBA)/include \
                 -I$(CURDIR)/$(BUILD)

CFLAGS  := -g -Wall -O2 $(ARCH) $(INCLUDE_FLAGS)
ASFLAGS := -g $(ARCH)

# -specs=gba.specs: selects devkitPro's crt0 (the _start startup code) and
#   linker script. WITHOUT it the linker cannot find _start and warns
#   "cannot find entry symbol _start": the game will not initialize properly.
#   gba.specs is the cartridge variant; use gba_mb.specs for multiboot builds.
# -Wl,OPT passes OPT straight to the linker. --gc-sections strips unused code.
# -Map produces the binary's memory map.
LDFLAGS := $(ARCH) \
            -specs=gba.specs \
            -Wl,--gc-sections \
            -Wl,-Map,$(BUILD)/$(TARGET).map \
            -L$(LIBGBA)/lib \
            -L$(CURDIR) \
            -lmm -lgba

# ─────────────────────────────────────────────────────────────────────────────
# SOURCE COLLECTION (recursive via find)
# ─────────────────────────────────────────────────────────────────────────────
CFILES   := $(shell find $(SOURCE_ROOT) -name '*.c'   2>/dev/null)
SFILES   := $(shell find $(SOURCE_ROOT) -name '*.s'   2>/dev/null)
PNGFILES := $(shell find $(GRAPHICS)    -name '*.png' 2>/dev/null)
BINFILES := $(foreach dir,$(DATA),$(wildcard $(dir)/*.*))

ifneq ($(strip $(MUSIC)),)
  AUDIOFILES := $(wildcard $(MUSIC)/*.*)
endif

# ─────────────────────────────────────────────────────────────────────────────
# GENERATED FILES
#
# External tools produce file pairs (.s + .h, or .S + .h). To stay compatible
# with Make 3.81 we do NOT use multi-target pattern rules: each tool is driven
# by a rule whose real target is the ASSEMBLY file, and the header is produced
# as a side effect (see the detailed explanation below, in the GRAPHICS section).
#
# The substitution foo/bar.png → $(BUILD)/foo/bar.s preserves the directory
# structure inside $(BUILD)/.
# ─────────────────────────────────────────────────────────────────────────────
GEN_PNG_S := $(PNGFILES:%.png=$(BUILD)/%.s)
GEN_PNG_H := $(GEN_PNG_S:%.s=%.h)

GEN_BIN_S := $(BINFILES:%=$(BUILD)/%.S)
GEN_BIN_H := $(GEN_BIN_S:%.S=%.h)

ifneq ($(strip $(MUSIC)),)
  GEN_SB_BIN := $(BUILD)/soundbank.bin
  GEN_SB_H   := $(BUILD)/soundbank.h
  GEN_SB_S   := $(BUILD)/soundbank.S
endif

# All generated headers: used as an order-only prerequisite of compilation,
# so they exist before the compiler looks for them.
GEN_HEADERS := $(GEN_PNG_H) $(GEN_BIN_H) $(if $(strip $(MUSIC)),$(GEN_SB_H),)

# ─────────────────────────────────────────────────────────────────────────────
# OBJECT FILES
# ─────────────────────────────────────────────────────────────────────────────
OFILES := $(CFILES:%.c=$(BUILD)/%.o)  \
          $(SFILES:%.s=$(BUILD)/%.o)  \
          $(GEN_PNG_S:%.s=%.o)        \
          $(GEN_BIN_S:%.S=%.o)        \
          $(if $(strip $(MUSIC)),$(GEN_SB_S:%.S=%.o),)

DEPENDS := $(OFILES:.o=.d)

# ─────────────────────────────────────────────────────────────────────────────
# OUTPUT
# ─────────────────────────────────────────────────────────────────────────────
ELF := $(BUILD)/$(TARGET).elf
GBA := $(TARGET).gba

# ─────────────────────────────────────────────────────────────────────────────
# GLOBAL DECLARATIONS
#
# .PHONY: targets that are not real files (always executed).
# .SUFFIXES: (empty) disables make's built-in implicit rules.
# .SECONDARY: stops make from deleting "intermediate" files (such as the .s
#   files generated by grit). Without this line, make 3.81 may remove them
#   after use and regenerate them on every build.
# ─────────────────────────────────────────────────────────────────────────────
.PHONY: all clean run
.SUFFIXES:
.SECONDARY:

all: $(GBA)

# ─────────────────────────────────────────────────────────────────────────────
# RULE: ELF → GBA
#
#   $@ = the target ($(GBA))      $< = first prerequisite ($(ELF))
#
# objcopy -O binary extracts the raw binary from the ELF (the ELF carries
# headers and metadata the GBA does not want). gbafix writes the Nintendo
# logo and checksum into the header: without it, the ROM will not boot on
# real hardware.
# ─────────────────────────────────────────────────────────────────────────────
$(GBA): $(ELF)
	@echo "[ROM]  $@"
	@$(OBJCOPY) -O binary $< $@
	@$(GBAFIX) $@ \
		$(if $(strip $(ROM_TITLE)),   '-t$(strip $(ROM_TITLE))',)    \
		$(if $(strip $(ROM_GAMECODE)),'-c$(strip $(ROM_GAMECODE))',) \
		$(if $(strip $(ROM_MAKER)),   '-m$(strip $(ROM_MAKER))',)    \
		>/dev/null

# ─────────────────────────────────────────────────────────────────────────────
# RULE: LINK → ELF
#
#   $^ = all prerequisites (every .o)
#   |  = separates normal prerequisites from order-only ones (see below)
# ─────────────────────────────────────────────────────────────────────────────
$(ELF): $(OFILES) | $(BUILD)
	@echo "[LD]   $@"
	@$(CC) -o $@ $^ $(LDFLAGS)

# ─────────────────────────────────────────────────────────────────────────────
# RULES: C and ASSEMBLY COMPILATION
#
# $(BUILD)/%.o: %.c is a pattern rule: % is the stem matched on both sides.
# For build/source/game/player.o, the stem is source/game/player and the
# prerequisite becomes source/game/player.c.
#
# | $(BUILD) $(GEN_HEADERS) are ORDER-ONLY prerequisites (after the |):
#   make builds them if missing, but does NOT recompile the .o if they change.
#   - $(BUILD): the directory must exist before writing the .o
#   - $(GEN_HEADERS): generated headers must exist before compilation
#
# mkdir -p $(dir $@) creates the subfolder under build/ (e.g. build/source/game/).
#
# -MMD generates the .d dependency file; -MP adds dummy rules for headers
# (avoids errors if a .h is deleted); -MF sets the .d file name.
# ─────────────────────────────────────────────────────────────────────────────
$(BUILD)/%.o: %.c | $(BUILD) $(GEN_HEADERS)
	@echo "[CC]   $<"
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -MMD -MP -MF $(@:.o=.d) -o $@ $<

$(BUILD)/%.o: %.s | $(BUILD) $(GEN_HEADERS)
	@echo "[AS]   $<"
	@mkdir -p $(dir $@)
	@$(CC) -c $(ASFLAGS) -o $@ $<

# Assembly already in build/ (generated by grit/bin2s): no mkdir needed.
# .s and .S are distinct extensions: .s is plain assembly, .S goes through the
# C preprocessor.
$(BUILD)/%.o: $(BUILD)/%.s
	@echo "[AS]   $<"
	@$(CC) -c $(ASFLAGS) -o $@ $<

$(BUILD)/%.o: $(BUILD)/%.S
	@echo "[AS]   $<"
	@$(CC) -c $(ASFLAGS) -o $@ $<

# ─────────────────────────────────────────────────────────────────────────────
# RULES: GRAPHICS — PNG → .s + .h via grit
#
# *** MAKE 3.81 COMPATIBILITY ***
# We avoid multi-target pattern rules (%.s %.h: %.png) because on make 3.81 they
# behave differently than on 4.3: make believes each file is produced by a
# SEPARATE invocation, and under parallel builds (-j) it may run grit twice on
# the same input.
#
# Portable solution: the REAL target is just the .s file. The rule generates it
# (and grit produces the .h as a side effect). We then declare that the .h
# "depends" on the .s WITH no recipe: this tells make the .h exists once the .s
# is built, without re-running grit.
#
# $* = the pattern stem. -o$(BUILD)/$* = output prefix for grit.
# $(<D) = the prerequisite's directory; $(notdir $(<D)) = just the folder name.
#
# Two rules for the .s: the first (dedicated .grit) takes priority. If a given
# image has no specific .grit, make discards the first rule (missing
# prerequisite) and uses the second one, with the folder's shared .grit.
# ─────────────────────────────────────────────────────────────────────────────
$(BUILD)/%.s: %.png %.grit | $(BUILD)
	@echo "[GRIT] $< (dedicated .grit)"
	@mkdir -p $(dir $@)
	@$(GRIT) $< -o$(BUILD)/$* -ff$(basename $<).grit

$(BUILD)/%.s: %.png | $(BUILD)
	@echo "[GRIT] $< (folder .grit)"
	@mkdir -p $(dir $@)
	@$(GRIT) $< -o$(BUILD)/$* -fts -ff$(<D)/$(notdir $(<D)).grit

# The .h is produced together with the .s: depend on the .s, no recipe of its own.
$(BUILD)/%.h: $(BUILD)/%.s ;

# ─────────────────────────────────────────────────────────────────────────────
# RULE: BINARIES — raw file → .S + .h via bin2s
#
# Same pattern as graphics: the real target is the .S, the .h depends on it.
# bin2s generates assembly with the data + a header with the symbols (pointer
# and size). -a 2 aligns to 2 bytes (the GBA bus is 16/32-bit). -H sets the
# header path.
# ─────────────────────────────────────────────────────────────────────────────
$(BUILD)/%.S: % | $(BUILD)
	@echo "[BIN2S] $<"
	@mkdir -p $(dir $@)
	@$(BIN2S) -a 2 -H $(BUILD)/$*.h $< > $@

# (the $(BUILD)/%.h: $(BUILD)/%.s rule above does not cover the binary's .h,
#  since that one derives from a .S; we add the matching dependency)
#
# MAKE 3.81 NOTE: a static pattern rule with an EMPTY target list is a fatal
# error ("No targets"). Guard it with ifneq, since DATA is empty by default.
ifneq ($(strip $(GEN_BIN_H)),)
$(GEN_BIN_H): $(BUILD)/%.h: $(BUILD)/%.S ;
endif

# ─────────────────────────────────────────────────────────────────────────────
# RULES: MUSIC — audio → soundbank (mmutil), then soundbank.bin → .S (bin2s)
#
# *** MAKE 3.81 COMPATIBILITY ***
# No grouped targets (&:) here either. The real target is soundbank.bin;
# soundbank.h depends on it with no recipe. mmutil converts the audio files
# into the MaxMod format, generating the .bin (data) and the .h (track/effect IDs).
# ─────────────────────────────────────────────────────────────────────────────
ifneq ($(strip $(MUSIC)),)
$(GEN_SB_BIN): $(AUDIOFILES) | $(BUILD)
	@echo "[MMUTIL] soundbank"
	@$(MMUTIL) $^ -o$(GEN_SB_BIN) -h$(GEN_SB_H)

$(GEN_SB_H): $(GEN_SB_BIN) ;

$(GEN_SB_S): $(GEN_SB_BIN)
	@echo "[BIN2S]  soundbank.bin"
	@$(BIN2S) -a 2 -H $(GEN_SB_H) $< > $@
endif

# ─────────────────────────────────────────────────────────────────────────────
# UTILITY
# ─────────────────────────────────────────────────────────────────────────────
$(BUILD):
	@mkdir -p $@

clean:
	@echo "[CLEAN]"
	@rm -rf $(BUILD) $(GBA)

run: $(ELF)
	@mgba-qt $<

# ─────────────────────────────────────────────────────────────────────────────
# AUTOMATIC DEPENDENCIES
#
# -include (with the leading dash) ignores missing .d files (first build /
# after clean) instead of erroring out. Each .d holds the header dependencies
# of its source, so editing a .h recompiles only the .c files that use it.
# ─────────────────────────────────────────────────────────────────────────────
-include $(DEPENDS)
