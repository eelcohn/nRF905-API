#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__

#include "en_html.h"
#include "en_api.h"
#include "../../config.h"			// LANGUAGE

const char SELECTED_LANGUAGE[] PROGMEM		= LANGUAGE;
const char LANGUAGE_CODES[][16] PROGMEM	= {
	"en",
	"nl"
};
const char LANGUAGES[][16] PROGMEM		= {
	"English",
	"Nederlands"
};

#endif

