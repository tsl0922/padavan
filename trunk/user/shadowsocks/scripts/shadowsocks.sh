#!/bin/sh
#
# Copyright (C) 2017 openwrt-ssr
# Copyright (C) 2017 yushi studio <ywb94@qq.com>
# Copyright (C) 2018 lean <coolsnowwolf@gmail.com>
# Copyright (C) 2019 chongshengB <bkye@vip.qq.com>
# Copyright (C) 2022 TurBoTse <860018505@qq.com>
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.
#

NAME=shadowsocksr
pppoemwan=`nvram get pppoemwan_enable`
http_username=`nvram get http_username`
CONFIG_FILE=/tmp/${NAME}.json
CONFIG_UDP_FILE=/tmp/${NAME}_u.json
CONFIG_SOCK5_FILE=/tmp/${NAME}_s.json
v2_json_file="/tmp/v2-redir.json"
xray_json_file="/tmp/xray-redir.json"
trojan_json_file="/tmp/tj-redir.json"
server_count=0
redir_tcp=0
v2ray_enable=0
xray_enable=0
redir_udp=0
tunnel_enable=0
local_enable=0
pdnsd_enable_flag=0
chinadnsng_enable_flag=0
wan_bp_ips="/tmp/whiteip.txt"
wan_fw_ips="/tmp/blackip.txt"
lan_fp_ips="/tmp/lan_ip.txt"
lan_gm_ips="/tmp/lan_gmip.txt"
run_mode=`nvram get ss_run_mode`
ss_turn=`nvram get ss_turn`
lan_con=`nvram get lan_con`
GLOBAL_SERVER=`nvram get global_server`
socks=""

log() {
	logger -t "$NAME" "$@"
	echo "$(date "+%Y-%m-%d %H:%M:%S") $@" >> "/tmp/ssrplus.log"
}

find_bin() {
	case "$1" in
	ss) ret="/usr/bin/ss-redir" ;;
	ss-local) ret="/usr/bin/ss-local" ;;
	ssr) ret="/usr/bin/ssr-redir" ;;
	ssr-local) ret="/usr/bin/ssr-local" ;;
	ssr-server) ret="/usr/bin/ssr-server" ;;
	v2ray|xray)
		if [ -f "/usr/bin/$1" ]; then
			ret="/usr/bin/$1"
		else
			bin=$(echo -e "v2ray\nxray" | grep -v $1)
			ret="/usr/bin/$bin"
		fi
		;;
	trojan) ret="/usr/bin/trojan" ;;
	socks5) ret="/usr/bin/ipt2socks" ;;
	esac
	echo $ret
}

run_bin() {
	(if [ "$(nvram get ss_cgroups)" = "1" ]; then
	 	echo 0 > /sys/fs/cgroup/cpu/$NAME/tasks
	 	echo 0 > /sys/fs/cgroup/memory/$NAME/tasks
	 fi
	 "$@" > /dev/null 2>&1
	) &
}

cgroups_init() {
	if [ "$(nvram get ss_cgroups)" = "1" ]; then
		cpu_limit=$(nvram get ss_cgoups_cpu_s)
		mem_limit=$(nvram get ss_cgoups_mem_s)
		log "启用进程资源限制, CPU: $cpu_limit, 内存: $mem_limit"
		mkdir -p /sys/fs/cgroup/cpu/$NAME
		mkdir -p /sys/fs/cgroup/memory/$NAME
		echo $cpu_limit > /sys/fs/cgroup/cpu/$NAME/cpu.shares
		echo $mem_limit > /sys/fs/cgroup/memory/$NAME/memory.limit_in_bytes
	fi
}

cgroups_cleanup() {
	cat /sys/fs/cgroup/cpu/$NAME/tasks > /sys/fs/cgroup/cpu/tasks
	cat /sys/fs/cgroup/memory/$NAME/tasks > /sys/fs/cgroup/memory/tasks
	rmdir /sys/fs/cgroup/cpu/$NAME
	rmdir /sys/fs/cgroup/memory/$NAME
}

gen_config_file() {
	fastopen="false"
	case "$2" in
	0) config_file=$CONFIG_FILE && local stype=$(nvram get d_type) ;;
	1) config_file=$CONFIG_UDP_FILE && local stype=$(nvram get ud_type) ;;
	*) config_file=$CONFIG_SOCK5_FILE && local stype=$(nvram get s5_type) ;;
	esac
	local type=$stype
	case "$type" in
	ss)
		lua /etc_ro/ss/genssconfig.lua $1 $3 >$config_file
		sed -i 's/\\//g' $config_file
		;;
	ssr)
		lua /etc_ro/ss/genssrconfig.lua $1 $3 >$config_file
		sed -i 's/\\//g' $config_file
		;;
	trojan)
		if [ "$2" = "0" ]; then
			lua /etc_ro/ss/gentrojanconfig.lua $1 nat 1080 >$trojan_json_file
			sed -i 's/\\//g' $trojan_json_file
		else
			lua /etc_ro/ss/gentrojanconfig.lua $1 client 10801 >/tmp/trojan-ssr-reudp.json
			sed -i 's/\\//g' /tmp/trojan-ssr-reudp.json
		fi
		;;
	v2ray)
		v2ray_enable=1
		if [ "$2" = "1" ]; then
			lua /etc_ro/ss/genv2config.lua $1 udp 1080 >/tmp/v2-ssr-reudp.json
			sed -i 's/\\//g' /tmp/v2-ssr-reudp.json
		else
			lua /etc_ro/ss/genv2config.lua $1 tcp 1080 >$v2_json_file
			sed -i 's/\\//g' $v2_json_file
		fi
		;;
	xray)
		xray_enable=1
		if [ "$2" = "1" ]; then
			lua /etc_ro/ss/genxrayconfig.lua $1 udp 1080 >/tmp/xray-ssr-reudp.json
			sed -i 's/\\//g' /tmp/xray-ssr-reudp.json
		else
			lua /etc_ro/ss/genxrayconfig.lua $1 tcp 1080 >$xray_json_file
			sed -i 's/\\//g' $xray_json_file
		fi
		;;	
	esac
}

get_arg_out() {
	router_proxy="1"
	case "$router_proxy" in
	1) echo "-o" ;;
	2) echo "-O" ;;
	esac
}

start_rules() {
    log "正在添加防火墙规则..."
	lua /etc_ro/ss/getconfig.lua $GLOBAL_SERVER > /tmp/server.txt
	server=`cat /tmp/server.txt` 
	cat /etc/storage/ss_ip.sh | grep -v '^!' | grep -v "^$" >$wan_fw_ips
	cat /etc/storage/ss_wan_ip.sh | grep -v '^!' | grep -v "^$" >$wan_bp_ips
	#resolve name
	if echo $server | grep -E "^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$" >/dev/null; then
		server=${server}
	elif [ "$server" != "${server#*:[0-9a-fA-F]}" ]; then
		server=${server}
	else
		server=$(resolveip -4 -t 3 $server | awk 'NR==1{print}')
		if echo $server | grep -E "^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$" >/dev/null; then
			echo $server >/etc/storage/ssr_ip
		else
			server=$(cat /etc/storage/ssr_ip)
		fi
	fi
	local_port="1080"
	lan_ac_ips=$lan_ac_ips
	lan_ac_mode="b"
	#if [ "$GLOBAL_SERVER" == "$UDP_RELAY_SERVER" ]; then
	#	ARG_UDP="-u"
	if [ "$UDP_RELAY_SERVER" != "nil" ]; then
		ARG_UDP="-U"
		lua /etc_ro/ss/getconfig.lua $UDP_RELAY_SERVER > /tmp/userver.txt
	    udp_server=`cat /tmp/userver.txt` 
		udp_local_port="1080"
	fi
	if [ -n "$lan_ac_ips" ]; then
		case "$lan_ac_mode" in
		w | W | b | B) ac_ips="$lan_ac_mode$lan_ac_ips" ;;
		esac
	fi
	#ac_ips="b"
	gfwmode=""
	if [ "$run_mode" = "gfw" ]; then
		gfwmode="-g"
	elif [ "$run_mode" = "router" ]; then
		gfwmode="-r"
	elif [ "$run_mode" = "oversea" ]; then
		gfwmode="-c"
	elif [ "$run_mode" = "all" ]; then
		gfwmode="-z"
	fi
	if [ "$lan_con" = "0" ]; then
		rm -f $lan_fp_ips
		lancon="all"
		lancons="全部走代理..."
		cat /etc/storage/ss_lan_ip.sh | grep -v '^!' | grep -v "^$" >$lan_fp_ips
	elif [ "$lan_con" = "1" ]; then
		rm -f $lan_fp_ips
		lancon="bip"
		lancons="指定 IP 走代理: 请到规则管理页面添加需要走代理的 IP..."
		cat /etc/storage/ss_lan_bip.sh | grep -v '^!' | grep -v "^$" >$lan_fp_ips
	fi
	rm -f $lan_gm_ips
	cat /etc/storage/ss_lan_gmip.sh | grep -v '^!' | grep -v "^$" >$lan_gm_ips
	dports=$(nvram get s_dports)
	if [ $dports = "0" ]; then
		proxyport=" "
	else
		proxyport="-m multiport --dports 22,53,587,465,995,993,143,80,443"
	fi
	/usr/bin/ss-rules \
		-s "$server" \
		-l "$local_port" \
		-S "$udp_server" \
		-L "$udp_local_port" \
		-a "$ac_ips" \
		-i "" \
		-b "$wan_bp_ips" \
		-w "$wan_fw_ips" \
		-p "$lan_fp_ips" \
		-G "$lan_gm_ips" \
		-D "$proxyport" \
		-k "$lancon" \
		$(get_arg_out) $gfwmode $ARG_UDP
	return $?
}

start_redir_tcp() {
	ARG_OTA=""
	gen_config_file $GLOBAL_SERVER 0 1080
	stype=$(nvram get d_type)
	local bin=$(find_bin $stype)
	[ ! -f "$bin" ] && log "Main node:Can't find $bin program, can't start!" && return 1
	if [ "$(nvram get ss_threads)" = "0" ]; then
		threads=$(cat /proc/cpuinfo | grep 'processor' | wc -l)
	else
		threads=$(nvram get ss_threads)
	fi
	log "正在启动 $stype 服务器..."
	case "$stype" in
	ss | ssr)
		last_config_file=$CONFIG_FILE
		pid_file="/tmp/ssr-retcp.pid"
		for i in $(seq 1 $threads); do
			run_bin $bin -c $CONFIG_FILE $ARG_OTA -f /tmp/ssr-retcp_$i.pid
			usleep 500000
		done
		redir_tcp=1
		log "Shadowsocks/ShadowsocksR $threads 线程启动成功..."
		;;
	trojan)
		for i in $(seq 1 $threads); do
			run_bin $bin --config $trojan_json_file
			usleep 500000
		done
		log "已运行 $($bin --version 2>&1 | head -1)"
		;;
	v2ray)
		run_bin $bin -config $v2_json_file
		log "已运行 $($bin -version | head -1)"
		;;
	xray)
		run_bin $bin -config $xray_json_file
		log "已运行 $($bin -version | head -1)"
		;;	
	socks5)
		for i in $(seq 1 $threads); do
			run_bin lua /etc_ro/ss/gensocks.lua $GLOBAL_SERVER 1080
			usleep 500000
		done
		;;
	esac
	return 0
}

start_redir_udp() {
	if [ "$UDP_RELAY_SERVER" != "nil" ]; then
		redir_udp=1
		utype=$(nvram get ud_type)
		log "正在启动 $utype 游戏 UDP 中继服务器..."
		local bin=$(find_bin $utype)
		[ ! -f "$bin" ] && log "UDP TPROXY Relay:Can't find $bin program, can't start!" && return 1
		case "$utype" in
		ss | ssr)
			ARG_OTA=""
			gen_config_file $UDP_RELAY_SERVER 1 1080
			last_config_file=$CONFIG_UDP_FILE
			pid_file="/var/run/ssr-reudp.pid"
			run_bin $bin -c $last_config_file $ARG_OTA -U -f /var/run/ssr-reudp.pid
			;;
		v2ray)
			gen_config_file $UDP_RELAY_SERVER 1
			run_bin $bin -config /tmp/v2-ssr-reudp.json
			;;
		xray)
			gen_config_file $UDP_RELAY_SERVER 1
			run_bin $bin -config /tmp/xray-ssr-reudp.json
			;;	
		trojan)
			gen_config_file $UDP_RELAY_SERVER 1
			$bin --config /tmp/trojan-ssr-reudp.json >/dev/null 2>&1 &
			run_bin ipt2socks -U -b 0.0.0.0 -4 -s 127.0.0.1 -p 10801 -l 1080
			;;
		socks5)
			echo "1"
			;;
		esac
	fi
	return 0
}

start_dns() {
	echo "create china hash:net family inet hashsize 1024 maxelem 65536" >/tmp/china.ipset
	awk '!/^$/&&!/^#/{printf("add china %s'" "'\n",$0)}' /etc/storage/chinadns/chnroute.txt >>/tmp/china.ipset
	ipset -! flush china
	ipset -! restore </tmp/china.ipset 2>/dev/null
	rm -f /tmp/china.ipset
	case "$run_mode" in
	router)
		dnsstr="$(nvram get tunnel_forward)"
		dnsserver=$(echo "$dnsstr" | awk -F '#' '{print $1}')
		#dnsport=$(echo "$dnsstr" | awk -F '#' '{print $2}')
		log "已启用 $dnsstr 端口..."
		dns2tcp -L"127.0.0.1#5353" -R"$dnsstr" >/dev/null 2>&1 &
		pdnsd_enable_flag=0
		log "开始处理 GFWList..."
	;;
	gfw)
		dnsstr="$(nvram get tunnel_forward)"
		dnsserver=$(echo "$dnsstr" | awk -F '#' '{print $1}')
		#dnsport=$(echo "$dnsstr" | awk -F '#' '{print $2}')
		ipset add gfwlist $dnsserver 2>/dev/null
		log "已启用 $dnsstr 端口..."
		dns2tcp -L"127.0.0.1#5353" -R"$dnsstr" >/dev/null 2>&1 &
		pdnsd_enable_flag=0
		log "开始处理 GFWList..."
		;;
	oversea)
		ipset add gfwlist $dnsserver 2>/dev/null
		mkdir -p /etc/storage/dnsmasq.oversea
		sed -i '/dnsmasq-ss/d' /etc/storage/dnsmasq/dnsmasq.conf
		sed -i '/dnsmasq.oversea/d' /etc/storage/dnsmasq/dnsmasq.conf
		cat >>/etc/storage/dnsmasq/dnsmasq.conf <<EOF
conf-dir=/etc/storage/dnsmasq.oversea
EOF
;;
	*)
		ipset -N ss_spec_wan_ac hash:net 2>/dev/null
		ipset add ss_spec_wan_ac $dnsserver 2>/dev/null
	;;
	esac
	log "正在重启 DNSmasq 进程..."
	/sbin/restart_dhcpd
	log "DNSmasq 进程已重启..."
}

start_AD() {
	mkdir -p /tmp/dnsmasq.dom
	curl -s -o /tmp/adnew.conf --connect-timeout 10 --retry 3 $(nvram get ss_adblock_url)
	if [ ! -f "/tmp/adnew.conf" ]; then
		log "广告过滤功能未开启或者过滤地址失效，网络异常等 ！！！"
	else
		log "去广告文件下载成功广告过滤功能已启用..."
		if [ -f "/tmp/adnew.conf" ]; then
			check = `grep -wq "address=" /tmp/adnew.conf`
	  		if [ ! -n "$check" ] ; then
				cp /tmp/adnew.conf /tmp/dnsmasq.dom/anti-ad-for-dnsmasq.conf
	  		else
			    cat /tmp/adnew.conf | grep ^\|\|[^\*]*\^$ | sed -e 's:||:address\=\/:' -e 's:\^:/0\.0\.0\.0:' > /tmp/dnsmasq.dom/anti-ad-for-dnsmasq.conf
			fi
		fi
	fi
	rm -f /tmp/adnew.conf
}

# ========== 启动 Socks5 代理 ==========
start_local() {
	local s5_port=$(nvram get socks5_port)
	local local_server=$(nvram get socks5_enable)
	[ "$local_server" == "nil" ] && return 1
	[ "$local_server" == "same" ] && local_server=$GLOBAL_SERVER
	local type=$(nvram get s5_type)
	local bin=$(find_bin $type)
	[ ! -f "$bin" ] && log "Global_Socks5:Can't find $bin program, can't start!" && return 1
	case "$type" in
	ss | ssr)
		local name="Shadowsocks"
		local bin=$(find_bin ss-local)
		[ ! -f "$bin" ] && log "Global_Socks5:Can't find $bin program, can't start!" && return 1
		[ "$type" == "ssr" ] && name="ShadowsocksR"
		gen_config_file $local_server 3 $s5_port
		run_bin $bin -c $CONFIG_SOCK5_FILE -u -f /var/run/ssr-local.pid
		log "Global_Socks5:$name Started!"
		;;
	v2ray)
		lua /etc_ro/ss/genv2config.lua $local_server tcp 0 $s5_port >/tmp/v2-ssr-local.json
		sed -i 's/\\//g' /tmp/v2-ssr-local.json
		run_bin $bin -config /tmp/v2-ssr-local.json
		log "Global_Socks5:$($bin -version | head -1) Started!"
		;;
	xray)
		lua /etc_ro/ss/genxrayconfig.lua $local_server tcp 0 $s5_port >/tmp/xray-ssr-local.json
		sed -i 's/\\//g' /tmp/xray-ssr-local.json
		run_bin $bin -config /tmp/xray-ssr-local.json
		log "Global_Socks5:$($bin -version | head -1) Started!"
		;;
	trojan)
		lua /etc_ro/ss/gentrojanconfig.lua $local_server client $s5_port >/tmp/trojan-ssr-local.json
		sed -i 's/\\//g' /tmp/trojan-ssr-local.json
		run_bin $bin --config /tmp/trojan-ssr-local.json
		log "Global_Socks5:$($bin --version 2>&1 | head -1) Started!"
		;;
	*)
		[ -e /proc/sys/net/ipv6 ] && local listenip='-i ::'
		run_bin microsocks $listenip -p $s5_port ssr-local
		log "Global_Socks5:$type Started!"
		;;
	esac
	local_enable=1
	return 0
}

rules() {
	[ "$GLOBAL_SERVER" = "nil" ] && return 1
	UDP_RELAY_SERVER=$(nvram get udp_relay_server)
	if [ "$UDP_RELAY_SERVER" = "same" ]; then
		UDP_RELAY_SERVER=$GLOBAL_SERVER
	fi
	if start_rules; then
		return 0
	else
		return 1
	fi
}

start_watchcat() {
	if [ $(nvram get ss_watchcat) = 1 ]; then
		let total_count=server_count+redir_tcp+redir_udp+tunnel_enable+v2ray_enable+xray_enable+local_enable+pdnsd_enable_flag+chinadnsng_enable_flag
		if [ $total_count -gt 0 ]; then
			#param:server(count) redir_tcp(0:no,1:yes)  redir_udp tunnel kcp local gfw
			/usr/bin/ssr-monitor $server_count $redir_tcp $redir_udp $tunnel_enable $v2ray_enable $xray_enable $local_enable $pdnsd_enable_flag $chinadnsng_enable_flag >/dev/null 2>&1 &
		fi
	fi
}

auto_update() {
	sed -i '/update_chnroute/d' /etc/storage/cron/crontabs/$http_username
	sed -i '/update_gfwlist/d' /etc/storage/cron/crontabs/$http_username
	sed -i '/ss-watchcat/d' /etc/storage/cron/crontabs/$http_username
	if [ $(nvram get ss_update_chnroute) = "1" ]; then
		cat >>/etc/storage/cron/crontabs/$http_username <<EOF
0 7 * * * /usr/bin/update_chnroute.sh > /dev/null 2>&1
EOF
	fi
	if [ $(nvram get ss_update_gfwlist) = "1" ]; then
		cat >>/etc/storage/cron/crontabs/$http_username <<EOF
0 8 * * * /usr/bin/update_gfwlist.sh > /dev/null 2>&1
EOF
	fi
}

# ========== 启动 SS ==========
ssp_start() { 
	ss_enable=`nvram get ss_enable`
	if rules; then
		cgroups_init
		if start_redir_tcp; then
			start_redir_udp
			#start_AD
			start_dns
		fi
	fi
	start_local
	start_watchcat
	auto_update
	ENABLE_SERVER=$(nvram get global_server)
	[ "$ENABLE_SERVER" = "nil" ] && return 1
	log "已启动科学上网..."
	log "内网控制为: $lancons"
	nvram set check_mode=0
	if [ "$pppoemwan" = 0 ]; then
		/usr/bin/detect.sh
	fi
}

# ========== 关闭 SS ==========
ssp_close() {
	rm -rf /tmp/cdn
	/usr/bin/ss-rules -f
	kill -9 $(ps | grep ssr-switch | grep -v grep | awk '{print $1}') >/dev/null 2>&1
	kill -9 $(ps | grep ssr-monitor | grep -v grep | awk '{print $1}') >/dev/null 2>&1
	kill_process
	cgroups_cleanup
	sed -i '/no-resolv/d' /etc/storage/dnsmasq/dnsmasq.conf
	sed -i '/server=127.0.0.1/d' /etc/storage/dnsmasq/dnsmasq.conf
	sed -i '/cdn/d' /etc/storage/dnsmasq/dnsmasq.conf
	sed -i '/gfwlist/d' /etc/storage/dnsmasq/dnsmasq.conf
	sed -i '/dnsmasq.oversea/d' /etc/storage/dnsmasq/dnsmasq.conf
	if [ -f "/etc/storage/dnsmasq-ss.d" ]; then
		rm -f /etc/storage/dnsmasq-ss.d
	fi
	clear_iptable
	log "正在重启 DNSmasq 进程..."
	/sbin/restart_dhcpd
	log "DNSmasq 进程已重启..."
	if [ "$pppoemwan" = 0 ]; then
		/usr/bin/detect.sh
	fi
}


clear_iptable() {
	s5_port=$(nvram get socks5_port)
	iptables -t filter -D INPUT -p tcp --dport $s5_port -j ACCEPT
	iptables -t filter -D INPUT -p tcp --dport $s5_port -j ACCEPT
	ip6tables -t filter -D INPUT -p tcp --dport $s5_port -j ACCEPT
	ip6tables -t filter -D INPUT -p tcp --dport $s5_port -j ACCEPT
}

kill_process() {
	xray_process=$(pidof v2ray || pidof xray)
	if [ -n "$xray_process" ]; then
		log "关闭 XRay 进程..."
		killall v2ray xray >/dev/null 2>&1
		kill -9 "$xray_process" >/dev/null 2>&1
	fi

	ssredir=$(pidof ss-redir)
	if [ -n "$ssredir" ]; then
		log "关闭 ss-redir 进程..."
		killall ss-redir >/dev/null 2>&1
		kill -9 "$ssredir" >/dev/null 2>&1
	fi

	rssredir=$(pidof ssr-redir)
	if [ -n "$rssredir" ]; then
		log "关闭 ssr-redir 进程..."
		killall ssr-redir >/dev/null 2>&1
		kill -9 "$rssredir" >/dev/null 2>&1
	fi

	sslocal_process=$(pidof ss-local)
	if [ -n "$sslocal_process" ]; then
		log "关闭 ss-local 进程..."
		killall ss-local >/dev/null 2>&1
		kill -9 "$sslocal_process" >/dev/null 2>&1
	fi

	trojandir=$(pidof trojan)
	if [ -n "$trojandir" ]; then
		log "关闭 trojan 进程..."
		killall trojan >/dev/null 2>&1
		kill -9 "$trojandir" >/dev/null 2>&1
	fi
	
	ipt2socks_process=$(pidof ipt2socks)
	if [ -n "$ipt2socks_process" ]; then
		log "关闭 ipt2socks 进程..."
		killall ipt2socks >/dev/null 2>&1
		kill -9 "$ipt2socks_process" >/dev/null 2>&1
	fi

	socks5_process=$(pidof srelay)
	if [ -n "$socks5_process" ]; then
		log "关闭 socks5 进程..."
		killall srelay >/dev/null 2>&1
		kill -9 "$socks5_process" >/dev/null 2>&1
	fi

	ssrs_process=$(pidof ssr-server)
	if [ -n "$ssrs_process" ]; then
		log "关闭 ssr-server 进程..."
		killall ssr-server >/dev/null 2>&1
		kill -9 "$ssrs_process" >/dev/null 2>&1
	fi

	cnd_process=$(pidof chinadns-ng)
	if [ -n "$cnd_process" ]; then
		log "关闭 chinadns-ng 进程..."
		killall chinadns-ng >/dev/null 2>&1
		kill -9 "$cnd_process" >/dev/null 2>&1
	fi

	dns2tcp_process=$(pidof dns2tcp)
	if [ -n "$dns2tcp_process" ]; then
		log "关闭 dns2tcp 进程..."
		killall dns2tcp >/dev/null 2>&1
		kill -9 "$dns2tcp_process" >/dev/null 2>&1
	fi

	microsocks_process=$(pidof microsocks)
	if [ -n "$microsocks_process" ]; then
		log "关闭 socks5 服务端进程..."
		killall microsocks >/dev/null 2>&1
		kill -9 "$microsocks_process" >/dev/null 2>&1
	fi
}

# ========== 启用备用服务器 ==========
ressp() {
	BACKUP_SERVER=$(nvram get backup_server)
	start_redir $BACKUP_SERVER
	start_rules $BACKUP_SERVER
	start_dns
	start_local
	start_watchcat
	auto_update
	ENABLE_SERVER=$(nvram get global_server)
	log "备用服务器启动成功..."
	log "内网控制为: $lancons"
}

check_smsrtdns() {
	smartdns_process=$(pidof smartdns)
	if [ -n "$smartdns_process" ] && [ $(nvram get sdns_enable) = 1 ] ; then
		log "检测到 SmartDNS 已开启,正在重启 SmartDNS..."
		[ $(pidof smartdns | awk '{ print $1 }')x != x ] && killall -9 smartdns >/dev/null 2>&1
		/usr/bin/smartdns.sh start
	fi
}

case $1 in
start)
	if [ $(nvram get ss_adblock) = "1" ]; then
		start_AD
	fi
	ssp_start
	smartdns_process=$(pidof smartdns)
	if [ -n "$smartdns_process" ] && [ $(nvram get sdns_enable) = 1 ] ; then
		sleep 2
		check_smsrtdns
	fi
	echo 3 > /proc/sys/vm/drop_caches
	;;
stop)
	killall -q -9 ssr-switch
	ssp_close
	dns2tcp_process=$(pidof dns2tcp)
	smartdns_process=$(pidof smartdns)
	if [ -n "$dns2tcp_process" ] && [ -n "$smartdns_process" ] && [ $(nvram get sdns_enable) = 1 ] ; then
		sleep 2
		check_smsrtdns
	else
		if [ -n "$smartdns_process" ] && [ $(nvram get ss_enable) = 0 ] ; then
			sleep 2
			check_smsrtdns
		fi
	fi
	echo 3 > /proc/sys/vm/drop_caches
	;;
restart)
	ssp_close
	ssp_start
	if [ $(nvram get sdns_enable) = 1 ] ; then
		sleep 2
		check_smsrtdns
	fi
	echo 3 > /proc/sys/vm/drop_caches
	;;
reserver)
	ssp_close
	ressp
	if [ $(nvram get sdns_enable) = 1 ] ; then
		sleep 2
		check_smsrtdns
	fi
	echo 3 > /proc/sys/vm/drop_caches
	;;
*)
	echo "check"
	#exit 0
	;;
esac

