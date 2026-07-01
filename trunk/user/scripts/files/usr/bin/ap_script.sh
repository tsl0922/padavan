#!/bin/sh
#/etc/storage/ap_script.sh
# 修复/tmp/apc.lock一直存在，无法启动检查进程
# 避免多次启动检查进程/etc/storage/sh_ezscript.sh
# 每隔15秒检查AP中断/连接(之前是63秒)

# AP中继连接守护功能。【0】 Internet互联网断线后自动搜寻；【1】 当中继信号断开时启动自动搜寻。
nvram set ap_check=0

# AP连接成功条件，【0】 连上AP即可，不检查是否联网；【1】 连上AP并连上Internet互联网。
nvram set ap_inet_check=0

# 【0】 自动搜寻AP，成功连接后停止搜寻；大于等于【10】时则每隔【N】秒搜寻(无线网络会瞬断一下)，直到连上最优先信号。
nvram set ap_time_check=0

# 如搜寻的AP不联网则列入黑名单/tmp/apblack.txt 功能 【0】关闭；【1】启动
# 控制台输入【echo "" > /tmp/apblack.txt】可以清空黑名单
nvram set ap_black=0

# 自定义分隔符号，默认为【@】，注意:下面配置一同修改
nvram set ap_fenge='@'

# 搜寻AP排序设置【0】从第一行开始（第一行的是最优先信号）；【1】不分顺序自动连接最强信号
nvram set ap_rule=0

# 【自动切换中继信号】功能 填写配置参数启动
cat >/tmp/ap2g5g.txt <<-\EOF
# 中继AP配置填写说明：
# 各参数用【@】分割开，如果有多个信号可回车换行继续填写即可(从第一行的参数开始搜寻)【第一行的是最优先信号】
# 搜寻时无线网络会瞬断一下
# 参数说明：
# ①2.4Ghz或5Ghz："2"=【2.4Ghz】"5"=【5Ghz】
# ②无线AP工作模式："3"=【AP-Client（AP被禁用）】"4"=【AP-Client + AP】（不支持WDS）
# ③无线AP-Client角色： "0"=【LAN bridge】"1"=【WAN (Wireless ISP)】
# ④中继AP 的 SSID："ASUS"
# ⑤中继AP 密码："1234567890"
# ⑥中继AP 的 MAC地址："20:76:90:20:B0:F0"【SSID有中文时需填写，不限大小写】
# 下面是信号填写例子：（删除前面的#可生效）
#2@4@1@ASUS@1234567890
#2@4@1@ASUS_中文@1234567890@34:bd:f9:1f:d2:b1
#2@4@1@ASUS3@1234567890@34:bd:f9:1f:d2:b0





EOF

rt_mode_x=`nvram get rt_mode_x`
wl_mode_x=`nvram get wl_mode_x`
if [ $rt_mode_x -gt 2 ] || [ $wl_mode_x -gt 2 ]; then
cat /tmp/ap2g5g.txt | grep -v '^#'  | grep -v "^$" > /tmp/ap2g5g
fi
killall sh_apauto.sh
if [ -f /tmp/ap2g5g ] ; then
cat >/tmp/sh_apauto.sh <<-\EOF
#!/bin/sh
[ "$1" = "crontabs" ] && sleep 15
logger -t "【AP 中继】" "连接守护启动"
while [ -s /tmp/ap2g5g ]; do
# [2023-1-11] check if ap_script.sh / sh_ezscript.sh exists
ap_script_num=$(ps | grep -E "[a]p_script.sh" | wc -l)
sh_ezscript_num=$(ps | grep -E "[s]h_ezscript.sh" | wc -l)
if [[ $ap_script_num == "0" && $ap_script_num == "0" ]]; then
  #logger -t "【移除apc.lock】" "没有找到中继检查进程"
  rm -f /tmp/apc.lock
fi

radio2_apcli=`nvram get radio2_apcli`
[ -z $radio2_apcli ] && radio2_apcli="apcli0"
radio5_apcli=`nvram get radio5_apcli`
[ -z $radio5_apcli ] && radio5_apcli="apclii0"
  ap_check=`nvram get ap_check`
  if [[ "$ap_check" == 1 ]] && [ ! -f /tmp/apc.lock ] ; then
  #【1】 当中继信号断开时启动自动搜寻
  a2=`ifconfig | grep $radio2_apcli`
  sleep 1
  a5=`ifconfig | grep $radio5_apcli`
  sleep 1
  [ "$a2" = "" -a "$a5" = "" ] && ap=1 || ap=0
  if [ "$ap" = "1" ] ; then
    logger -t "【AP 中继】" "连接中断，启动自动搜寻"
    [ $sh_ezscript_num == "0" ] && /etc/storage/sh_ezscript.sh connAPSite_scan &
    sleep 10
  fi
  fi
  ap_time_check="$(nvram get ap_time_check)"
  if [ "$ap_time_check" -ge 9 ] && [ ! -f /tmp/apc.lock ] ; then
    ap_fenge="$(nvram get ap_fenge)"
    rtwlt_sta_ssid_1=$(echo $(grep -v '^#' /tmp/ap2g5g | grep -v "^$" | head -1) | cut -d $ap_fenge -f4)
    rtwlt_sta_bssid_1=$(echo $(grep -v '^#' /tmp/ap2g5g | grep -v "^$" | head -1) | cut -d $ap_fenge -f6 | tr 'A-Z' 'a-z')
    [ "$(echo $(grep -v '^#' /tmp/ap2g5g | grep -v "^$" | head -1) | cut -d $ap_fenge -f1)" = "5" ] && radio2_apcli="$radio5_apcli"
    rtwlt_sta_ssid="$(iwconfig $radio2_apcli | grep ESSID: | awk -F'"' '/ESSID/ {print $2}')"
    sleep 1
    rtwlt_sta_bssid="$(iwconfig $radio2_apcli |sed -n '/'$radio2_apcli'/,/Rate/{/'$radio2_apcli'/n;/Rate/b;p}' | tr 'A-Z' 'a-z'  | awk -F'point:' '/point/ {print $2}')"
    sleep 1
    rtwlt_sta_bssid="$(echo $rtwlt_sta_bssid)"
    [ ! -z "$rtwlt_sta_ssid_1" ] && [ ! -z "$rtwlt_sta_ssid" ] && [ "$rtwlt_sta_ssid_1" == "$rtwlt_sta_ssid" ] && ap_time_check=0
    [ ! -z "$rtwlt_sta_bssid_1" ] && [ ! -z "$rtwlt_sta_bssid" ] && [ "$rtwlt_sta_bssid_1" == "$rtwlt_sta_bssid" ] && ap_time_check=0
    if [ "$ap_time_check" -ge 9 ] && [ ! -f /tmp/apc.lock ] ; then
    
    logger -t "【连接 AP】" "$ap_time_check 秒后,自动搜寻 ap ,直到连上最优先信号 $rtwlt_sta_ssid_1 "
    sleep $ap_time_check
    [ $sh_ezscript_num == "0" ] && /etc/storage/sh_ezscript.sh connAPSite_scan &

    sleep 10
    fi
  fi
  if [[ "$ap_check" == 0 ]] && [ ! -f /tmp/apc.lock ] ; then
    #【2】 Internet互联网断线后自动搜寻
    ping_text=`ping -4 223.5.5.5 -c 1 -w 4 -q`
    ping_time=`echo $ping_text | awk -F '/' '{print $4}'| awk -F '.' '{print $1}'`
    ping_loss=`echo $ping_text | awk -F ', ' '{print $3}' | awk '{print $1}'`
    if [ ! -z "$ping_time" ] ; then
    echo "online"
    else
    echo "Internet互联网断线后自动搜寻"
    [ $sh_ezscript_num == "0" ] && /etc/storage/sh_ezscript.sh connAPSite_scan &
    sleep 10
    fi
  fi
  sleep 15
  cat /tmp/ap2g5g.txt | grep -v '^#'  | grep -v "^$" > /tmp/ap2g5g
done
EOF
  chmod 777 "/tmp/sh_apauto.sh"
  [ -z "$(ps -w | grep sh_apauto.sh | grep -v grep)" ] && /tmp/sh_apauto.sh $1 &
fi

