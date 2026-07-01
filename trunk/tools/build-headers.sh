#!/bin/bash

THISDIR=`pwd`

. ${THISDIR}/../.config

CONFIG_CROSS_COMPILER_PATH="$CONFIG_CROSS_COMPILER_ROOT/bin"

export PATH="${CONFIG_CROSS_COMPILER_PATH}:$PATH"

build_dir="${THISDIR}/headers"

rm -rf ${build_dir}
mkdir -p ${build_dir}/usr

cd ../${CONFIG_LINUXDIR}
make O=${build_dir} ARCH=mips HOSTCC=cc CROSS_COMPILE=$(CONFIG_CROSS_COMPILE)- INSTALL_HDR_PATH=${build_dir}/usr headers_install

find ${build_dir}/usr -type f -name ".*" -exec rm "{}" \;
