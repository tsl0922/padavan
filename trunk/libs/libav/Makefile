include $(ROOTDIR)/rules.mk

PKG_NAME:=libav

ifeq ($(CONFIG_FIRMWARE_INCLUDE_FFMPEG_NEW),y)
PKG_VERSION:=0.11
SRC_DIR = ./ffmpeg-0.11.x
PARAMS_CONFIG=--disable-swresample
else
PKG_VERSION:=0.6
SRC_DIR = ./ffmpeg-0.6.x
PARAMS_CONFIG=
endif

include $(INCLUDE_DIR)/package.mk

PARSERS_CONFIG=--enable-parser=aac --enable-parser=ac3 --enable-parser=mpegaudio

DEMUXERS_CONFIG=--enable-demuxer=aac --enable-demuxer=ac3 --enable-demuxer=asf \
                --enable-demuxer=avi --enable-demuxer=dts --enable-demuxer=eac3 \
                --enable-demuxer=flac --enable-demuxer=flv --enable-demuxer=h264 \
                --enable-demuxer=m4v --enable-demuxer=matroska --enable-demuxer=mov \
                --enable-demuxer=mp3 --enable-demuxer=mpegps --enable-demuxer=mpegts \
                --enable-demuxer=mpegvideo --enable-demuxer=ogg --enable-demuxer=wav

DECODERS_CONFIG=

ifeq ($(CONFIG_FIRMWARE_INCLUDE_FIREFLY),y)
DECODERS_CONFIG+=--enable-decoder=flac --enable-decoder=alac
endif

PROTOCOLS_CONFIG=--enable-protocol=file

TARGET_CFLAGS += -D_LARGEFILE64_SOURCE -D_LFS64_LARGEFILE

CONFIGURE_ARGS = \
	--prefix=$(STAGING_DIR) \
	--cross-prefix=$(TARGET_CROSS) \
	--enable-cross-compile \
	--arch=$(ARCH) \
	--target-os=linux \
	--cc="$(CC)" \
	--as="$(AS)" \
	$(if $(LIBS_BUILD_SHARED),--disable-static,--enable-static) \
	$(if $(LIBS_BUILD_SHARED),--enable-shared,--disable-shared) \
	--enable-small \
	--enable-pthreads \
	--enable-gpl \
	--enable-zlib \
	--disable-doc \
	--disable-debug \
	--disable-avdevice \
	--disable-network \
	--disable-swscale \
	--disable-swscale-alpha \
	--disable-ffmpeg \
	--disable-ffplay \
	--disable-ffprobe \
	--disable-ffserver \
	--disable-hwaccels \
	--disable-dxva2 \
	--disable-indevs \
	--disable-outdevs \
	--disable-devices \
	--disable-filters \
	--disable-encoders \
	--disable-muxers \
	--disable-decoders \
	--disable-demuxers \
	--disable-parsers \
	--disable-postproc \
	--disable-protocols \
	--disable-bsfs \
	--disable-everything \
	$(PARAMS_CONFIG) \
	$(PROTOCOLS_CONFIG) \
	$(PARSERS_CONFIG) \
	$(DECODERS_CONFIG) \
	$(DEMUXERS_CONFIG)

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR)/$(MAKE_PATH) \
		$(MAKE_FLAGS) \
		DESTDIR= \
		install
endef

$(eval $(call BuildPackage,libav))

romfs:
ifeq ($(LIBS_BUILD_SHARED),y)
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libavcodec.so* $(ROMFSDIR)/lib/
	$(CP) $(PKG_INSTALL_DIR)/lib/libavformat.so* $(ROMFSDIR)/lib/
	$(CP) $(PKG_INSTALL_DIR)/lib/libavutil.so* $(ROMFSDIR)/lib/
endif
