#ifndef __STYLESHEET_H__
#define __STYLESHEET_H__

const char HTTP_CSS_RESPONSE[] PROGMEM		= "text/css";
const char HTML_STYLESHEET_URL[] PROGMEM	= "/s/style.css";
const char STYLESHEET[] PROGMEM		= R"(
html {max-width: 600px; margin: 0 auto;}
body {background:#444; color: #fff; font-family:verdana, sans-serif; text-align: center;}
fieldset {margin: 24px; text-align: left;}
input {color: #000; width: 180px; background-color: white; box-sizing: border-box; border: 2px solid #ccc; border-radius: 5px; padding: 6px 36px 6px 6px;}
input[type=radio] {text-align: left;}
label {display: inline-block; margin: 10px; text-align: right; width: 235px;}​
legend {text-align: left;}
.title {font-size: 20px; font-weight: bold; margin-top: 2em; margin-bottom: 2em;}
.message {font-style: italic; margin: 20px;}
.form-list {display: table; width: 100%;}
.form-row {display: table-row;}
.form-label {display: table-cell; vertical-align: middle;}
.form-input {display: table-cell; float: right; vertical-align: middle;}
.form-icon {display: table-cell; width: 24px; vertical-align: middle;}
.icon-password {height: 24px; width: 24px; margin-left: 10px;}
.icon-white {filter: invert(100%) sepia(6%) saturate(0%) hue-rotate(1deg) brightness(104%) contrast(109%);}
.icon-big {height: 48px; width: 48px;}
)";

void stylesheetAddHandlers(void);
void html_styleSheet(void);

#endif

