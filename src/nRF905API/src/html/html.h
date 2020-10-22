#ifndef __HANDLER_HTML_H__
#define __HANDLER_HTML_H__

#include "icons.h"				// ICON_*
#include "../../board.h"			// PROGMEM
#include "../../nRF905API.h"			// FIRMWARE_TITLE, FIRMWARE_VERSION

/* HTTP response codes */
#define HTTP_CODE_OK				200
#define HTTP_CODE_SEE_OTHER			303
#define HTTP_CODE_NOT_FOUND			404
#define HTTP_CODE_METHOD_NOT_ALLOWED		405
#define HTTP_CODE_INTERNAL_SERVER_ERROR	500
#define HTTP_CODE_VERSION_NOT_SUPPORTED	505

/* HTML endpoints */
const char HTML_ROOT_URL[] PROGMEM		= "/";
const char HTML_CONFIG_URL[] PROGMEM		= "/config";
const char HTML_FAN_URL[] PROGMEM		= "/fan/";

/* HTTP content types */
const char HTTP_HTML_RESPONSE[] PROGMEM	= "text/html; charset=utf-8;";
const char HTTP_SVGXML_RESPONSE[] PROGMEM	= "image/svg+xml";

/* HTTP jeaders */
const char HTTP_HEADER_LOCATION[] PROGMEM	= "Location";
const char HTTP_HEADER_CACHE_CONTROL[] PROGMEM	= "Cache-Control";
const char HTTP_HEADER_NO_CACHE[] PROGMEM	= "no-cache";

/* HTTP authentication */	// TODO move to json_api.h
const char HTTP_HTML_AUTH_REALM[]		= FIRMWARE_TITLE " authentication";	// TODO do this in PROGMEM
const char HTTP_HTML_AUTH_FAILED[]		= "Authentication failed";		// TODO do this in PROGMEM

/* HTML header and footer */
const char HTML_HEADER1[] PROGMEM		= R"(
<!DOCTYPE html>
<html lang=")";
//	"input {color: #000; width: 180px; box-sizing: border-box; border: 2px solid #ccc; border-radius: 5px;}\n" 
//	"input[type=password] {background-image: url('" ICON_VISIBILITY_OFF_URL "'); background-position: 98%; background-repeat: no-repeat;}\n"
//	document.getElementByID('wifi_password').style.backgroundImage=('" ICON_VISIBILITY_URL "')
//	"#myid {background-image: url('searchicon.png'); background-position: 98%; background-repeat: no-repeat;}" 
const char HTML_HEADER2[] PROGMEM		= \
	"\">\n" \
	"<head>\n" \
	"<title>" FIRMWARE_TITLE " v" FIRMWARE_VERSION "</title>\n" \
	"<meta charset=\"UTF-8\">\n" \
	"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n" \
	"<link rel=\"stylesheet\" href=\"/s/style.css\"/>\n" \
	"<script src=\"/s/lang_en.js\"></script>\n" \
	"</head>\n" \
	"<body onload=\"setlang();\">\n" \
	"	<div class=\"title\">";

const char HTML_HEADER3[] PROGMEM		= \
	"</div>\n";
const char HTML_FOOTER[] PROGMEM		= \
	"</body>\n" \
	"</html>\n";
	// TODO replace Show password/Hide password with MSG_SHOW_PASSWORD and MSG_HIDE_PASSWORD

/* Function declarations */
void htmlAddHandlers(void);
void htmlRoot(void);
void htmlConfig(void);
void htmlFan(void);
void htmlHeader(void);
void htmlFooter(void);

#endif

