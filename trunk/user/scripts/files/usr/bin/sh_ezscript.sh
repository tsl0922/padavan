connAPSite () {
. /etc/storage/ap_script.sh
logger -t "【连接 AP】" "10秒后, 自动搜寻 ap"
sleep 10
ap_fenge=`nvram get ap_fenge`
radio2_apcli=`nvram get radio2_apcli`
[ -z $radio2_apcli ] && radio2_apcli="apcli0"
radio5_apcli=`nvram get radio5_apcli`
[ -z $radio5_apcli ] && radio5_apcli="apclii0"
cat /tmp/ap2g5g.txt | grep -v '^#'  | grep -v "^$" > /tmp/ap2g5g
if [ ! -f /tmp/apc.lock ] && [ -s /tmp/ap2g5g ] ; then
touch /tmp/apc.lock
a2=`iwconfig $radio2_apcli | awk -F'"' '/ESSID/ {print $2}'`
a5=`iwconfig $radio5_apcli | awk -F'"' '/ESSID/ {print $2}'`
[ "$a2" = "" -a "$a5" = "" ] && ap=1 || ap=0
if [ "$ap" = "1" ] || [ "$1" = "scan" ] ; then
#搜寻开始/tmp/ap2g5g
	if [ ! -z "$(cat /tmp/ap2g5g | cut -d $ap_fenge -f1 | grep "2")" ]; then
	radio_main="$(nvram get radio2_main)"
	[ -z "$radio_main" ] && radio_main="ra0"
	logger -t "【连接 AP】" "scan 2g $radio_main"
	iwpriv $radio_main set SiteSurvey=1
	sleep 1
	wds_aplist2g="$(iwpriv $radio_main get_site_survey)"
	[ ! -z "$(echo "$wds_aplist"| grep "get_site_survey:No BssInfo")" ] && sleep 2 && wds_aplist="$(iwpriv $radio_main get_site_survey)"
	[ ! -z "$(echo "$wds_aplist"| grep "get_site_survey:No BssInfo")" ] && sleep 3 && wds_aplist="$(iwpriv $radio_main get_site_survey)"
	aplist_n="$(echo "$wds_aplist2g" | sed -n '2p')"
	fi
	if [ ! -z "$(cat /tmp/ap2g5g | cut -d $ap_fenge -f1 | grep "5")" ] ; then
	radio_main="$(nvram get radio5_main)"
	[ -z "$radio_main" ] && radio_main="rai0"
	logger -t "【连接 AP】" "scan 5g $radio_main"
	iwpriv $radio_main set SiteSurvey=1
	sleep 1
	wds_aplist5g="$(iwpriv $radio_main get_site_survey)"
	[ ! -z "$(echo "$wds_aplist"| grep "get_site_survey:No BssInfo")" ] && sleep 2 && wds_aplist="$(iwpriv $radio_main get_site_survey)"
	[ ! -z "$(echo "$wds_aplist"| grep "get_site_survey:No BssInfo")" ] && sleep 3 && wds_aplist="$(iwpriv $radio_main get_site_survey)"
	aplist_n="$(echo "$wds_aplist5g" | sed -n '2p')"
	fi
	ap3=1
	ap_rule=`nvram get ap_rule`
	# 【0】从第一行开始（第一行的是最优先信号）；
	while read line
	do
		apc=`echo "$line" | grep -v '^#' | grep -v "^$"`
		if [ ! -z "$apc" ] ; then
		if [ "$ap_rule" = "1" ] ; then
			# 只执行一次【1】不分顺序自动连接最强信号
			echo -n "" > /tmp/ap2g5g_site_survey ; echo -n "" > /tmp/ap2g5g_apc
			echo "$wds_aplist2g" | while read i_a ; do 
			if [ ! -z "$i_a" ] ; then
			[ -s /tmp/ap2g5g_site_survey ] && continue #跳出当前循环
			for i_b in `cat /tmp/ap2g5g  | grep -v '^#' | grep -v "^$" | grep "^2"` ; do
			if [ ! -z "$i_b" ] ; then
			[ -s /tmp/ap2g5g_apc ] && continue #跳出当前循环
			site_survey=""
			rtwlt_sta_ssid="$(echo "$i_b" | cut -d $ap_fenge -f4)"
			rtwlt_sta_bssid="$(echo "$i_b" | cut -d $ap_fenge -f6 | tr 'A-Z' 'a-z')"
			[ ! -z "$rtwlt_sta_ssid" ] && site_survey="$(echo "$i_a" | grep -Eo '[0-9]*[\ ]*'"$rtwlt_sta_ssid".* | tr 'A-Z' 'a-z' | sed -n '1p')"
			[ ! -z "$rtwlt_sta_bssid" ] && site_survey="$(echo "$i_a" | tr 'A-Z' 'a-z' | sed -n "/$rtwlt_sta_bssid/p" | sed -n '1p')"
			[ ! -z "$rtwlt_sta_ssid" ] && [ ! -z "$rtwlt_sta_bssid" ] && site_survey="$(echo "$i_a" | grep -Eo '[0-9]*[\ ]*'"$rtwlt_sta_ssid".* | tr 'A-Z' 'a-z' | sed -n "/$rtwlt_sta_bssid/p" | sed -n '1p')"
			[ ! -z "$site_survey" ] && echo "$site_survey" > /tmp/ap2g5g_site_survey
			[ ! -z "$site_survey" ] && echo "$i_b" > /tmp/ap2g5g_apc
			[ ! -z "$site_survey" ] && continue #跳出当前循环
			fi
			done
			fi
			done
			echo "$wds_aplist5g" | while read i_a ; do 
			if [ ! -z "$i_a" ] ; then
			[ -s /tmp/ap2g5g_site_survey ] && continue #跳出当前循环
			for i_b in `cat /tmp/ap2g5g  | grep -v '^#' | grep -v "^$" | grep "^5"` ; do
			if [ ! -z "$i_b" ] ; then
			[ -s /tmp/ap2g5g_apc ] && continue #跳出当前循环
			site_survey=""
			rtwlt_sta_ssid="$(echo "$i_b" | cut -d $ap_fenge -f4)"
			rtwlt_sta_bssid="$(echo "$i_b" | cut -d $ap_fenge -f6 | tr 'A-Z' 'a-z')"
			[ ! -z "$rtwlt_sta_ssid" ] && site_survey="$(echo "$i_a" | grep -Eo '[0-9]*[\ ]*'"$rtwlt_sta_ssid".* | tr 'A-Z' 'a-z' | sed -n '1p')"
			[ ! -z "$rtwlt_sta_bssid" ] && site_survey="$(echo "$i_a" | tr 'A-Z' 'a-z' | sed -n "/$rtwlt_sta_bssid/p" | sed -n '1p')"
			[ ! -z "$rtwlt_sta_ssid" ] && [ ! -z "$rtwlt_sta_bssid" ] && site_survey="$(echo "$i_a" | grep -Eo '[0-9]*[\ ]*'"$rtwlt_sta_ssid".* | tr 'A-Z' 'a-z' | sed -n "/$rtwlt_sta_bssid/p" | sed -n '1p')"
			[ ! -z "$site_survey" ] && echo "$site_survey" > /tmp/ap2g5g_site_survey
			[ ! -z "$site_survey" ] && echo "$i_b" > /tmp/ap2g5g_apc
			[ ! -z "$site_survey" ] && continue #跳出当前循环
			fi
			done
			fi
			done
			apc="$(cat /tmp/ap2g5g_apc)"
			site_survey="$(cat /tmp/ap2g5g_site_survey)"
		fi
		radio=$(echo "$apc" | cut -d $ap_fenge -f1)
		rtwlt_mode_x="$(echo "$apc" | cut -d $ap_fenge -f2)"
		rtwlt_sta_wisp="$(echo "$apc" | cut -d $ap_fenge -f3)"
		rtwlt_sta_ssid="$(echo "$apc" | cut -d $ap_fenge -f4)"
		rtwlt_sta_wpa_psk="$(echo "$apc" | cut -d $ap_fenge -f5)"
		rtwlt_sta_bssid="$(echo "$apc" | cut -d $ap_fenge -f6 | tr 'A-Z' 'a-z')"
		if [ "$radio" = "2" ] ; then
		[ ! -z "$rtwlt_sta_ssid" ] && site_survey="$(echo "$wds_aplist2g" | grep -Eo '[0-9]*[\ ]*'"$rtwlt_sta_ssid".* | tr 'A-Z' 'a-z' | sed -n '1p')"
		[ ! -z "$rtwlt_sta_bssid" ] && site_survey="$(echo "$wds_aplist2g" | tr 'A-Z' 'a-z' | sed -n "/$rtwlt_sta_bssid/p" | sed -n '1p')"
		[ ! -z "$rtwlt_sta_ssid" ] && [ ! -z "$rtwlt_sta_bssid" ] && site_survey="$(echo "$wds_aplist2g" | grep -Eo '[0-9]*[\ ]*'"$rtwlt_sta_ssid".* | tr 'A-Z' 'a-z' | sed -n "/$rtwlt_sta_bssid/p" | sed -n '1p')"
		fi
		if [ "$radio" = "5" ] ; then
		[ ! -z "$rtwlt_sta_ssid" ] && site_survey="$(echo "$wds_aplist5g" | grep -Eo '[0-9]*[\ ]*'"$rtwlt_sta_ssid".* | tr 'A-Z' 'a-z' | sed -n '1p')"
		[ ! -z "$rtwlt_sta_bssid" ] && site_survey="$(echo "$wds_aplist5g" | tr 'A-Z' 'a-z' | sed -n "/$rtwlt_sta_bssid/p" | sed -n '1p')"
		[ ! -z "$rtwlt_sta_ssid" ] && [ ! -z "$rtwlt_sta_bssid" ] && site_survey="$(echo "$wds_aplist5g" | grep -Eo '[0-9]*[\ ]*'"$rtwlt_sta_ssid".* | tr 'A-Z' 'a-z' | sed -n "/$rtwlt_sta_bssid/p" | sed -n '1p')"
		fi
		if [ ! -z "$site_survey" ] ; then
		ap_ch_ac="$(awk -v a="$aplist_n" -v b="Ch" 'BEGIN{print index(a,b)}')"
		ap_ch_b="$(echo "$aplist_n" | grep -Eo "Ch *" | sed -n '1p')"
		ap_ch_bc="$(echo """$ap_ch_b" | wc -c)"
		ap_ch_bc=`expr $ap_ch_bc - 1`
		ap_ch_c=`expr $ap_ch_ac + $ap_ch_bc - 1`
		ap_ssid_ac="$(awk -v a="$aplist_n" -v b="SSID" 'BEGIN{print index(a,b)}')"
		ap_ssid_b="$(echo "$aplist_n" | grep -Eo "SSID *" | sed -n '1p')"
		ap_ssid_bc="$(echo """$ap_ssid_b" | wc -c)"
		ap_ssid_bc=`expr $ap_ssid_bc - 1`
		ap_ssid_c=`expr $ap_ssid_ac + $ap_ssid_bc - 1`
		ap_bssid_ac="$(awk -v a="$aplist_n" -v b="BSSID" 'BEGIN{print index(a,b)}')"
		ap_bssid_b="$(echo "$aplist_n" | grep -Eo "BSSID *" | sed -n '1p')"
		ap_bssid_bc="$(echo """$ap_bssid_b" | wc -c)"
		ap_bssid_bc=`expr $ap_bssid_bc - 1`
		ap_bssid_c=`expr $ap_bssid_ac + $ap_bssid_bc - 1`
		ap_security_ac="$(awk -v a="$aplist_n" -v b="Security" 'BEGIN{print index(a,b)}')"
		ap_security_b="$(echo "$aplist_n" | grep -Eo "Security *" | sed -n '1p')"
		ap_security_bc="$(echo """$ap_security_b" | wc -c)"
		ap_security_bc=`expr $ap_security_bc - 1`
		ap_security_c=`expr $ap_security_ac + $ap_security_bc - 1`
		ap_signal_ac="$(awk -v a="$aplist_n" -v b="Signal" 'BEGIN{print index(a,b)}')"
		ap_signal_b="Signal(%)"
		ap_signal_bc="$(echo """$ap_signal_b" | wc -c)"
		ap_signal_bc=`expr $ap_signal_bc - 1`
		ap_signal_c=`expr $ap_signal_ac + $ap_signal_bc - 1`
		ap_wmode_ac="$(awk -v a="$aplist_n" -v b="W-Mode" 'BEGIN{print index(a,b)}')"
		ap_wmode_b="$(echo "$aplist_n" | grep -Eo "W-Mode *" | sed -n '1p')"
		ap_wmode_bc="$(echo """$ap_wmode_b" | wc -c)"
		ap_wmode_bc=`expr $ap_wmode_bc - 1`
		ap_wmode_c=`expr $ap_wmode_ac + $ap_wmode_bc - 1`
		Ch="$(echo "$site_survey" | cut -b "$ap_ch_ac-$ap_ch_c")"
		SSID="$(echo "$site_survey" | cut -b "$ap_ssid_ac-$ap_ssid_c")"
		BSSID="$(echo "$site_survey" | cut -b "$ap_bssid_ac-$ap_bssid_c")"
		Security="$(echo "$site_survey" | cut -b "$ap_security_ac-$ap_security_c")"
		Signal="$(echo "$site_survey" | cut -b "$ap_signal_ac-$ap_signal_c")"
		WMode="$(echo "$site_survey" | cut -b "$ap_wmode_ac-$ap_wmode_c")"
		if [ "$radio" = "2" ] ; then
			radio_x="2g"
			ap="$(iwconfig $radio2_apcli | grep ESSID: | grep "$rtwlt_sta_ssid" | wc -l)"
			if [ "$ap" = "0" ] ; then
			ap="$(iwconfig $radio2_apcli |sed -n '/'$radio2_apcli'/,/Rate/{/'$radio2_apcli'/n;/Rate/b;p}' | tr 'A-Z' 'a-z' | grep $rtwlt_sta_bssid | wc -l)"
			fi
		else
			radio_x="5g"
			ap="$(iwconfig $radio5_apcli | grep ESSID: | grep "$rtwlt_sta_ssid" | wc -l)"
			if [ "$ap" = "0" ] ; then
			ap="$(iwconfig $radio5_apcli |sed -n '/'$radio5_apcli'/,/Rate/{/'$radio5_apcli'/n;/Rate/b;p}' | tr 'A-Z' 'a-z' | grep $rtwlt_sta_bssid | wc -l)"
			fi
		fi
		if [ "$ap" = "1" ] ; then
		ap3=1
		fi
		if [ "$ap" = "0" ] ; then
		ap3=0
		fi
		ap_black=`nvram get ap_black`
		if [ "$ap_black" = "1" ] ; then
			apblacktxt=$(grep "【SSID:$rtwlt_sta_bssid" /tmp/apblack.txt)
			if [ ! -z $apblacktxt ] ; then
			logger -t "【连接 AP】" "当前是黑名单 $rtwlt_sta_ssid, 跳过黑名单继续搜寻"
			ap3=1
			else
			apblacktxt=$(grep "【SSID:$rtwlt_sta_ssid" /tmp/apblack.txt)
			if [ ! -z $apblacktxt ] ; then
			logger -t "【连接 AP】" "当前是黑名单 $rtwlt_sta_ssid, 跳过黑名单继续搜寻"
			ap3=1
			fi
			fi
		fi
		if [ "$ap3" != "1" ] ; then
		if [ "$radio" = "2" ] ; then
		nvram set rt_channel=$Ch
		iwpriv $radio2_apcli set Channel=$Ch
		else
		nvram set wl_channel=$Ch
		iwpriv $radio5_apcli set Channel=$Ch
		fi
		if [ ! -z "$(echo $Security | grep none)" ] ; then
		rtwlt_sta_auth_mode="open"
		rtwlt_sta_wpa_mode="0"
		fi
		if [ ! -z "$(echo $Security | grep open)" ] ; then
		rtwlt_sta_auth_mode="open"
		rtwlt_sta_wpa_mode="0"
		fi
		if [ ! -z "$(echo $Security | grep 1psk)" ] ; then
		rtwlt_sta_auth_mode="psk"
		rtwlt_sta_wpa_mode="1"
		fi
		if [ ! -z "$(echo $Security | grep wpapsk)" ] ; then
		rtwlt_sta_auth_mode="psk"
		rtwlt_sta_wpa_mode="1"
		fi
		if [ ! -z "$(echo $Security | grep 2psk)" ] ; then
		rtwlt_sta_auth_mode="psk"
		rtwlt_sta_wpa_mode="2"
		fi
		if [ ! -z "$(echo $Security | grep tkip)" ] ; then
		rtwlt_sta_crypto="tkip"
		fi
		if [ ! -z "$(echo $Security | grep aes)" ] ; then
		rtwlt_sta_crypto="aes"
		fi
		if [ "$radio" = "2" ] ; then
		nvram set rt_mode_x="$rtwlt_mode_x"
		nvram set rt_sta_wisp="$rtwlt_sta_wisp"
		nvram set rt_sta_ssid="$rtwlt_sta_ssid"
		nvram set rt_sta_auth_mode="$rtwlt_sta_auth_mode"
		nvram set rt_sta_wpa_mode="$rtwlt_sta_wpa_mode"
		nvram set rt_sta_crypto="$rtwlt_sta_crypto"
		nvram set rt_sta_wpa_psk="$rtwlt_sta_wpa_psk"
		#强制20MHZ
		nvram set rt_HT_BW=0
		nvram commit ; radio2_restart
		else
		nvram set wl_mode_x="$rtwlt_mode_x"
		nvram set wl_sta_wisp="$rtwlt_sta_wisp"
		nvram set wl_sta_ssid="$rtwlt_sta_ssid"
		nvram set wl_sta_auth_mode="$rtwlt_sta_auth_mode"
		nvram set wl_sta_wpa_mode="$rtwlt_sta_wpa_mode"
		nvram set wl_sta_crypto="$rtwlt_sta_crypto"
		nvram set wl_sta_wpa_psk="$rtwlt_sta_wpa_psk"
		nvram commit ; radio5_restart
		fi
		logger -t "【连接 AP】" "$rtwlt_mode_x $rtwlt_sta_wisp $rtwlt_sta_ssid $rtwlt_sta_auth_mode $rtwlt_sta_wpa_mode $rtwlt_sta_crypto $rtwlt_sta_wpa_psk"
		sleep 7
		logger -t "【连接 AP】" "【Ch:""$(echo $Ch)""】【SSID:""$(echo $SSID)""】【BSSID:""$(echo $BSSID)""】"
		logger -t "【连接 AP】" "【Security:""$(echo $Security)""】【Signal(%):""$(echo $Signal)""】【WMode:""$(echo $WMode)""】"
		if [ "$apblack" = "1" ] ; then
			sleep 7
			ping_text=`ping -4 223.5.5.5 -c 1 -w 4 -q`
			ping_time=`echo $ping_text | awk -F '/' '{print $4}'| awk -F '.' '{print $1}'`
			ping_loss=`echo $ping_text | awk -F ', ' '{print $3}' | awk '{print $1}'`
			if [ ! -z "$ping_time" ] ; then
				logger -t "【连接 AP】" "$ap 已连接上 $rtwlt_sta_ssid, 成功联网"
			else
				logger -t "【连接 AP】" "$ap 已连接上 $rtwlt_sta_ssid"
				apblacktxt="$ap AP不联网列入黑名单:【Ch:""$(echo $Ch)""】【SSID:""$(echo $SSID)""】【BSSID:""$(echo $BSSID)""】【Security:""$(echo $Security)""】【Signal(%):""$(echo $Signal)""】【WMode:""$(echo $WMode)""】"
				logger -t "【连接 AP】" "$apblacktxt"
				echo $apblacktxt >> /tmp/apblack.txt
			fi
		fi
		rm -f /tmp/apc.lock
		/etc/storage/ap_script.sh &
		exit
		fi
		fi
		fi
		if [ "$ap_rule" = "1" ] ; then
			rm -f /tmp/apc.lock
			exit
		fi
	done < /tmp/ap2g5g
fi
fi
rm -f /tmp/apc.lock

}

case "$1" in
1)
  button_1
  ;;
2)
  button_2
  ;;
3)
  button_3
  ;;
timesystem)
  timesystem
  ;;
serverchan)
  serverchan
  ;;
serverchan_clean)
  serverchan_clean
  ;;
relnmp)
  relnmp
  ;;
mkfs)
  mkfs
  ;;
reszUID)
  reszUID
  ;;
getAPSite2g)
  getAPSite "2g"
  ;;
getAPSite5g)
  getAPSite "5g"
  ;;
connAPSite)
  connAPSite
  ;;
connAPSite_scan)
  connAPSite "scan"
  ;;
esac

