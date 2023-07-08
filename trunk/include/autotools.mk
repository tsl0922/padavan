# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2007-2020 OpenWrt.org

ifneq ($(__autotools_inc),1)
__autotools_inc=1

autoconf_bool = $(patsubst %,$(if $($(1)),--enable,--disable)-%,$(2))

# 1: build dir
# 2: remove files
# 3: automake paths
# 4: libtool paths
# 5: extra m4 dirs
define autoreconf
	(cd $(1); \
		$(patsubst %,rm -f %;,$(2)) \
		$(foreach p,$(3), \
			if [ -f $(p)/configure.ac ] || [ -f $(p)/configure.in ]; then \
				[ -d $(p)/autom4te.cache ] && rm -rf $(p)/autom4te.cache; \
				[ -e $(p)/config.rpath ] || \
						ln -s $(SCRIPT_DIR)/config.rpath $(p)/config.rpath; \
				touch NEWS AUTHORS COPYING ABOUT-NLS ChangeLog; mkdir -p $(5); \
				autoreconf -v -f -i -s \
					$(if $(word 2,$(3)),--no-recursive) \
					$(patsubst %,-I %,$(5)) \
					$(patsubst %,-I %,$(4)) $(p) || true; \
			fi; \
		) \
	);
endef

# 1: build dir
define patch_libtool
	@(cd $(1); \
		for lt in $$$$(find . -name ltmain.sh); do \
			lt_version="$$$$(sed -ne 's,^[[:space:]]*VERSION="\?\([0-9]\.[0-9]\+\).*,\1,p' $$$$lt)"; \
			case "$$$$lt_version" in \
				1.5|2.2|2.4) echo "autotools.mk: Found libtool v$$$$lt_version - applying patch to $$$$lt"; \
					(cd $$$$(dirname $$$$lt) && $$(PATCH) -N -s -p1 < $$(TOPDIR)/tools/libtool/files/libtool-v$$$$lt_version.patch || true) ;; \
				*) echo "autotools.mk: error: Unsupported libtool version v$$$$lt_version - cannot patch $$$$lt"; exit 1 ;; \
			esac; \
		done; \
	);
endef

define set_libtool_abiver
	sed -i \
		-e 's,^soname_spec=.*,soname_spec="\\$$$${libname}\\$$$${shared_ext}.$(PKG_ABI_VERSION)",' \
		-e 's,^library_names_spec=.*,library_names_spec="\\$$$${libname}\\$$$${shared_ext}.$(PKG_ABI_VERSION) \\$$$${libname}\\$$$${shared_ext}",' \
		$(PKG_BUILD_DIR)/libtool
endef

PKG_LIBTOOL_PATHS?=$(CONFIGURE_PATH)
PKG_AUTOMAKE_PATHS?=$(CONFIGURE_PATH)
PKG_MACRO_PATHS?=m4
PKG_REMOVE_FILES?=aclocal.m4

define autoreconf_target
  $(strip $(call autoreconf, \
    $(PKG_BUILD_DIR), $(PKG_REMOVE_FILES), \
    $(PKG_AUTOMAKE_PATHS), $(PKG_LIBTOOL_PATHS), \
    aclocal $(STAGING_DIR)/share/aclocal $(PKG_MACRO_PATHS)))
endef

define patch_libtool_target
  $(strip $(call patch_libtool, \
    $(PKG_BUILD_DIR)))
endef

define gettext_version_target
	(cd $(PKG_BUILD_DIR) && \
		GETTEXT_VERSION=$(shell gettext -V | sed -rne '1s/.*\b([0-9]\.[0-9]+(\.[0-9]+)?)\b.*/\1/p' ) && \
		sed \
			-i $(PKG_BUILD_DIR)/configure.ac \
			-e "s/AM_GNU_GETTEXT_VERSION(.*)/AM_GNU_GETTEXT_VERSION(\[$$$$GETTEXT_VERSION\])/g" && \
		autopoint --force \
	);
endef

ifneq ($(filter gettext-version,$(PKG_FIXUP)),)
  Hooks/Configure/Pre += gettext_version_target
 ifeq ($(filter no-autoreconf,$(PKG_FIXUP)),)
  Hooks/Configure/Pre += autoreconf_target
 endif
endif

ifneq ($(filter patch-libtool,$(PKG_FIXUP)),)
  Hooks/Configure/Pre += patch_libtool_target
endif

ifneq ($(filter libtool,$(PKG_FIXUP)),)
  PKG_BUILD_DEPENDS += libtool
 ifeq ($(filter no-autoreconf,$(PKG_FIXUP)),)
  Hooks/Configure/Pre += autoreconf_target
 endif
endif

ifneq ($(filter libtool-abiver,$(PKG_FIXUP)),)
  Hooks/Configure/Post += set_libtool_abiver
endif

ifneq ($(filter autoreconf,$(PKG_FIXUP)),)
  ifeq ($(filter autoreconf,$(Hooks/Configure/Pre)),)
    Hooks/Configure/Pre += autoreconf_target
  endif
endif

endif #__autotools_inc
