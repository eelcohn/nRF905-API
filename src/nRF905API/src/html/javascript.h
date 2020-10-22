#ifndef __JAVASCRIPT_H__
#define __JAVASCRIPT_H__

#include "../../board.h"				// PROGMEM
#include "../../nvram.h"				// DEFAULT_*
#include "../language/language.h"			// MSG_EN_*

const char HTTP_JS_RESPONSE[] PROGMEM			= "application/javascript";
const char HTML_SCRIPT_LANGUAGE_EN[] PROGMEM		= "/s/lang_en.js";
const char JAVASCRIPT_LANGUAGE_EN[] PROGMEM		= "\
/* jshint esversion: 6 */ \
var show_password='" MSG_EN_SHOW_PASSWORD "'; \
var hide_password='" MSG_EN_HIDE_PASSWORD "'; \
var l=[]; \
l.n95='" MSG_EN_NRF905_CONFIGURATION "'; \
l.aut='" MSG_EN_HTTP_AUTHENTICATION "'; \
l.dpw='" DEFAULT_HTTP_PASSWORD "'; \
l.dun='" DEFAULT_HTTP_USERNAME "'; \
l.fac='" MSG_EN_FAN_AUTOCONFIG "'; \
l.fmc='" MSG_EN_FAN_MANUALCONFIG "'; \
l.fsk='" MSG_EN_FAN_SKIPCONFIG "'; \
l.fid='" MSG_EN_MAIN_UNIT_ID "'; \
l.fpi='" MSG_EN_FAN_PLUGIN "'; \
l.fpm='" MSG_EN_FAN_PARAMETERS "'; \
l.fup='" MSG_EN_FAN_UNPLUG "'; \
l.hnm='" MSG_EN_HOSTNAME "'; \
l.hpw='" MSG_EN_HIDE_PASSWORD "'; \
l.lng='" MSG_EN_LANGUAGE "'; \
l.mid='" MSG_EN_MY_DEVICE_ID "'; \
l.ncf='" MSG_EN_NRF905_CONFIG_FAILED "'; \
l.ncp='" MSG_EN_UNABLE_TO_COMPLETE_JOIN "'; \
l.net='" MSG_EN_NETWORK_ADDRESS "'; \
l.nnc='" MSG_EN_NEW_NETWORK_CONFIGURED "'; \
l.nnd='" MSG_EN_NO_NETWORKS_DISCOVERED "'; \
l.nti='" MSG_EN_INTERVAL "'; \
l.nto='" MSG_EN_OFFSET "'; \
l.ntp='" MSG_EN_NTP_SETTINGS "'; \
l.nts='" MSG_EN_SERVER "'; \
l.ous='" MSG_EN_OTA_UPDATE_SETTINGS "'; \
l.phs='" MSG_EN_PASSWORD_HASH "'; \
l.prt='" MSG_EN_PORT "'; \
l.pwd='" MSG_EN_PASSWORD "'; \
l.rgs='" MSG_EN_REGIONAL_SETTINGS "'; \
l.sid='" MSG_EN_SSID "'; \
l.ucj='" MSG_EN_UNABLE_TO_COMPLETE_JOIN "'; \
l.unm='" MSG_EN_USERNAME "'; \
l.urs='" MSG_EN_JOIN_UNKNOWN_RESULT "'; \
l.wfi='" MSG_EN_WIFI_SETTINGS "'; \
l.dnc='" MSG_EN_NEW_NETWORK_CONFIGURED "'; \
l.fnr='" MSG_EN_FAN_NO_RESPONSE "'; \
l.fss='" MSG_EN_FAN_SET_SUCCESS "'; \
var v=[]; \
v.back='" MSG_EN_BACK "'; \
v.next='" MSG_EN_NEXT "'; \
v.finish='" MSG_EN_FINISH "'; \
v.reboot='" MSG_EN_REBOOT "'; \
v.submit='" MSG_EN_SUBMIT "'; \
var t=[]; \
t.icon_http_password=show_password; \
t.icon_wifi_password=show_password; \
t.icon_ota_password=show_password; \
t.icon_my_device_id='" MSG_EN_GENERATE_RANDOM_ID "'; \
";
const char JAVASCRIPT_LANGUAGE_GENERIC[] PROGMEM	= R"(
function w(t) {
	document.write(l[t]);
}
function setlang() {
	var e, id;
	for (e in v) {
		id=document.getElementById(e);
		if(id) {
			id.value=v[e];
		}}
	for (e in t) {
		id=document.getElementById(e);
		if(id) {
			id.title=t[e];
		}
	}
}
function updateFanForm() {
	var x, formids=['lfrequency', 'frequency', 'ltxpower', 'txpower', 'lrxpower', 'rxpower', 'lretransmit', 'retransmit', 'ltxaddrwidth', 'txaddrwidth', 'lrxaddrwidth', 'rxaddrwidth', 'ltxpayloadwidth', 'txpayloadwidth', 'lrxpayloadwidth', 'rxpayloadwidth', 'lrxaddress', 'rxaddress', 'lupclk', 'upclk', 'lupclken', 'upclken', 'lxof', 'xof', 'lcrcenable', 'crcenable', 'lcrcmode', 'crcmode'];
	formids.forEach(id => {x = document.getElementById(id);
		if (document.getElementById('profile').selectedIndex === 2) {
 			x.style.display = '';
		} else {
			x.style.display = 'none';
		}
	});
}
function changeInputType(input_id, icon_id) {
	var input=document.getElementById(input_id);
	var icon=document.getElementById(icon_id);
	if(input.type=='text') {
		input.type='password';
		icon.src=')" ICON_VISIBILITY_OFF_URL R"(';
		icon.title=show_password;
	} else {
		input.type='text';
		icon.src=')" ICON_VISIBILITY_URL R"(';
		icon.title=hide_password;
	}
}
function rndid(id) {
	var v;
	do {
		v=((Math.floor(Math.random() * 16)*16)+((Math.floor(Math.random() * 16))));
	} while ((v === 0) || (v === 255));
	document.getElementById(id).value=v.toString(16).toUpperCase().padStart(2, '0');
}
)";

void javascriptAddHandlers(void);
void html_scriptLanguage_EN(void);

#endif

