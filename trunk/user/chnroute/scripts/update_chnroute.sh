#!/bin/sh

set -e -o pipefail

NAME=chnroute
CHNROUTE_URL="$(nvram get ss_chnroute_url)"

log() {
	logger -t "$NAME" "$@"
	echo "$(date "+%Y-%m-%d %H:%M:%S") $@" >> "/tmp/ssrplus.log"
}

[ "$1" != "force" ] && [ "$(nvram get ss_update_chnroute)" != "1" ] && exit 0

log "开始更新 CHNRoute..."
[ ! -d /etc/storage/chinadns/ ] && mkdir /etc/storage/chinadns/
rm -f /tmp/chinadns_chnroute.txt

if [ -z "$CHNROUTE_URL" ]; then
	curl -s -o /tmp/chinadns_chnroute.txt --connect-timeout 10 --retry 3 "$CHNROUTE_URL"
else
	curl -s --connect-timeout 10 --retry 3 http://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest | \
		awk -F\| '/CN\|ipv4/ { printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > /tmp/chinadns_chnroute.txt
fi

mv -f /tmp/chinadns_chnroute.txt /etc/storage/chinadns/chnroute.txt
mtd_storage.sh save >/dev/null 2>&1
log "CHNRoute 已更新..."

[ -f /usr/bin/shadowsocks.sh ] && [ "$(nvram get ss_enable)" = "1" ] && [ "$(nvram get ss_run_mode)" = "router" ] && /usr/bin/shadowsocks.sh restart >/dev/null 2>&1

