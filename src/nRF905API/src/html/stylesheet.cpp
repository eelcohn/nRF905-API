#include "html.h"					// HTTP_CODE_OK, HTTP_HTML_RESPONSE, HTTP_CSS_RESPONSE
#include "stylesheet.h"				// HTML_STYLESHEET_URL
#include "../../board.h"				// server.*


void stylesheetAddHandlers(void) {
	server.on(FPSTR(HTML_STYLESHEET_URL)	, html_styleSheet);
}

void html_styleSheet(void) {
	server.setContentLength(CONTENT_LENGTH_UNKNOWN);	// Start chunked HTTP transfer
	server.send(HTTP_CODE_OK, HTTP_CSS_RESPONSE, "");
	server.sendContent(FPSTR(STYLESHEET));
	server.sendContent_P("");
}

