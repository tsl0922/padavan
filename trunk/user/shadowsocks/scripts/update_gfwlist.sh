#!/bin/sh

set -e -o pipefail

NAME=shadowsocksr
GFWLIST_URL="$(nvram get ss_gfwlist_url)"

log() {
	logger -t "$NAME" "$@"
	echo "$(date "+%Y-%m-%d %H:%M:%S") $@" >> "/tmp/ssrplus.log"
}

[ "$1" != "force" ] && [ "$(nvram get ss_update_gfwlist)" != "1" ] && exit 0

log "开始更新 GFWList..."
[ ! -d /etc/storage/gfwlist/ ] && mkdir -p /etc/storage/gfwlist/
curl -s -o /tmp/gfwlist_list_origin.conf --connect-timeout 10 --retry 3 $GFWLIST_URL
lua /etc_ro/ss/gfwupdate.lua
count=`awk '{print NR}' /tmp/gfwlist_list.conf|tail -n1`
if [ $count -gt 1000 ]; then
    rm -f /etc/storage/gfwlist/gfwlist_list.conf
    mv -f /tmp/gfwlist_list.conf /etc/storage/gfwlist/gfwlist_list.conf
	mtd_storage.sh save >/dev/null 2>&1
	log "GFWList 已更新..."
	if [ $(nvram get ss_enable) = 1 ]; then
		lua /etc_ro/ss/gfwcreate.lua
		log "正在重启 ShadowSocksR Plus..."
		/usr/bin/shadowsocks.sh stop
		/usr/bin/shadowsocks.sh start
	fi
else
	log "GFWList 下载失败,请重试 ！！！"
fi
rm -f /tmp/gfwlist_list_origin.conf
