#ifndef __ICONS_H__
#define __ICONS_H__

#include "../../board.h"	// PROGMEM

// https://material.io/resources/icons/
// Compress with: https://vecta.io/nano
// All icons are 24dpi

#define ICON_FAVICON_URL			"/favicon.ico"
#define ICON_CHECK_URL				"/i/chk.svg"
#define ICON_DICE_URL				"/i/dce.svg"
#define ICON_ERROR_URL				"/i/err.svg"
#define ICON_FAN_URL				"/i/fan.svg"
#define ICON_INFO_URL				"/i/inf.svg"
#define ICON_LANGUAGE_URL			"/i/lan.svg"
#define ICON_MORE_URL				"/i/more.svg"
#define ICON_PENDING_URL			"/i/pnd.svg"
#define ICON_REBOOT_URL			"/i/rbt.svg"
#define ICON_REMOTE_CONTROL_URL		"/i/rc.svg"
#define ICON_POWER_URL				"/i/pwr.svg"
#define ICON_POWER_OFF_URL			"/i/pwroff.svg"
#define ICON_RF_URL				"/i/rf.svg"
#define ICON_SEARCH_URL			"/i/src.svg"
#define ICON_SECURITY_URL			"/i/sec.svg"
#define ICON_SETTINGS_URL			"/i/set.svg"
#define ICON_SYNC_URL				"/i/snc.svg"
#define ICON_SYNC_PROBLEM_URL			"/i/sncerr.svg"
#define ICON_VISIBILITY_URL			"/i/vis.svg"
#define ICON_VISIBILITY_OFF_URL		"/i/visoff.svg"
#define ICON_WARNING_URL			"/i/wrn.svg"
#define ICON_WIFI_URL				"/i/wfi.svg"

const char ICON_CHECK[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm-2 15l-5-5 1.4-1.4 3.6 3.58 7.6-7.6L19 8l-9 9z\"/></svg>";
const char ICON_DICE[] PROGMEM			= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M19 3H5c-1.1 0-2 .9-2 2v14c0 1.1.9 2 2 2h14c1.1 0 2-.9 2-2V5c0-1.1-.9-2-2-2zM7.5 18c-.83 0-1.5-.67-1.5-1.5S6.67 15 7.5 15s1.5.67 1.5 1.5S8.33 18 7.5 18zm0-9C6.67 9 6 8.33 6 7.5S6.67 6 7.5 6 9 6.67 9 7.5 8.33 9 7.5 9zm4.5 4.5c-.83 0-1.5-.67-1.5-1.5s.67-1.5 1.5-1.5 1.5.67 1.5 1.5-.67 1.5-1.5 1.5zm4.5 4.5c-.83 0-1.5-.67-1.5-1.5s.67-1.5 1.5-1.5 1.5.67 1.5 1.5-.67 1.5-1.5 1.5zm0-9c-.83 0-1.5-.67-1.5-1.5S15.67 6 16.5 6s1.5.67 1.5 1.5S17.33 9 16.5 9z\"/></svg>";
const char ICON_ERROR[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-2h2v2zm0-4h-2V7h2v6z\"/></svg>";
const char ICON_FAN[] PROGMEM			= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M12 12c0-3 2.5-5.5 5.5-5.5S23 9 23 12H12zm0 0c0 3-2.5 5.5-5.5 5.5S1 15 1 12h11zm0 0c-3 0-5.5-2.5-5.5-5.5S9 1 12 1v11zm0 0c3 0 5.5 2.5 5.5 5.5S15 23 12 23V12z\"/></svg>";
const char ICON_INFO[] PROGMEM			= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-6h2v6zm0-8h-2V7h2v2z\"/></svg>";
const char ICON_LANGUAGE[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M12 2C6.47 2 2 6.48 2 12s4.47 10 10 10A10.01 10.01 0 0 0 22 12 10.01 10.01 0 0 0 11.99 2zm6.93 6h-2.95a15.65 15.65 0 0 0-1.38-3.56A8.03 8.03 0 0 1 18.92 8zM12 4.04c.83 1.2 1.48 2.53 1.9 3.96H10.1c.43-1.43 1.08-2.76 1.9-3.96zM4.26 14C4.1 13.36 4 12.7 4 12s.1-1.36.26-2h3.38c-.08.66-.14 1.32-.14 2s.06 1.34.14 2H4.26zm.82 2h2.95a15.65 15.65 0 0 0 1.38 3.56c-1.84-.63-3.37-1.9-4.33-3.56zm2.95-8H5.08c.96-1.66 2.5-2.93 4.33-3.56A15.65 15.65 0 0 0 8.03 8zM12 19.96c-.83-1.2-1.48-2.53-1.9-3.96h3.82c-.43 1.43-1.08 2.76-1.9 3.96zM14.34 14H9.66c-.1-.66-.16-1.32-.16-2s.07-1.35.16-2h4.68c.1.65.16 1.32.16 2s-.07 1.34-.16 2zm.25 5.56A15.65 15.65 0 0 0 15.97 16h2.95a8.03 8.03 0 0 1-4.33 3.56zM16.36 14c.08-.66.14-1.32.14-2s-.06-1.34-.14-2h3.38c.16.64.26 1.3.26 2s-.1 1.36-.26 2h-3.38z\"/></svg>";
const char ICON_MORE[] PROGMEM			= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M6 10c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2zm12 0c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2zm-6 0c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2z\"/></svg>";
const char ICON_PENDING[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zM7 13.5a1.5 1.5 0 1 1 0-3 1.5 1.5 0 1 1 0 3zm5 0a1.5 1.5 0 1 1 0-3 1.5 1.5 0 1 1 0 3zm5 0a1.5 1.5 0 1 1 0-3 1.5 1.5 0 1 1 0 3z\"/></svg>";
const char ICON_POWER[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M16 7V3h-2v4h-4V3H8v4H8C7 7 6 8 6 9v5.5L9.5 18v3h5v-3l3.5-3.5V9c0-1-1-2-2-2z\"/></svg>";
const char ICON_POWER_OFF[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\" fill=\"black\"><path d=\"M18 14.5V9c0-1-1-2-2-2V3h-2v4h-4V3H8v2.48l9.5 9.5.5-.5zm-1.76 1.77L7.2 7.2H7.2L3.98 4 2.7 5.25 6.07 8.6c-.03.13-.07.26-.07.4v5.48L9.5 18v3h5v-3l.48-.48L19.45 22l1.26-1.28-4.47-4.46z\"/></svg>";
const char ICON_REBOOT[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M12 6v3l4-4-4-4v3a8 8 0 0 0-8 8 7.93 7.93 0 0 0 1.24 4.26L6.7 14.8A5.87 5.87 0 0 1 6 12c0-3.3 2.7-6 6-6zm6.76 1.74L17.3 9.2c.44.84.7 1.8.7 2.8 0 3.3-2.7 6-6 6v-3l-4 4 4 4v-3a8 8 0 0 0 8-8 7.93 7.93 0 0 0-1.24-4.26z\"/></svg>";
const char ICON_REMOTE_CONTROL[] PROGMEM	= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M15 9H9c-.55 0-1 .45-1 1v12c0 .55.45 1 1 1h6c.55 0 1-.45 1-1V10c0-.55-.45-1-1-1zm-3 6c-1.1 0-2-.9-2-2s.9-2 2-2 2 .9 2 2-.9 2-2 2zM7.05 6.05l1.4 1.4a5.02 5.02 0 0 1 7.08 0l1.4-1.4C15.68 4.78 13.93 4 12 4s-3.68.78-4.95 2.05zM12 0C8.96 0 6.2 1.23 4.22 3.22l1.4 1.4A9.01 9.01 0 0 1 12 2c2.5 0 4.74 1 6.36 2.64l1.4-1.4A10.89 10.89 0 0 0 12 0z\"/></svg>";
const char ICON_RF[] PROGMEM			= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zM8.46 14.45l-1.36-.62c.28-.6.4-1.24.4-1.86a4.42 4.42 0 0 0-.4-1.8l1.36-.63c.35.75.53 1.56.54 2.4.01.86-.17 1.7-.54 2.5zM11.53 16l-1.3-.74c.52-.92.78-1.98.78-3.15 0-1.2-.27-2.33-.8-3.4l1.34-.67a9.02 9.02 0 0 1 .96 4.07c0 1.43-.33 2.74-.98 3.9zm3.14 1.32l-1.35-.66c.78-1.6 1.18-3.18 1.18-4.7s-.4-3.07-1.18-4.64l1.34-.67c.9 1.78 1.34 3.56 1.34 5.3 0 1.74-.44 3.54-1.33 5.35z\"/></svg>";
const char ICON_SEARCH[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M15.5 14h-.8l-.28-.27A6.47 6.47 0 0 0 16 9.5a6.5 6.5 0 1 0-13 0A6.5 6.5 0 0 0 9.5 16a6.47 6.47 0 0 0 4.23-1.57l.27.28v.8l5 5 1.5-1.5-5-5zm-6 0C7 14 5 12 5 9.5S7 5 9.5 5 14 7 14 9.5 12 14 9.5 14z\"/></svg>";
const char ICON_SECURITY[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M12 1L3 5v6c0 5.55 3.84 10.74 9 12 5.16-1.26 9-6.45 9-12V5l-9-4zm0 11h7c-.53 4.12-3.28 7.8-7 8.94V12H5V6.3l7-3.1V12z\"/></svg>";
const char ICON_SETTINGS[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M19.14 12.94c.04-.3.06-.6.06-.94 0-.32-.02-.64-.07-.94l2.03-1.58a.49.49 0 0 0 .12-.61l-1.92-3.32c-.12-.22-.37-.3-.6-.22l-2.4.96c-.5-.38-1.03-.7-1.62-.94L14.4 2.8c-.04-.24-.24-.4-.48-.4h-3.84c-.24 0-.43.17-.47.4l-.36 2.54a7.22 7.22 0 0 0-1.62.94l-2.4-.96c-.22-.08-.47 0-.6.22l-1.9 3.32c-.12.2-.08.47.12.6l2.03 1.58a5.87 5.87 0 0 0-.09.94c0 .3.02.64.07.94l-2.03 1.58a.49.49 0 0 0-.12.61l1.92 3.32c.12.22.37.3.6.22l2.4-.96c.5.38 1.03.7 1.62.94l.36 2.54c.05.24.24.4.48.4h3.84c.24 0 .44-.17.47-.4l.36-2.54a6.86 6.86 0 0 0 1.62-.94l2.4.96c.22.08.47 0 .6-.22l1.92-3.32c.12-.22.07-.47-.12-.6l-2-1.58zM12 15.6A3.61 3.61 0 0 1 8.4 12 3.61 3.61 0 0 1 12 8.4a3.61 3.61 0 0 1 3.6 3.6 3.61 3.61 0 0 1-3.6 3.6z\"/></svg>";
const char ICON_SYNC[] PROGMEM			= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M12 4V1L8 5l4 4V6c3.3 0 6 2.7 6 6a5.87 5.87 0 0 1-.7 2.8l1.46 1.46A7.93 7.93 0 0 0 20 12a8 8 0 0 0-8-8zm0 14c-3.3 0-6-2.7-6-6a5.87 5.87 0 0 1 .7-2.8L5.24 7.74A7.93 7.93 0 0 0 4 12a8 8 0 0 0 8 8v3l4-4-4-4v3z\"/></svg>";
const char ICON_SYNC_PROBLEM[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M3 12c0 2.2.9 4.2 2.36 5.64L3 20h6v-6l-2.24 2.24C5.68 15.15 5 13.66 5 12a5.99 5.99 0 0 1 4-5.65v-2.1c-3.45.9-6 4-6 7.74zm8 5h2v-2h-2v2zM21 4h-6v6l2.24-2.24C18.32 8.85 19 10.34 19 12a5.99 5.99 0 0 1-4 5.65v2.1c3.45-.9 6-4 6-7.74 0-2.2-.9-4.2-2.36-5.64L21 4zm-10 9h2V7h-2v6z\"/></svg>";
const char ICON_VISIBILITY[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"18\" width=\"18\" viewBox=\"0 0 24 24\" fill=\"black\"><path d=\"M12 4.5C7 4.5 2.73 7.6 1 12c1.73 4.4 6 7.5 11 7.5s9.27-3.1 11-7.5c-1.73-4.4-6-7.5-11-7.5zM12 17c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z\"/></svg>";
const char ICON_VISIBILITY_OFF[] PROGMEM	= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"18\" width=\"18\" viewBox=\"0 0 24 24\" fill=\"black\"><path d=\"M12 7c2.76 0 5 2.24 5 5 0 .65-.13 1.26-.36 1.83l2.92 2.92c1.5-1.26 2.7-2.9 3.43-4.75-1.73-4.4-6-7.5-11-7.5-1.4 0-2.74.25-3.98.7l2.16 2.16C10.74 7.13 11.35 7 12 7zM2 4.27L4.74 7c-1.66 1.3-2.96 3-3.74 5 1.73 4.4 6 7.5 11 7.5 1.55 0 3.03-.3 4.38-.84l.42.42L19.73 22 21 20.73 3.27 3 2 4.27zM7.53 9.8l1.55 1.55A2.82 2.82 0 0 0 9 12c0 1.66 1.34 3 3 3a2.82 2.82 0 0 0 .65-.08l1.55 1.55c-.67.33-1.4.53-2.2.53-2.76 0-5-2.24-5-5 0-.8.2-1.53.53-2.2zm4.3-.78L15 12.17V12c0-1.66-1.34-3-3-3l-.17.01z\"/></svg>";
const char ICON_WARNING[] PROGMEM		= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M1 21h22L12 2 1 21zm12-3h-2v-2h2v2zm0-4h-2v-4h2v4z\"/></svg>";
const char ICON_WIFI[] PROGMEM			= "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"24\" width=\"24\"><path d=\"M1 9l2 2a12.73 12.73 0 0 1 18 0l2-2A15.57 15.57 0 0 0 1 9zm8 8l3 3 3-3c-1.65-1.66-4.34-1.66-6 0zm-4-4l2 2c2.76-2.76 7.24-2.76 10 0l2-2C15.14 9.14 8.87 9.14 5 13z\"/></svg>";

void iconsAddHandlers(void);

#endif

