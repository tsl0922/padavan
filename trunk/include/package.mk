# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2020 OpenWrt.org

PKG_BUILD_DIR ?= $(BUILD_DIR)/$(PKG_NAME)$(if $(PKG_VERSION),-$(PKG_VERSION))
PKG_INSTALL_DIR ?= $(STAGING_DIR)
PKG_JOBS ?= -j$(HOST_NCPU)
SRC_DIR ?= ./src
PATCH_DIR ?= ./patches

PKG_BUILD_FLAGS?=
__unknown_flags=$(filter-out gc-sections no-gc-sections lto no-lto,$(PKG_BUILD_FLAGS))
ifneq ($(__unknown_flags),)
  $(error unknown PKG_BUILD_FLAGS: $(__unknown_flags))
endif

# $1=flagname, $2=default (0/1)
define pkg_build_flag
$(if $(filter no-$(1),$(PKG_BUILD_FLAGS)),0,$(if $(filter $(1),$(PKG_BUILD_FLAGS)),1,$(2)))
endef

ifeq ($(call pkg_build_flag,gc-sections,$(if $(CONFIG_USE_GC_SECTIONS),1,0)),1)
  TARGET_CFLAGS+= -ffunction-sections -fdata-sections
  TARGET_CXXFLAGS+= -ffunction-sections -fdata-sections
  TARGET_LDFLAGS+= -Wl,--gc-sections
endif
ifeq ($(call pkg_build_flag,lto,$(if $(CONFIG_USE_LTO),1,0)),1)
  TARGET_CFLAGS+= -flto=auto -fno-fat-lto-objects
  TARGET_CXXFLAGS+= -flto=auto -fno-fat-lto-objects
  TARGET_LDFLAGS+= -flto=auto -fuse-linker-plugin
endif

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/unpack.mk
include $(INCLUDE_DIR)/autotools.mk

define PatchDir
	@if [ -d "$(2)" ] && [ "$$$$(ls $(2) | wc -l)" -gt 0 ]; then \
		for p in $(2)/*.patch; do \
			echo "Applying $$$$p ..."; \
			$(PATCH) -d $(1) -p1 < $$$$p; \
		done \
	fi
endef

define Build/Patch/Default
	$(call PatchDir,$(PKG_BUILD_DIR),$(PATCH_DIR))
endef

Build/Patch:=$(Build/Patch/Default)
define Build/Prepare/Default
	$(PKG_UNPACK)
	[ ! -d $(SRC_DIR) ] || $(CP) $(SRC_DIR)/. $(PKG_BUILD_DIR)
	$(Build/Patch)
endef

EXTRA_CXXFLAGS = $(EXTRA_CFLAGS)

CONFIGURE_PREFIX:=
CONFIGURE_ARGS = \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--disable-dependency-tracking \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=$(CONFIGURE_PREFIX) \
		--exec-prefix=$(CONFIGURE_PREFIX) \
		--bindir=$(CONFIGURE_PREFIX)/bin \
		--sbindir=$(CONFIGURE_PREFIX)/sbin \
		--libexecdir=$(CONFIGURE_PREFIX)/lib \
		--sysconfdir=/etc \
		--datadir=$(CONFIGURE_PREFIX)/share \
		--localstatedir=/var \
		--mandir=$(CONFIGURE_PREFIX)/man \
		--infodir=$(CONFIGURE_PREFIX)/info \
		$(DISABLE_IPV6)

CONFIGURE_VARS = \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(EXTRA_CXXFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS)" \

CONFIGURE_PATH = .
CONFIGURE_CMD = ./configure

define Build/Configure/Default
	(cd $(PKG_BUILD_DIR)/$(CONFIGURE_PATH); \
	if [ -x $(CONFIGURE_CMD) ]; then \
		$(CONFIGURE_VARS) \
		$(CONFIGURE_CMD) \
		$(CONFIGURE_ARGS); \
	fi; \
	)
endef

MAKE_VARS = \
	CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(EXTRA_CXXFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS)"

MAKE_FLAGS = \
	$(TARGET_CONFIGURE_OPTS) \
	CROSS="$(TARGET_CROSS)" \
	ARCH="$(ARCH)"

MAKE_INSTALL_FLAGS = \
	$(MAKE_FLAGS) \
	DESTDIR="$(PKG_INSTALL_DIR)"

MAKE_PATH ?= .

define Build/Compile/Default
	+$(MAKE_VARS) \
	$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/$(MAKE_PATH) \
		$(MAKE_FLAGS) \
		$(1)
endef

define Build/Install/Default
	$(MAKE_VARS) \
	$(MAKE) -C $(PKG_BUILD_DIR)/$(MAKE_PATH) \
		$(MAKE_INSTALL_FLAGS) \
		$(if $(1), $(1), install)
endef

STAMP_PREPARED=$(PKG_BUILD_DIR)/.prepared
STAMP_CONFIGURED=$(PKG_BUILD_DIR)/.configured
STAMP_BUILT=$(PKG_BUILD_DIR)/.built
STAMP_INSTALLED=$(PKG_BUILD_DIR)/.installed

Build/Prepare=$(call Build/Prepare/Default,)
Build/Configure=$(call Build/Configure/Default,)
Build/Compile=$(call Build/Compile/Default,)
Build/Install=$(call Build/Install/Default,)

define BuildPackage
  .DEFAULT_GOAL := all
  .PHONY: all install clean

  all: $(STAMP_BUILT) $(if $(PKG_INSTALL),$(STAMP_INSTALLED))

  install: $(STAMP_INSTALLED)

  clean:
	rm -rf $(PKG_BUILD_DIR)

  $(if $(strip $(PKG_SOURCE_URL)),$(call Download,default))

  $(STAMP_PREPARED):
	@-rm -rf $(PKG_BUILD_DIR)
	@mkdir -p $(PKG_BUILD_DIR)
	touch $$@_check
	$(foreach hook,$(Hooks/Prepare/Pre),$(call $(hook))$(sep))
	$(Build/Prepare)
	$(foreach hook,$(Hooks/Prepare/Post),$(call $(hook))$(sep))
	touch $$@

  $(STAMP_CONFIGURED): $(STAMP_PREPARED)
	$(foreach hook,$(Hooks/Configure/Pre),$(call $(hook))$(sep))
	$(Build/Configure)
	$(foreach hook,$(Hooks/Configure/Post),$(call $(hook))$(sep))
	touch $$@

  $(STAMP_BUILT): $(STAMP_CONFIGURED)
	rm -f $$@
	touch $$@_check
	$(foreach hook,$(Hooks/Compile/Pre),$(call $(hook))$(sep))
	$(Build/Compile)
	$(foreach hook,$(Hooks/Compile/Post),$(call $(hook))$(sep))
	touch $$@

  $(STAMP_INSTALLED): $(STAMP_BUILT)
	$(Build/Install)
	$(foreach hook,$(Hooks/Install/Post),$(call $(hook))$(sep))
	touch $$@
endef