<!DOCTYPE html>
<html>
<head>
<title><#Web_Title#> - <#menu5_21#></title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">

<link rel="shortcut icon" href="images/favicon.ico">
<link rel="icon" href="images/favicon.png">
<link rel="stylesheet" type="text/css" href="/bootstrap/css/bootstrap.min.css">
<link rel="stylesheet" type="text/css" href="/bootstrap/css/main.css">
<link rel="stylesheet" type="text/css" href="/bootstrap/css/engage.itoggle.css">

<script type="text/javascript" src="/jquery.js"></script>
<script type="text/javascript" src="/bootstrap/js/bootstrap.min.js"></script>
<script type="text/javascript" src="/bootstrap/js/engage.itoggle.min.js"></script>
<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="/general.js"></script>
<script type="text/javascript" src="/itoggle.js"></script>
<script type="text/javascript" src="/help_b.js"></script>
<script type="text/javascript" src="/popup.js"></script>
<script>
var $j = jQuery.noConflict();
<% smartdns_status(); %>

$j(document).ready(function(){
	init_itoggle('sdns_enable');
	init_itoggle('sdns_tcp_server');
	init_itoggle('sdns_ipv6_server');
	init_itoggle('sdnse_ipv6_server');
	init_itoggle('sdns_ip_change');
	init_itoggle('sdns_dualstack_ip_allow_force_AAAA');
	init_itoggle('sdns_cache_persist');
	init_itoggle('sdns_prefetch_domain');
	init_itoggle('sdns_force_aaaa_soa');
	init_itoggle('sdns_force_qtype_soa');
	init_itoggle('sdns_exp');
	init_itoggle('sdnse_enable');
	init_itoggle('sdnse_address');
	init_itoggle('sdns_address');
	init_itoggle('sdnse_tcp');
	init_itoggle('sdnse_as');
	init_itoggle('sdns_as');
	init_itoggle('sdnse_speed');
	init_itoggle('sdns_speed');
	init_itoggle('sdns_speed_mode');
	init_itoggle('sdnse_ns');
	init_itoggle('sdns_ns');
	init_itoggle('sdnse_ipc');
	init_itoggle('sdnse_ipset');
	init_itoggle('sdns_ipset');
	init_itoggle('sdns_ipset_timeout');
	init_itoggle('sdnse_cache');
	init_itoggle('sdns_coredump');
	init_itoggle('sdns_black');
	init_itoggle('sdns_white');
	init_itoggle('sdns_adblock');
	init_itoggle('sdns_adblock_url');
	init_itoggle('sdnss_enable_x_0');
		$j("#tab_sm_cfg, #tab_sm_exp, #tab_sm_sec, #tab_sm_dns, #tab_sm_cou").click(function(){
		var newHash = $j(this).attr('href').toLowerCase();
		showTab(newHash);
		return false;
	});
});

var m_list = [<% get_nvram_list("SmartdnsConf", "SdnsList"); %>];
var mlist_ifield = 6;
if(m_list.length > 0){
	var m_list_ifield = m_list[0].length;
	for (var i = 0; i < m_list.length; i++) {
		m_list[i][mlist_ifield] = i;
	}
}
function initial(){
	show_banner(2);
	show_menu(5,16);
	show_footer();
	showTab(getHash());
	showMRULESList();
	showmenu();
	fill_status(smartdns_status());
}

function applyRule(){
	//if(validForm()){
		showLoading();
		document.form.action_mode.value = " Restart ";
		document.form.current_page.value = "Advanced_smartdns.asp";
		document.form.next_page.value = "";
		document.form.submit();
	//}
}
var arrHashes = ["cfg", "exp", "sec", "dns", "cou"];
function showTab(curHash){
	var obj = $('tab_sm_'+curHash.slice(1));
	if (obj == null || obj.style.display == 'none')
		curHash = '#cfg';
	for(var i = 0; i < arrHashes.length; i++){
		if(curHash == ('#'+arrHashes[i])){
			$j('#tab_sm_'+arrHashes[i]).parents('li').addClass('active');
			$j('#wnd_sm_'+arrHashes[i]).show();
		}else{
			$j('#wnd_sm_'+arrHashes[i]).hide();
			$j('#tab_sm_'+arrHashes[i]).parents('li').removeClass('active');
		}
	}
	window.location.hash = curHash;
}

function getHash(){
	var curHash = window.location.hash.toLowerCase();
	for(var i = 0; i < arrHashes.length; i++){
		if(curHash == ('#'+arrHashes[i]))
			return curHash;
	}
	return ('#'+arrHashes[0]);
}

function fill_status(status_code){
	var stext = "Unknown";
	if (status_code == 0)
		stext = "<#Stopped#>";
	else if (status_code == 1)
		stext = "<#Running#>";
	$("smartdns_status").innerHTML = '<span class="label label-' + (status_code != 0 ? 'success' : 'warning') + '">' + stext + '</span>';
}
function markGroupRULES(o, c, b) {
	document.form.group_id.value = "SdnsList";
	if(b == " Add "){
		if (document.form.sdnss_staticnum_x_0.value >= c){
			alert("<#JS_itemlimit1#> " + c + " <#JS_itemlimit2#>");
			return false;
		}else if (document.form.sdnss_ip_x_0.value==""){
			alert("<#JS_fieldblank#>");
			document.form.sdnss_ip_x_0.focus();
			document.form.sdnss_ip_x_0.select();
			return false;
		}else if(document.form.sdnss_name_x_0.value==""){
			alert("<#JS_fieldblank#>");
			document.form.sdnss_name_0.focus();
			document.form.sdnss_name_0.select();
			return false;
		}else{
			for(i=0; i<m_list.length; i++){
				if(document.form.sdnss_ip_x_0.value==m_list[i][2]) {
				if(document.form.sdnss_type_x_0.value==m_list[i][4]) {
					alert('<#JS_duplicate#>' + ' (' + m_list[i][2] + ')' );
					document.form.sdnss_ip_x_0.focus();
					document.form.sdnss_ip_x_0.select();
					return false;
					}
				}
				if(document.form.sdnss_name_x_0.value.value==m_list[i][1]) {
					alert('<#JS_duplicate#>' + ' (' + m_list[i][1] + ')' );
					document.form.sdnss_name_0.focus();
					document.form.sdnss_name_0.select();
					return false;
				}
			}
		}
	}
	pageChanged = 0;
	document.form.action_mode.value = b;
	document.form.current_page.value = "Advanced_smartdns.asp#dns";
	return true;
}
function showmenu(){
showhide_div('adglink', found_app_adguardhome());
}
function showMRULESList(){
	var code = '<table width="100%" cellspacing="0" cellpadding="3" class="table table-list">';
	if(m_list.length == 0)
		code +='<tr><td colspan="3" style="text-align: center;"><div class="alert alert-info"><#IPConnection_VSList_Norule#></div></td></tr>';
	else{
	    for(var i = 0; i < m_list.length; i++){
		if(m_list[i][0] == 0)
		adbybyrulesroad="已禁用";
		else{
		adbybyrulesroad="已启用";
		}
		if(m_list[i][5] == 0)
		ipc="禁用";
		else if(m_list[i][5] == "whitelist"){
		ipc="白名单";
		}else{
		ipc="黑名单";
		}
		code +='<tr id="rowrl' + i + '">';
		code +='<td width="10%">&nbsp;' + adbybyrulesroad + '</td>';
		code +='<td width="20%">&nbsp;' + m_list[i][1] + '</td>';
		code +='<td width="25%" class="spanb">' + m_list[i][2] + '</td>';
		code +='<td width="10%">&nbsp;' + m_list[i][3] + '</td>';
		code +='<td width="10%">&nbsp;' + m_list[i][4] + '</td>';
		code +='<td width="15%">&nbsp;' + ipc + '</td>';
		code +='<center><td width="5%" style="text-align: center;"><input type="checkbox" name="SdnsList_s" value="' + m_list[i][mlist_ifield] + '" onClick="changeBgColorrl(this,' + i + ');" id="check' + m_list[i][mlist_ifield] + '"></td></center>';
		code +='</tr>';
	    }
		code += '<tr>';
		code += '<td colspan="6">&nbsp;</td>'
		code += '<td><button class="btn btn-danger" type="submit" onclick="markGroupRULES(this, 64, \' Del \');" name="SdnsList"><i class="icon icon-minus icon-white"></i></button></td>';
		code += '</tr>'
	}
	code +='</table>';
	$("MRULESList_Block").innerHTML = code;
}
</script>

<style>
.nav-tabs > li > a {
    padding-right: 6px;
    padding-left: 6px;
}
.spanb{
    overflow:hidden;
    text-overflow:ellipsis;
    white-space:nowrap;
}
</style>
</head>

<body onload="initial();" onunLoad="return unload_body();">

<div class="wrapper">
    <div class="container-fluid" style="padding-right: 0px">
        <div class="row-fluid">
            <div class="span3"><center><div id="logo"></div></center></div>
            <div class="span9" >
                <div id="TopBanner"></div>
            </div>
        </div>
    </div>

    <div id="Loading" class="popup_bg"></div>

    <iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
    <form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
	
    <input type="hidden" name="current_page" value="Advanced_smartdns.asp">
    <input type="hidden" name="next_page" value="">
    <input type="hidden" name="next_host" value="">
    <input type="hidden" name="sid_list" value="SmartdnsConf;">
    <input type="hidden" name="group_id" value="SdnsList">
    <input type="hidden" name="action_mode" value="">
    <input type="hidden" name="action_script" value="">
	<input type="hidden" name="sdnss_staticnum_x_0" value="<% nvram_get_x("SdnsList", "sdnss_staticnum_x"); %>" readonly="1" />

    <div class="container-fluid">
        <div class="row-fluid">
            <div class="span3">
                <!--Sidebar content-->
                <!--=====Beginning of Main Menu=====-->
                <div class="well sidebar-nav side_nav" style="padding: 0px;">
                    <ul id="mainMenu" class="clearfix"></ul>
                    <ul class="clearfix">
                        <li>
                            <div id="subMenu" class="accordion"></div>
                        </li>
                    </ul>
                </div>
            </div>

            <div class="span9">
                <!--Body content-->
                <div class="row-fluid">
                    <div class="span12">
                        <div class="box well grad_colour_dark_blue">
                            <h2 class="box_head round_top"><#menu5_21#> - <#menu5_24#></h2>
                            <div class="round_bottom">
		    <div>
                            <ul class="nav nav-tabs" style="margin-bottom: 10px;">
                                <li class="active">
                                    <a href="Advanced_smartdns.asp"><#menu5_24#></a>
                                </li>
								 <li id="adglink" style="display:none">
                                    <a href="Advanced_adguardhome.asp"><#menu5_28#></a>
                                </li>
                            </ul>
                        </div>
						<div>
                            <ul class="nav nav-tabs" style="margin-bottom: 10px;">
                                <li class="active">
                                    <a id="tab_sm_cfg" href="#cfg"><#SmartDNS_1#></a>
                                </li>
								<li>
                                    <a id="tab_sm_exp" href="#exp"><#SmartDNS_2#></a>
                                </li>
								<li>
                                    <a id="tab_sm_sec" href="#sec"><#SmartDNS_3#></a>
								</li>
								<li>
                                    <a id="tab_sm_dns" href="#dns"><#SmartDNS_4#></a>
                                </li>
                                <li>
                                    <a id="tab_sm_cou" href="#cou"><#SmartDNS_5#></a>
                                </li>
                            </ul>
                        </div>
                                <div class="row-fluid">
                                    <div id="tabMenu" class="submenuBlock"></div>
									<div class="alert alert-info" style="margin: 10px;"><input type="button" class="btn btn-success" value="SmartDNS官网" onclick="window.open('https://github.com/pymumu/smartdns')" size="0"><br />
									</br><#SmartDNS_6#>
									</div>
                                </div>
                                    <div id="wnd_sm_cfg">
                                        <table width="100%" cellpadding="4" cellspacing="0" class="table">
                                        <tr> <th width="50%"><#SmartDNS_7#></th>
                                            <td id="smartdns_status" colspan="2"></td>
                                        </tr>
                                        <tr> <th width="50%"><#SmartDNS_8#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_enable_on_of">
                                                    <input type="checkbox" id="sdns_enable_fake" <% nvram_match_x("", "sdns_enable", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_enable", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_enable" id="sdns_enable_1" <% nvram_match_x("", "sdns_enable", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_enable" id="sdns_enable_0" <% nvram_match_x("", "sdns_enable", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>

                                        <tr> <th width="50%"><#SmartDNS1#></th>
                                            <td>
                                                <input type="text" maxlength="15" class="input" size="15" name="sdns_name" style="width: 200px" value="<% nvram_get_x("","sdns_name"); %>" />
                                            </td>
                                        </tr>

                                        <tr> <th width="50%"><#SmartDNS2#></th>
                                            <td>
                                                <input type="text" maxlength="5" class="input" size="15" name="sdns_port" style="width: 200px" value="<% nvram_get_x("", "sdns_port"); %>">
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS3#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_tcp_server_on_of">
                                                    <input type="checkbox" id="sdns_tcp_server_fake" <% nvram_match_x("", "sdns_tcp_server", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_tcp_server", "0", "value=0"); %>>
                                                </div>
                                                </div><span style="color:#888;"><#SmartDNS3_1#></span></div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_tcp_server" id="sdns_tcp_server_1" <% nvram_match_x("", "sdns_tcp_server", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_tcp_server" id="sdns_tcp_server_0" <% nvram_match_x("", "sdns_tcp_server", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS4#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_ipv6_server_on_of">
                                                    <input type="checkbox" id="sdns_ipv6_server_fake" <% nvram_match_x("", "sdns_ipv6_server", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_ipv6_server", "0", "value=0"); %>>
                                                </div>
                                                </div><span style="color:#888;"><#SmartDNS4_1#></span></div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_ipv6_server" id="sdns_ipv6_server_1" <% nvram_match_x("", "sdns_ipv6_server", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_ipv6_server" id="sdns_ipv6_server_0" <% nvram_match_x("", "sdns_ipv6_server", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS5#></th>
											<td>
												<select name="sdns_redirect" class="input" style="width: 200px">
													<option value="0" <% nvram_match_x("","sdns_redirect", "0","selected"); %>>无</option>
													<option value="1" <% nvram_match_x("","sdns_redirect", "1","selected"); %>>作为dnsmasq的上游服务器</option>
													<option value="2" <% nvram_match_x("","sdns_redirect", "2","selected"); %>>重定向53端口到SmartDNS</option>
												</select>
											</td>
										</tr>
                                        <tr> <th width="50%"><#SmartDNS6#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_cache" style="width: 200px" value="<% nvram_get_x("", "sdns_cache"); %>">
                                            </td>
                                        </tr>
                                        <tr> <th width="50%"><#SmartDNS7#></th>
											<td>
                                                <div class="main_itoggle">
                                                <div id="sdns_cache_persist_on_of">
                                                    <input type="checkbox" id="sdns_cache_persist_fake" <% nvram_match_x("", "sdns_cache_persist", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_cache_persist", "0", "value=0"); %>>
                                                </div>
												</div>
                                                <div><span style="color:#888;">cache-file /tmp/smartdns.cache</span></div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_cache_persist" id="sdns_cache_persist_1" <% nvram_match_x("", "sdns_cache_persist", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_cache_persist" id="sdns_cache_persist_0" <% nvram_match_x("", "sdns_cache_persist", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS8#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_tcp_idle_time" style="width: 200px" value="<% nvram_get_x("", "sdns_tcp_idle_time"); %>">
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS9#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_rr_ttl" style="width: 200px" value="<% nvram_get_x("", "sdns_rr_ttl"); %>">
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS10#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_rr_ttl_min" style="width: 200px" value="<% nvram_get_x("", "sdns_rr_ttl_min"); %>">
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS11#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_rr_ttl_max" style="width: 200px" value="<% nvram_get_x("", "sdns_rr_ttl_max"); %>">
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS12#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_rr_ttl_reply_max" style="width: 200px" value="<% nvram_get_x("", "sdns_rr_ttl_reply_max"); %>">
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS13#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_max_reply_ip_num" style="width: 200px" value="<% nvram_get_x("", "sdns_max_reply_ip_num"); %>">
                                            </td>
                                        </tr>
										</table>
										</div>
                                        <div id="wnd_sm_exp">
                                        <table width="100%" cellpadding="2" cellspacing="0" class="table">
                                        <tr> <th width="50%"><#SmartDNS14#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_speed_on_of">
                                                    <input type="checkbox" id="sdns_speed_fake" <% nvram_match_x("", "sdns_speed", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_speed", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_speed" id="sdns_speed_1" <% nvram_match_x("", "sdns_speed", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_speed" id="sdns_speed_0" <% nvram_match_x("", "sdns_speed", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
                                        <tr> <th width="50%"><#SmartDNS15#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_ipset_on_of">
                                                    <input type="checkbox" id="sdns_ipset_fake" <% nvram_match_x("", "sdns_ipset", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_ipset", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_ipset" id="sdns_ipset_1" <% nvram_match_x("", "sdns_ipset", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_ipset" id="sdns_ipset_0" <% nvram_match_x("", "sdns_ipset", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
                                        <tr> <th width="50%"><#SmartDNS16#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_address_on_of">
                                                    <input type="checkbox" id="sdns_address_fake" <% nvram_match_x("", "sdns_address", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_address", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_address" id="sdns_address_1" <% nvram_match_x("", "sdns_address", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_address" id="sdns_address_0" <% nvram_match_x("", "sdns_address", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr> 
										<tr> <th width="50%"><#SmartDNS17#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_ns_on_of">
                                                    <input type="checkbox" id="sdns_ns_fake" <% nvram_match_x("", "sdns_ns", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_ns", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_ns" id="sdns_ns_1" <% nvram_match_x("", "sdns_ns", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_ns" id="sdns_ns_0" <% nvram_match_x("", "sdns_ns", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
                                        <tr> <th width="50%"><#SmartDNS18#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_as_on_of">
                                                    <input type="checkbox" id="sdns_as_fake" <% nvram_match_x("", "sdns_as", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_as", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_as" id="sdns_as_1" <% nvram_match_x("", "sdns_as", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_as" id="sdns_as_0" <% nvram_match_x("", "sdns_as", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
                                        <tr> <th width="50%"><#SmartDNS19#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_ipset_timeout_on_of">
                                                    <input type="checkbox" id="sdns_ipset_timeout_fake" <% nvram_match_x("", "sdns_ipset_timeout", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_ipset_timeout", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_ipset_timeout" id="sdns_ipset_timeout_1" <% nvram_match_x("", "sdns_ipset_timeout", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_ipset_timeout" id="sdns_ipset_timeout_0" <% nvram_match_x("", "sdns_ipset_timeout", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS20#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_speed_mode" style="width: 200px" value="<% nvram_get_x("", "sdns_speed_mode"); %>">
												<div><span style="color:#888;">例如: none 为禁止 ping,tcp:80,tcp:443</span></div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS21#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_ip_change_on_of">
                                                    <input type="checkbox" id="sdns_ip_change_fake" <% nvram_match_x("", "sdns_ip_change", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_ip_change", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_ip_change" id="sdns_ip_change_1" <% nvram_match_x("", "sdns_ip_change", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_ip_change" id="sdns_ip_change_0" <% nvram_match_x("", "sdns_ip_change", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS2#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="64" name="sdns_ip_change_time" style="width: 120px" value="<% nvram_get_x("", "sdns_ip_change_time"); %>"> 毫秒（0-100）
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS23#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_dualstack_ip_allow_force_AAAA_on_of">
                                                    <input type="checkbox" id="sdns_dualstack_ip_allow_force_AAAA_fake" <% nvram_match_x("", "sdns_dualstack_ip_allow_force_AAAA", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_dualstack_ip_allow_force_AAAA", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_dualstack_ip_allow_force_AAAA" id="sdns_dualstack_ip_allow_force_AAAA_1" <% nvram_match_x("", "sdns_dualstack_ip_allow_force_AAAA", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_dualstack_ip_allow_force_AAAA" id="sdns_dualstack_ip_allow_force_AAAA_0" <% nvram_match_x("", "sdns_dualstack_ip_allow_force_AAAA", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
                                        <tr> <th width="50%"><#SmartDNS24#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_force_aaaa_soa_on_of">
                                                    <input type="checkbox" id="sdns_force_aaaa_soa_fake" <% nvram_match_x("", "sdns_force_aaaa_soa", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_force_aaaa_soa", "0", "value=0"); %>>
                                                </div>
                                                <div><span style="color:#888;"><#SmartDNS24_1#></span></div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_force_aaaa_soa" id="sdns_force_aaaa_soa_1" <% nvram_match_x("", "sdns_force_aaaa_soa", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_force_aaaa_soa" id="sdns_force_aaaa_soa_0" <% nvram_match_x("", "sdns_force_aaaa_soa", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS25#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_force_qtype_soa" style="width: 200px" value="<% nvram_get_x("", "sdns_force_qtype_soa"); %>">
												<div><span style="color:#888;"><#SmartDNS25_1#></span></div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS26#></th>
                                             <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_prefetch_domain_on_of">
                                                    <input type="checkbox" id="sdns_prefetch_domain_fake" <% nvram_match_x("", "sdns_prefetch_domain", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_prefetch_domain", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_prefetch_domain" id="sdns_prefetch_domain_1" <% nvram_match_x("", "sdns_prefetch_domain", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_prefetch_domain" id="sdns_prefetch_domain_0" <% nvram_match_x("", "sdns_prefetch_domain", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS27#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_exp_on_of">
                                                    <input type="checkbox" id="sdns_exp_fake" <% nvram_match_x("", "sdns_exp", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_exp", "0", "value=0"); %>>
                                                </div>
                                                </div><span style="color:#888;"><#SmartDNS27_1#></span></div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_exp" id="sdns_exp_1" <% nvram_match_x("", "sdns_exp", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_exp" id="sdns_exp_0" <% nvram_match_x("", "sdns_exp", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS28#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_exp_ttl" style="width: 200px" value="<% nvram_get_x("", "sdns_exp_ttl"); %>">
												<div><span style="color:#888;"><#SmartDNS28_1#></span></div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS29#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_exp_ttl_max" style="width: 200px" value="<% nvram_get_x("", "sdns_exp_ttl_max"); %>">
												<div><span style="color:#888;"><#SmartDNS28_1#></span></div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS30#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="15" name="sdns_exp_prefetch_time" style="width: 200px" value="<% nvram_get_x("", "sdns_exp_prefetch_time"); %>">
												<div><span style="color:#888;"><#SmartDNS28_1#></span></div>
                                            </td>
                                        </tr>
										</table>
										</div>
										<div id="wnd_sm_sec">
										<table width="100%" cellpadding="2" cellspacing="0" class="table">
										<tr> <th width="50%"><#SmartDNS31#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdnse_enable_on_of">
                                                    <input type="checkbox" id="sdnse_enable_fake" <% nvram_match_x("", "sdnse_enable", "1", "value=1 checked"); %><% nvram_match_x("", "sdnse_enable", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdnse_enable" id="sdnse_enable_1" <% nvram_match_x("", "sdnse_enable", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdnse_enable" id="sdnse_enable_0" <% nvram_match_x("", "sdnse_enable", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS1#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="64" name="sdnse_name" placeholder="default" style="width: 200px" value="<% nvram_get_x("", "sdnse_name"); %>">
												<div><span style="color:#888;">例如: oversea, office, home</span></div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS2#></th>
                                            <td>
                                                <input type="text" maxlength="64" class="input" size="64" name="sdnse_port" style="width: 200px" value="<% nvram_get_x("", "sdnse_port"); %>">
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS3#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdnse_tcp_on_of">
                                                    <input type="checkbox" id="sdnse_tcp_fake" <% nvram_match_x("", "sdnse_tcp", "1", "value=1 checked"); %><% nvram_match_x("", "sdnse_tcp", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdnse_tcp" id="sdnse_tcp_1" <% nvram_match_x("", "sdnse_tcp", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdnse_tcp" id="sdnse_tcp_0" <% nvram_match_x("", "sdnse_tcp", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS14#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdnse_speed_on_of">
                                                    <input type="checkbox" id="sdnse_speed_fake" <% nvram_match_x("", "sdnse_speed", "1", "value=1 checked"); %><% nvram_match_x("", "sdnse_speed", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdnse_speed" id="sdnse_speed_1" <% nvram_match_x("", "sdnse_speed", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdnse_speed" id="sdnse_speed_0" <% nvram_match_x("", "sdnse_speed", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS15#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdnse_ipset_on_of">
                                                    <input type="checkbox" id="sdnse_ipset_fake" <% nvram_match_x("", "sdnse_ipset", "1", "value=1 checked"); %><% nvram_match_x("", "sdnse_ipset", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdnse_ipset" id="sdnse_ipset_1" <% nvram_match_x("", "sdnse_ipset", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdnse_ipset" id="sdnse_ipset_0" <% nvram_match_x("", "sdnse_ipset", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS16#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdnse_address_on_of">
                                                    <input type="checkbox" id="sdnse_address_fake" <% nvram_match_x("", "sdnse_address", "1", "value=1 checked"); %><% nvram_match_x("", "sdnse_address", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdnse_address" id="sdnse_address_1" <% nvram_match_x("", "sdnse_address", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdnse_address" id="sdnse_address_0" <% nvram_match_x("", "sdnse_address", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS17#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdnse_ns_on_of">
                                                    <input type="checkbox" id="sdnse_ns_fake" <% nvram_match_x("", "sdnse_ns", "1", "value=1 checked"); %><% nvram_match_x("", "sdnse_ns", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdnse_ns" id="sdnse_ns_1" <% nvram_match_x("", "sdnse_ns", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdnse_ns" id="sdnse_ns_0" <% nvram_match_x("", "sdnse_ns", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS18#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdnse_as_on_of">
                                                    <input type="checkbox" id="sdnse_as_fake" <% nvram_match_x("", "sdnse_as", "1", "value=1 checked"); %><% nvram_match_x("", "sdnse_as", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdnse_as" id="sdnse_as_1" <% nvram_match_x("", "sdnse_as", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdnse_as" id="sdnse_as_0" <% nvram_match_x("", "sdnse_as", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS24#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdnse_ipv6_server_on_of">
                                                    <input type="checkbox" id="sdnse_ipv6_server_fake" <% nvram_match_x("", "sdnse_ipv6_server", "1", "value=1 checked"); %><% nvram_match_x("", "sdnse_ipv6_server", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdnse_ipv6_server" id="sdnse_ipv6_server_1" <% nvram_match_x("", "sdnse_ipv6_server", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdnse_ipv6_server" id="sdnse_ipv6_server_0" <% nvram_match_x("", "sdnse_ipv6_server", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS32#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdnse_ipc_on_of">
                                                    <input type="checkbox" id="sdnse_ipc_fake" <% nvram_match_x("", "sdnse_ipc", "1", "value=1 checked"); %><% nvram_match_x("", "sdnse_ipc", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdnse_ipc" id="sdnse_ipc_1" <% nvram_match_x("", "sdnse_ipc", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdnse_ipc" id="sdnse_ipc_0" <% nvram_match_x("", "sdnse_ipc", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										<tr> <th width="50%"><#SmartDNS33#></th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdnse_cache_on_of">
                                                    <input type="checkbox" id="sdnse_cache_fake" <% nvram_match_x("", "sdnse_cache", "1", "value=1 checked"); %><% nvram_match_x("", "sdnse_cache", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdnse_cache" id="sdnse_cache_1" <% nvram_match_x("", "sdnse_cache", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdnse_cache" id="sdnse_cache_0" <% nvram_match_x("", "sdnse_cache", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										</table>
										</div>
										<div id="wnd_sm_dns">
										<table width="100%" cellpadding="4" cellspacing="0" class="table">
										<tbody>
                                        <tr> <th width="50%">启用:</th>
										    <td>
                                                <div class="main_itoggle">
                                                <div id="sdnss_enable_x_0_on_of">
                                                    <input type="checkbox" id="sdnss_enable_x_0_fake" <% nvram_match_x("", "sdnss_enable_x_0", "1", "value=1 checked"); %><% nvram_match_x("", "sdnss_enable_x_0", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdnss_enable_x_0" id="sdnss_enable_x_0_1" <% nvram_match_x("", "sdnss_enable_x_0", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdnss_enable_x_0" id="sdnss_enable_x_0_0" <% nvram_match_x("", "sdnss_enable_x_0", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
										</tr>
                                        <tr> <th width="50%">上游名称:</th>
										    <td>
                                                <input type="text" maxlength="255" class="span12" style="width: 200px" size="200" name="sdnss_name_x_0" value="<% nvram_get_x("", "sdnss_name_x_0"); %>" onKeyPress="return is_string(this,event);"/>
                                            </td>
										</tr>
                                        <tr> <th width="50%">上游地址:</th>
										    <td>
                                                <input type="text" maxlength="255" class="span12" style="width: 200px" size="200" name="sdnss_ip_x_0" value="<% nvram_get_x("", "sdnss_ip_x_0"); %>" onKeyPress="return is_string(this,event);"/>
                                            </td>
										</tr>
                                        <tr> <th width="50%">上游端口:</th>
										    <td>
                                                <input type="text" maxlength="255" class="span12" style="width: 200px" size="200" name="sdnss_port_x_0" value="default" onKeyPress="return is_string(this,event);"/>
											</td>
										</tr>
                                        <tr> <th width="50%">上游类型:</th>
										    <td>
                                          	    <select name="sdnss_type_x_0" class="input" style="width: 200px">
													<option value="tcp" <% nvram_match_x("","sdnss_type_x_0", "tcp","selected"); %>>tcp</option>
													<option value="udp" <% nvram_match_x("","sdnss_type_x_0", "udp","selected"); %>>udp</option>
													<option value="tls" <% nvram_match_x("","sdnss_type_x_0", "tls","selected"); %>>tls</option>
													<option value="https" <% nvram_match_x("","sdnss_type_x_0", "https","selected"); %>>https</option>
												</select>
                                            </td>
										</tr>
                                        <tr> <th width="50%">IP过滤:</th>
										    <td>
                                          	    <select name="sdnss_ipc_x_0" class="input" style="width: 200px">
													<option value="0" <% nvram_match_x("","sdnss_ipc_x_0", "0","selected"); %>>禁用</option>
													<option value="whitelist" <% nvram_match_x("","sdnss_ipc_x_0", "whitelist","selected"); %>>白名单</option>
													<option value="blacklist" <% nvram_match_x("","sdnss_ipc_x_0", "blacklist","selected"); %>>黑名单</option>
												</select>
                                            </td>
                                        </tr>
										<tr> <th colspan="2" style="background-color: #E3E3E3;">指定服务器组可用于单独解析gfwlist,如果不需要配合SS解析gfwlist,可以不填</th></tr>
										<tr> <th width="50%">服务器组(留空为不指定):</th>
										    <td>
                                                <input type="text" maxlength="255" class="span12" style="width: 200px" size="200" name="sdnss_named_x_0" value="<% nvram_get_x("", "sdnss_named_x_0"); %>" />
												<div><span style="color:#888;">例如: oversea, office, home</span></div>
											</td>
										</tr>
										<tr> <th width="50%">加入ipset(解析gfwlist要用):</th>
										    <td>
                                                <input type="text" maxlength="255" class="span12" style="width: 200px" size="200" name="sdnss_ipset_x_0" value="<% nvram_get_x("", "sdnss_ipset_x_0"); %>" />
												<div><span style="color:#888;">注意IP直接填,如果是域名</span></div>
												<div><span style="color:#888;">例如:https://dns.google/dns-query</span></div>
											</td>
										</tr>
										<tr> <th width="50%">将服务器从默认分组中排除:</th>
										    <td>
                                          	    <select name="sdnss_non_x_0" class="input" style="width: 200px">
													<option value="0" <% nvram_match_x("","sdnss_non_x_0", "0","selected"); %>>否</option>
													<option value="1" <% nvram_match_x("","sdnss_non_x_0", "1","selected"); %>>是</option>
												</select>
                                            </td>
                                        </tr>
										</tbody>
										</table>
										<table width="100%" align="center" cellpadding="0" cellspacing="0" class="table">
                                        <tr>
                                            <td><center><input name="ManualRULESList2" type="submit" class="btn btn-primary" style="width: 100px" onclick="return markGroupRULES(this, 64, ' Add ');" value="保存上游"/></center></td>										
                                        </tr>
										</table>
                                        <table width="100%" align="center" cellpadding="3" cellspacing="0" class="table">
                                        <tr id="row_rules_caption">
                                            <th width="10%">启用 <i class="icon-circle-arrow-down"></i></th>
											<th width="20%">名称 <i class="icon-circle-arrow-down"></i></th>
											<th width="20%">地址 <i class="icon-circle-arrow-down"></i></th>
											<th width="10%">端口 <i class="icon-circle-arrow-down"></i></th>
											<th width="10%">协议 <i class="icon-circle-arrow-down"></i></th>
											<th width="15%">过滤 <i class="icon-circle-arrow-down"></i></th>
                                            <th width="5%"><center><i class="icon-th-list"></i></center></th>
                                        </tr>
                                        <tr id="row_rules_body" >
                                            <td colspan="7" style="border-top: 0 none; padding: 0px;">
                                                <div id="MRULESList_Block"></div>
                                            </td>
                                        </tr>
										</table>
										</div>
										<div id="wnd_sm_cou">
										<table width="100%" cellpadding="2" cellspacing="0" class="table">
										<tr>
											<td colspan="7" >
												<i class="icon-hand-right"></i> <a href="javascript:spoiler_toggle('script9')"><span>域名地址:</span></a>
												<div id="script9" style="display:none;">
													<textarea rows="8" wrap="off" spellcheck="false" class="span12" name="scripts.smartdns_address.conf" style="font-family:'Courier New'; font-size:12px;"><% nvram_dump("scripts.smartdns_address.conf",""); %></textarea>
												</div>
											</td>
										</tr>
										<tr>
											<td colspan="6" >
												<i class="icon-hand-right"></i> <a href="javascript:spoiler_toggle('script10')"><span>IP黑名单:</span></a>
												<div id="script10" style="display:none;">
													<textarea rows="8" wrap="off" spellcheck="false" class="span12" name="scripts.smartdns_blacklist-ip.conf" style="font-family:'Courier New'; font-size:12px;"><% nvram_dump("scripts.smartdns_blacklist-ip.conf",""); %></textarea>
												</div>
											</td>
										</tr>
										<tr>
											<td colspan="6" >
												<i class="icon-hand-right"></i> <a href="javascript:spoiler_toggle('script12')"><span>IP白名单:</span></a>
												<div id="script12" style="display:none;">
													<textarea rows="8" wrap="off" spellcheck="false" class="span12" name="scripts.smartdns_whitelist-ip.conf" style="font-family:'Courier New'; font-size:12px;"><% nvram_dump("scripts.smartdns_whitelist-ip.conf",""); %></textarea>
												</div>
											</td>
										</tr>
										<tr>
											<td colspan="6" >
												<i class="icon-hand-right"></i> <a href="javascript:spoiler_toggle('script11')"><span>自定义设置:</span></a>
												<div id="script11" style="display:none;">
													<textarea rows="8" wrap="off" spellcheck="false" class="span12" name="scripts.smartdns_custom.conf" style="font-family:'Courier New'; font-size:12px;"><% nvram_dump("scripts.smartdns_custom.conf",""); %></textarea>
												</div>
											</td>
										</tr>
										<tr>
											<th width="50%">广告过滤</th>
											<td>
												<div class="main_itoggle">
													<div id="sdns_adblock_on_of">
														<input type="checkbox" id="sdns_adblock_fake"<% nvram_match_x("", "sdns_adblock", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_adblock", "0", "value=0"); %>>
													</div>
												</div>
												<div style="position: absolute; margin-left: -10000px;">
													<input type="radio" value="1" name="sdns_adblock" id="sdns_adblock_1"<% nvram_match_x("", "sdns_adblock", "1", "checked"); %>><#checkbox_Yes#>
													<input type="radio" value="0" name="sdns_adblock" id="sdns_adblock_0"<% nvram_match_x("", "sdns_adblock", "0", "checked"); %>><#checkbox_No#>
												</div>
											</td>
										</tr>
										<tr>
											<th width="50%">过滤文件地址:</th>
											<td>
												<input type="text" class="input" size="15" name="sdns_adblock_url" style="width: 280px"  value="<% nvram_get_x("","sdns_adblock_url"); %>" />
											</td>
										</tr>
										<tr> <th width="50%">加载ChnrouteIP为白名单</th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_white_on_of">
                                                    <input type="checkbox" id="sdns_white_fake" <% nvram_match_x("", "sdns_white", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_white", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_white" id="sdns_white_1" <% nvram_match_x("", "sdns_white", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_white" id="sdns_white_0" <% nvram_match_x("", "sdns_white", "0", "checked"); %>><#checkbox_No#>
                                                </div>
												<div><span style="color:#888;">此项可配合科学上网来实现大陆IP才走国内DNS</span></div>
												<div><span style="color:#888;">需在上游服务器国内组中开启白名单过滤[-whitelist-ip]</span></div>
                                            </td>
                                        </tr>
										<tr> <th width="50%">加载ChnrouteIP为黑名单</th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_black_on_of">
                                                    <input type="checkbox" id="sdns_black_fake" <% nvram_match_x("", "sdns_black", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_black", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_black" id="sdns_black_1" <% nvram_match_x("", "sdns_black", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_black" id="sdns_black_0" <% nvram_match_x("", "sdns_black", "0", "checked"); %>><#checkbox_No#>
                                                </div>
												<div><span style="color:#888;">此项可配合科学上网来实现大陆IP禁止走国外DNS</span></div>
												<div><span style="color:#888;">需在上游服务器国外组中开启黑名单过滤[-blacklist-ip]</span></div>
                                            </td>
                                        </tr>
										<tr> <th width="50%">生成coredump</th>
                                            <td>
                                                <div class="main_itoggle">
                                                <div id="sdns_coredump_on_of">
                                                    <input type="checkbox" id="sdns_coredump_fake" <% nvram_match_x("", "sdns_coredump", "1", "value=1 checked"); %><% nvram_match_x("", "sdns_coredump", "0", "value=0"); %>>
                                                </div>
                                                </div>
                                                <div style="position: absolute; margin-left: -10000px;">
                                                    <input type="radio" value="1" name="sdns_coredump" id="sdns_coredump_1" <% nvram_match_x("", "sdns_coredump", "1", "checked"); %>><#checkbox_Yes#>
                                                    <input type="radio" value="0" name="sdns_coredump" id="sdns_coredump_0" <% nvram_match_x("", "sdns_coredump", "0", "checked"); %>><#checkbox_No#>
                                                </div>
                                            </td>
                                        </tr>
										</table>
										</div>										
                                    <table class="table">									
                                        <tr>
                                            <td colspan="6">
                                                <center><input class="btn btn-primary" style="width: 219px" type="button" value="<#CTL_apply#>" onclick="applyRule()" /></center>
                                            </td>
                                        </tr>
                                    </table>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
    </form>
    <div id="footer"></div>
</div>

</body>
</html>
