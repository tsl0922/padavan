# === Metadata ===
IMAGE_AUTHOR    = TurBoTse
IMAGE_DATE      = $(shell TZ='Asia/Shanghai' date +%Y%m%d)
IMAGE_NAME      = $(IMAGE_AUTHOR)-$(CONFIG_FIRMWARE_PRODUCT_ID)-$(FIRMWARE_KERNEL_VER)-$(CONFIG_CROSS_COMPILE)-$(IMAGE_DATE).trx
IMAGE           = $(IMAGEDIR)/$(IMAGE_NAME)

# === Paths ===
KERNELZ         = $(IMAGEDIR)/zImage
RAMDISK         = $(IMAGEDIR)/ramdisk
BOARD_DIR       = $(ROOTDIR)/configs/boards/$(CONFIG_FIRMWARE_PRODUCT_ID)
SINGLESKU_DIR   = $(ROOTDIR)/vendors/Ralink

# === Compression/Alignment ===
COMP            = $(if $(CONFIG_KERNEL_NO_COMPRESS),none,lzma)
PART_ALIGN      = $(if $(filter y,$(CONFIG_MTD_NAND_MTK) $(CONFIG_MTD_NAND_RALINK)),131072,16)

# === Load Address ===
LOAD_ADDR       = $(if $(filter $(CONFIG_PRODUCT),MT7621),81001000,80000000)

# === Entry address command ===
ifeq ($(CONFIG_WITHOUT_KERNEL),y)
ENTRY_CMD       = $(MKIMAGE_TOOL) -l $(UBOOT_HEADER) 2>/dev/null | grep "Entry" | awk '{print $$3}'
else
ENTRY_CMD       = readelf -h $(ROOTDIR)/$(LINUXDIR)/vmlinux | awk '/Entry/{print $$4}'
endif

# === SquashFS Options (user overridable) ===
MKSQUASHFS_OPTS ?= -all-root -no-exports -noappend -nopad -noI -no-xattrs -b 1M

# === ROMFS directory structure ===
ROMFS_DIRS = \
	bin dev dev/pts home etc etc_ro etc_ro/ppp/peers etc_ro/ppp/plugins etc_ro/ppp/3g \
	etc_ro/Wireless lib lib/modules mnt media opt proc sbin sys \
	usr usr/bin usr/sbin usr/lib/pppd usr/codepages tmp var

# === Common Padding Macro ===
define pad_kernel
	@SIZE=`wc -c $(1) | awk '{ print $$1 }'` ; \
	SIZE=`expr $$SIZE + 64` ; \
	DIV=`expr $$SIZE % $(PART_ALIGN)` ; \
	if [ $$DIV -gt 0 ]; then \
		PAD=`expr $(PART_ALIGN) - $$DIV` ; \
		dd if=/dev/zero count=1 bs=$$PAD conv=sync 2>/dev/null | tr \\000 \\377 >> $(1) ; \
	fi
endef

# === Targets ===

.PHONY: clean romfs romfs.post image

clean:
	@echo "[*] Cleaning ramdisk..."
	@rm -f $(RAMDISK)

romfs:
	@echo "[*] Generating ROMFS structure..."
	@mkdir -p $(ROMFSDIR)
	@for dir in $(ROMFS_DIRS); do \
		mkdir -p $(ROMFSDIR)/$$dir; \
	done
	@if [ -f $(BOARD_DIR)/l1profile.dat ]; then \
		cp -f $(BOARD_DIR)/l1profile.dat $(ROMFSDIR)/etc_ro/Wireless; \
	fi
	@for f in $(wildcard $(SINGLESKU_DIR)/SingleSKU*.dat); do \
		[ -f $$f ] && cp -f $$f $(ROMFSDIR)/etc_ro/Wireless; \
	done

romfs.post::
	@echo "[*] Compressing ROMFS into SquashFS..."
	@mkdir -p $(IMAGEDIR)
	@rm -f $(RAMDISK) $(KERNELZ).*
	@$(MKSQUASHFS_TOOL) $(ROMFSDIR) $(RAMDISK) $(MKSQUASHFS_OPTS)

image:
	@echo "[*] Building firmware image..."

ifeq ($(CONFIG_WITHOUT_KERNEL),y)
	@echo "[*] Using external kernel image..."
	@( cd $(IMAGEDIR) && rm -f $(KERNELZ).* && cp -f $(MERGE_DIR)/$(CONFIG_FIRMWARE_PRODUCT_ID)/kernel.bin $(KERNELZ).$(COMP) )
else
	@$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -S $(ROOTDIR)/$(LINUXDIR)/vmlinux $(KERNELZ)
ifeq ($(CONFIG_KERNEL_NO_COMPRESS),y)
	@( cd $(IMAGEDIR) && rm -f $(KERNELZ).* && mv -f $(KERNELZ) $(KERNELZ).$(COMP) )
else
	@( cd $(IMAGEDIR) && rm -f $(KERNELZ).* && $(LZMA_TOOL) e -a1 -d25 $(KERNELZ) $(KERNELZ).$(COMP) )
endif
endif

	@echo "[*] Padding kernel image and appending root filesystem..."
	$(call pad_kernel,$(KERNELZ).$(COMP))

	@cat $(RAMDISK) >> $(KERNELZ).$(COMP)
	@wc -c $(KERNELZ).$(COMP)

	@echo "[*] Packing final TRX image..."
	@( cd $(IMAGEDIR) && \
		ENTRY=`LANG=C LC_ALL=C $(ENTRY_CMD)` ; \
		ISIZE=`wc -c $(KERNELZ).$(COMP) | awk '{print $$1}'` ; \
		RSIZE=`wc -c $(RAMDISK) | awk '{print $$1}'` ; \
		KRN_SIZE=`expr $$ISIZE - $$RSIZE + 64` ; \
		$(MKIMAGE_TOOL) -A mips -O linux -T kernel -C $(COMP) -a $(LOAD_ADDR) -e $$ENTRY -k $$KRN_SIZE \
		-n $(CONFIG_FIRMWARE_PRODUCT_ID) -V $(FIRMWARE_KERNEL_VER) $(FIRMWARE_ROOTFS_VER) \
		-d $(KERNELZ).$(COMP) $(IMAGE) )
