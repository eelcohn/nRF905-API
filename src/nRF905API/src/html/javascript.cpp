#include "html.h"					// HTTP_CODE_OK, HTTP_HTML_RESPONSE, HTTP_JS_RESPONSE
#include "javascript.h"				// HTML_SCRIPT_LANGUAGE_EN
#include "../../board.h"				// server.*


void javascriptAddHandlers(void) {
	server.on(FPSTR(HTML_SCRIPT_LANGUAGE_EN)	, html_scriptLanguage_EN);
}

void html_scriptLanguage_EN(void) {
	server.setContentLength(CONTENT_LENGTH_UNKNOWN);	// Start chunked HTTP transfer
	server.send(HTTP_CODE_OK, HTTP_JS_RESPONSE, "");
	server.sendContent(FPSTR(JAVASCRIPT_LANGUAGE_EN));
	server.sendContent(FPSTR(JAVASCRIPT_LANGUAGE_GENERIC));
	server.sendContent_P("");
}

