/*
 *	DIE - GTK+2 Demo
 *	----------------
 *
 *	DESCRIPTION:	Small GTK+2 demo, faking an animation of a die (2D & 3D).
 *	AUTHOR:		migf1 <mig_f1@hotmail.com> | http://x-karagiannis.gr/prg/
 *	WEBSITE:	https://github.com/migf1/gtk_dice
 *	LICENSE:	Free for all
 *
 *	PROGR.LANG:	C (ANSI 89 / ISO C90)
 *	PLATFORM:	Cross-platfrom
 *
 *	REQUIREMENTS:	for Execution:
 *				Runtime of GTK+ v2.24.8 or newer (*not* GTK+3)
 *			for Compilation:
 *				GTK+ v2.24.8 or newer DevPackages (*not* GTK+3)
 *				(they include the runtime)
 *
 *	BUILT INFO:	Developed & tested on Windows XP SP3, using:
 *				GTK+2 v2.24.8 *all-in-one* bundle
 *				MinGW32 GCC v4.70
 *				Glade v3.8.3 (*not* v3.14.x, that's for GTK+3)
 *			Tested on:
 *				Windows XP SP3	(32bit)
 *				Windows 7	(64bit)
 *				Ubuntu 1.10	(32bit)
 *
 *	Please read also the file README.txt for more info, links, etc.
 */

/* -------------------------------
 * Preprocessor
 * -------------------------------
 */

/* prepare GLib's i18n localization via GNU-gettext (see Useful-Links in README.txt)
 * NOTE: to be safe, always set GETTEXT_PACKAGE before including <glib/gi18n.h>
 */
#define GETTEXT_PACKAGE	"gtk_dice"	/* our app's translation text-domain */
#define LOCALEDIR	"lang"		/* locale directory (to host translations) */
#define OUTCODESET	"UTF-8"		/* desired output-codeset for translations */
#include <glib/gi18n.h>

/* we need this for opening http:// and mailto: links when OS is Windows */
#ifdef G_OS_WIN32
#include <windows.h>
#endif

/* other header-file inclusions */
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdarg.h>
#include <locale.h>

/* locale related constants & string literals */

#define MAXSIZ_LOCALE		(511+1)

#define STR_EN_LOCALE		"en_US.utf8"
#define STR_EN_LOCALE_LANG	"en_US.UTF-8"
#define STR_EN_LOCALE_LANGUAGE	"en"

#define STR_EL_LOCALE		"el_GR.utf8"
#define STR_EL_LOCALE_LANG	"el_GR.UTF-8"
#define STR_EL_LOCALE_LANGUAGE	"el"

/* definitions of some handy symbols */

#define MAXSIZ_DBGMSG		(1023+1)
#define MAXSIZ_FNAME		(255+1)

#define DIR_GUI			"gui/"			/* gui resources */
#define FNAME_APPICON		DIR_GUI"3d_01.png"
#define FNAME_APPLOGO		FNAME_APPICON
#define FNAME_GLADE		DIR_GUI"dice.glade"

/* total number of image-files used for the 3d & 2d roll-animations */
#define NFRAMES_3D			32
#define NFRAMES_2D			6

/* default settings for 3d rolling-effect */
#define SETTINGS_DEF3D_MAXSPINS		60
#define SETTINGS_DEF3D_BASEDELAY	7000L
#define SETTINGS_DEF3D_STEPDELAY	400L

/* default settings for 2d rolling-effect */
#define SETTINGS_DEF2D_MAXSPINS		12
#define SETTINGS_DEF2D_BASEDELAY	5000L
#define SETTINGS_DEF2D_STEPDELAY	30000L

/* GUI strings that are dynamically added/updated,
 * regardless their initial values specified in
 * the Glade file: FNAME_GLADE.
 * Only those enclosed in _() are translatable.
 */
#define TXT_APP_NAME		_("GTK+2 Demo - Die")
#define TXT_APP_VERSION		"0.2a"
#define TXT_APP_DESCRIPTION	_("Small GTK+2 Demo\nfaking 3D/2D die-animation")
#define TXT_APP_COPYRIGHT	"copyright (c) 2013"
#define TXT_APP_LICENSE		_("Free for all")
#define TXT_APP_WEBSITE_LABEL	"x-karagiannis.gr/prg/"
#define TXT_APP_WEBSITE		"http://x-karagiannis.gr/prg/"
#define TXT_APP_AUTHOR		"migf1 <mig_f1@hotmail.com>"
#define TXT_APP_ARTIST		"Napoleon\nhttp://opengameart.org/content/dice-1/"

#define TXT_WTITLE_ALERT_BOX	_("Alert Box")
#define TXT_BUTTON_ROLL2D	_("_Roll")
#define TXT_BUTTON_ROLLING	_("...rolling...")
#define TXTF_STATUSBAR_ONROLL	_(" Spin: %03d | Frame: %03d | Delay: %06ld")
#define TXTF_STATUSBAR_RESULT	_(" Result: %d")

/**
 * Macro displaying an error-message with debugging info within a GUI alert-box
 * (this is a convenient wrapper of the custom function myGtk_alert_box()).
 *
 * @param appWindow The owner-window of the alert-box (may be NULL).
 * @param errmsg The message to be displayed.
 */
#define DBG_GUI_ERRMSG( appWindow, errmsg )					\
	do {									\
		gchar dbgMsgOut[MAXSIZ_DBGMSG] = {'\0'};			\
		if ( !global_debugOn )						\
			break;							\
		g_snprintf(							\
			dbgMsgOut,						\
			MAXSIZ_DBGMSG,						\
			_("File\t: %s\nFunc\t: %s\nLine\t: %d\n\n%s"),		\
			__FILE__, __func__,  __LINE__,				\
			(const gchar *)(errmsg) ? errmsg : "\0"			\
			);							\
		myGtk_alert_box( GTK_WIDGET(appWindow), dbgMsgOut );		\
	} while(0)

/**
 * Macro displaying an error-message with debugging info in the stderr stream.
 */
#define DBG_STDERR_MSG( errmsg )						\
	do {									\
		if ( !global_debugOn )						\
			break;							\
		fputs( "*** ERROR:\n", stderr );				\
		fprintf(stderr,							\
			"*** File: %s | Func: %s | Line: %d\n*** %s\n",		\
			__FILE__, __func__,  __LINE__,				\
			(const char *)(errmsg) ? errmsg : "\0"			\
			);							\
	} while(0)

/* -------------------------------
 * Custom DataTypes
 * -------------------------------
 */

/* core data of the application (e.g. separated from the GUI ) */
typedef struct Core {
	GRand	*randGen;
	gint32	resultRolled;
}Core;

/* aliases of some GTK+2 widget types (just for consistency reasons) */
typedef GtkWidget GuiWindow, GuiDialog;

/* a bit more convenient GUI abstraction of the menus */
typedef struct GuiMenus {
	GtkWidget	*titleFile;
	GtkWidget	*itemQuit;
	GtkWidget	*titleLang;
	GtkWidget	*lang;
	GtkWidget	*itemEnglish;
	GtkWidget	*itemGreek;
	GtkWidget	*itemEnvLang;
	GtkWidget	*titleHelp;
	GtkWidget 	*itemAbout;
}GuiMenus;

/* more convenient GUI abstraction of the "Die area".
 * 	The GUI "Die area" consists of an event-box, an image & a "Roll" button.
 *	Since in GTK+2 image-widgets are not clickable, we create an event-box
 *	as their parent, and then we catch the signals emitted by that event-box
 *	(see the file: dice.glade, preferably with Glade).
 */
typedef struct GuiDieArea {
	GtkWidget	*evboxWidget;
	GtkWidget	*imgWidget;
	GtkWidget	*btnWidget;
}GuiDieArea;

/* UNUSED: there used to be 2 animation areas, one for 2d and one for 3d */
typedef struct GuiDieArea3d {
	GtkWidget	*vbox3d;
	GtkWidget	*evboxWidget;
	GtkWidget	*imgWidget;
}GuiDieArea3d;

/* a bit more convenient GUI abstraction of an horizontal slider */
typedef struct GuiHorzSlider {
	GtkWidget	*widget;
	GtkAdjustment	*adjustment;
	gdouble		minVal, maxVal, value;
	gdouble		stepInc, pageInc;
	gint		digits;
	/* non-critical fields, let them be as defined in Glade
	gdouble		pageSize;
	gboolean	drawValue;
	GtkPositionType	valuePos;
	*/
}GuiHorzSlider;

/* more convenient GUI abstraction of the "Settings area" */
typedef struct GuiSettings {
	GtkWidget	*chkbtnDebugInfoWidget;
	GtkWidget	*chkbtnRollEffectWidget;
	GtkWidget	*labelMaxStepsWidget;
	GuiHorzSlider	hsliderMaxSteps;
	GtkWidget	*labelBaseDelayWidget;
	GuiHorzSlider	hsliderBaseDelay;
	GtkWidget	*labelStepDelayWidget;
	GuiHorzSlider	hsliderStepDelay;
	GtkWidget	*btnDefault3dWidget;
	GtkWidget	*btnDefault2dWidget;
}GuiSettings;

/* a bit more convenient GUI abstraction of the status-bar */
typedef struct GuiStatusBar {
	GtkWidget 	*widget;
	guint		contextId;
	guint		currMessageId;
}GuiStatusBar;

typedef struct GuiLocaleEnv {
	gchar locale[MAXSIZ_LOCALE];
	gchar *varLang;
	gchar *varLanguage;	/* GTK+ does not check this on Win32 */
}GuiLocaleEnv;

/* a bit more convenient abstraction of the application's GUI
 *	consists of the smaller structs defined above, along
 *	with a pointer that links the GUI with the core-data.
 */
typedef struct Gui {
	gboolean	quitOnDestroyAppWindow;
	GuiLocaleEnv	callerLocaleEnv;
	GuiWindow	*appWindow;
	GuiDialog	*dlgAbout;
	GuiMenus	menu;
	GuiDieArea 	dieArea;
	GuiDieArea3d 	dieArea3d;	/* unused */
	GuiSettings	settings;
	GuiStatusBar	statusBar;
	Core		*linkToCoreData;
}Gui;

/* -------------------------------
 * Global Variables
 * -------------------------------
 */

static gboolean global_debugOn;	/* too lazy to wrap it */


/* -------------------------------
 * Function Prototypes (only the bare minimum needed)
 * -------------------------------
 */

static void	gui_refresh_default3d2d_buttons( Gui *gui );
static gboolean gui_reload_gtkGladeFile( Gui *gui, const gchar *fnameGlade );

/* -------------------------------
 * Function Definitions
 * -------------------------------
 */

/*************************************************//**
 * Print the specified arguments in the stdout stream,
 * only if 'global_debugOn' is TRUE.
 *****************************************************
 */
static void dbg_print_info( char *fmtxt, ... )
{
	va_list args;

	if ( !global_debugOn || !fmtxt )
		return;

	va_start(args, fmtxt);
	vprintf( fmtxt, args );
	va_end( args );
}

/*************************************************//**
 * Print info about GUI's inherited locale environment,
 * only if 'global_debugOn' is TRUE.
 *****************************************************
 */
static void dbg_print_gui_callerLocaleEnv( const Gui *gui, const gchar *title )
{
	/* sanity check */
	if ( !gui ) {
		return;
	}

	if ( title ) {
		dbg_print_info( "%s\n", title );
	}
	dbg_print_info( "locale:\t\t%s\n", gui->callerLocaleEnv.locale );
	dbg_print_info( "LANG:\t\t%s\n", gui->callerLocaleEnv.varLang );
	dbg_print_info( "LANGUAGE:\t%s\n", gui->callerLocaleEnv.varLanguage );
	dbg_print_info( "%c", '\n' );

}

/*************************************************//**
 * Print info about the current locale environment,
 * only if 'global_debugOn' is TRUE.
 *****************************************************
 */
static void dbg_print_locale_environment( const gchar *title )
{
	if ( title ) {
		dbg_print_info( "%s\n", title );
	}

	dbg_print_info( "locale:\t\t%s\n", setlocale(LC_ALL, NULL) );
	dbg_print_info( "LANG:\t\t%s\n", g_getenv("LANG") );
	dbg_print_info( "LANGUAGE:\t%s\n", g_getenv("LANGUAGE") );
	dbg_print_info( "%c", '\n' );

}

/*************************************************//**
 * Initialize localization via the GNU-gettext library.
 *****************************************************
 */
static gboolean init_localization_gettext(
	const char *transTextDomain,
	const char *localeDir,
	const char *outCodeset
	)
{
	/* sanity checks */
	if ( !transTextDomain || !localeDir || !outCodeset ) {
		return FALSE;
	}

	/* set the current locale to system default & save it */
	setlocale( LC_ALL, "" );

	/* ensure that the translated messages of the specified text-domain
	 * will be searched in the specified locale-directory rather than in
	 * the system locale database
	 */
	if ( !bindtextdomain(transTextDomain, localeDir) ) {
		return FALSE;
	}

	/* */
	if ( !bind_textdomain_codeset(transTextDomain, outCodeset) ) {
		return FALSE;
	}

	/* set the current default text-domain to the specified one */
	if ( !textdomain(transTextDomain) ) {
		return FALSE;
	}

	return TRUE;
}

/*************************************************//**
 * Set the specified arguments as the current locale environment
 *****************************************************
 */
static void set_locale_environment(
	const gchar *locale,
	const gchar *lang,
	const gchar *language
	)
{
	/* sanity checks */
	if ( !locale || !lang || !language ) {
		return;
	}

	setlocale( LC_ALL, locale );

	if ( g_getenv("LANG") ) {
		g_unsetenv("LANG");
	}
	g_setenv("LANG", lang, TRUE);

	if ( g_getenv("LANGUAGE") ) {
		g_unsetenv("LANGUAGE");
	}
	g_setenv( "LANGUAGE", language, TRUE );
}

/*************************************************//**
 * Force a redraw of the specified widget.
 *
 * 	First the widget is flagged for display, and then
 *	GTK+ is forced to iterate through its main-loop
 *	until no more events are left for processing.
 *****************************************************
 */
static void myGtk_widget_refresh( GtkWidget *widget )
{
	/* sanity check */
	if ( !widget )
		return;

	gtk_widget_show( widget );
	while( gtk_events_pending() )
		gtk_main_iteration();

	return;
}

/*************************************************//**
 * Display the specified message inside a simple modal dialog.
 *
 * 	The owner of the dialog is the window specified in
 *	the first argument, which usually is the main window
 *	of the application (if passed as NULL, the alert-box
 *	will not have an owner, which is fine but a bit odd).
 *****************************************************
 */
static void myGtk_alert_box( GtkWidget *appMainWindow, gchar *message )
{
	GtkWidget *alertBox = NULL;

	if ( appMainWindow )
		gtk_widget_set_sensitive( appMainWindow, FALSE );

	alertBox = gtk_message_dialog_new(
			GTK_WINDOW(appMainWindow),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO,
			GTK_BUTTONS_OK,
			"%s", message
		);

	gtk_window_set_title( GTK_WINDOW(alertBox), TXT_WTITLE_ALERT_BOX );
	gtk_dialog_run( GTK_DIALOG(alertBox) );
	gtk_widget_destroy( alertBox );

	if ( appMainWindow )
		gtk_widget_set_sensitive( appMainWindow, TRUE );
}

/*************************************************//**
 * Set the specified values to the fields of the
 * specified horizontal slider.
 *****************************************************
 */
static gboolean myGtk_horzSlider_set(
	GuiHorzSlider	*hslider,
	gdouble		value,		/* current value */
	gdouble		minVal,		/* lower bound */
	gdouble		maxVal,		/* upper bound */
	gdouble		stepInc,	/* step increment */
	gdouble		pageInc,	/* page increment */
	gint		digits		/* # of decimal digits (for increments) */
	)
{
	/* sanity checks */
	if ( !hslider ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (hslider)") );
		return FALSE;
	}
	if ( !hslider->widget || !hslider->adjustment ) {
		DBG_GUI_ERRMSG( NULL, _("hslider has not been properly created") );
		return FALSE;
	}

/*	hslider->value	= value;
*/
	hslider->minVal	= minVal;
	hslider->maxVal	= maxVal;
	hslider->stepInc= stepInc;
	hslider->pageInc= pageInc;
/*	hslider->digits	= digits;
*/
	gtk_scale_set_digits( GTK_SCALE(hslider->widget), digits );
	hslider->digits = gtk_scale_get_digits( GTK_SCALE(hslider->widget) );

	gtk_range_set_range(
		GTK_RANGE( hslider->widget ),
		hslider->minVal,
		hslider->maxVal
		);
	gtk_range_set_increments(
		GTK_RANGE( hslider->widget ),
		hslider->stepInc,
		hslider->pageInc
		);

	gtk_adjustment_set_value( hslider->adjustment, value );
	hslider->value = gtk_adjustment_get_value( hslider->adjustment );

	return TRUE;
}

/*************************************************//**
 * Change the value field of the specified horizontal slider
 *****************************************************
 */
static gboolean myGtk_horzSlider_set_value( GuiHorzSlider *hslider, gdouble value )
{
	/* sanity checks */
	if ( !hslider ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (hslider)") );
		return FALSE;
	}
	if ( !hslider->widget || !hslider->adjustment ) {
		DBG_GUI_ERRMSG( NULL, _("hslider has not been properly created") );
		return FALSE;
	}

	gtk_adjustment_set_value( hslider->adjustment, value );
	hslider->value = gtk_adjustment_get_value( hslider->adjustment );

	return TRUE;
}

/*************************************************//**
 * Check if the 3d-animation settings are currently
 * equal to their default values.
 *****************************************************
 */
static gboolean gui_has_default3d_settings( const Gui *gui )
{
	GuiSettings *settings = NULL;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	settings = (GuiSettings *) &gui->settings;
	if ( !settings->chkbtnRollEffectWidget
	|| !settings->hsliderMaxSteps.widget || !settings->hsliderMaxSteps.adjustment
	|| !settings->hsliderBaseDelay.widget || !settings->hsliderBaseDelay.adjustment
	|| !settings->hsliderStepDelay.widget || !settings->hsliderStepDelay.adjustment
	){
		DBG_GUI_ERRMSG( gui->appWindow, _("Found invalid GUI elements") );
		return FALSE;
	}

	return 	!gtk_toggle_button_get_active(
			GTK_TOGGLE_BUTTON(settings->chkbtnRollEffectWidget)
		)
		&& settings->hsliderMaxSteps.value == SETTINGS_DEF3D_MAXSPINS
		&& settings->hsliderBaseDelay.value == SETTINGS_DEF3D_BASEDELAY
		&& settings->hsliderStepDelay.value == SETTINGS_DEF3D_STEPDELAY
		;
}

/*************************************************//**
 * Check if the 2d-animation settings are currently
 * equal to their default values.
 *****************************************************
 */
static gboolean gui_has_default2d_settings( const Gui *gui )
{
	GuiSettings *settings = NULL;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	settings = (GuiSettings *) &gui->settings;
	if ( !settings->chkbtnRollEffectWidget
	|| !settings->hsliderMaxSteps.widget || !settings->hsliderMaxSteps.adjustment
	|| !settings->hsliderBaseDelay.widget || !settings->hsliderBaseDelay.adjustment
	|| !settings->hsliderStepDelay.widget || !settings->hsliderStepDelay.adjustment
	){
		DBG_GUI_ERRMSG( gui->appWindow, _("Found invalid GUI elements") );
		return FALSE;
	}

	return 	gtk_toggle_button_get_active(
			GTK_TOGGLE_BUTTON(settings->chkbtnRollEffectWidget)
		)
		&& settings->hsliderMaxSteps.value == SETTINGS_DEF2D_MAXSPINS
		&& settings->hsliderBaseDelay.value == SETTINGS_DEF2D_BASEDELAY
		&& settings->hsliderStepDelay.value == SETTINGS_DEF2D_STEPDELAY
		;
}

/*************************************************//**
 * Set the 3d-animation settings to their default values.
 *****************************************************
 */
static void gui_set_default3d_settings( Gui *gui )
{
	GuiSettings *settings = NULL;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	settings = &gui->settings;
	if ( !settings->chkbtnRollEffectWidget
	|| !settings->hsliderMaxSteps.widget || !settings->hsliderMaxSteps.adjustment
	|| !settings->hsliderBaseDelay.widget || !settings->hsliderBaseDelay.adjustment
	|| !settings->hsliderStepDelay.widget || !settings->hsliderStepDelay.adjustment
	){
		DBG_GUI_ERRMSG( gui->appWindow, _("Found invalid GUI elements") );
		return;
	}

	/* set default 3D values to the hsliders MaxSteps, BaseDelay & StepDelay */
	myGtk_horzSlider_set_value(
		&settings->hsliderMaxSteps,
		SETTINGS_DEF3D_MAXSPINS
		);
	myGtk_horzSlider_set_value(
		&settings->hsliderBaseDelay,
		SETTINGS_DEF3D_BASEDELAY
		);
	myGtk_horzSlider_set_value(
		&settings->hsliderStepDelay,
		SETTINGS_DEF3D_STEPDELAY
		);

}

/*************************************************//**
 * Set the 2d-animation settings to their default values.
 *****************************************************
 */
static void gui_set_default2d_settings( Gui *gui )
{
	GuiSettings *settings = NULL;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	settings = &gui->settings;
	if ( !settings->chkbtnRollEffectWidget
	|| !settings->hsliderMaxSteps.widget || !settings->hsliderMaxSteps.adjustment
	|| !settings->hsliderBaseDelay.widget || !settings->hsliderBaseDelay.adjustment
	|| !settings->hsliderStepDelay.widget || !settings->hsliderStepDelay.adjustment
	){
		DBG_GUI_ERRMSG( gui->appWindow, _("Found invalid GUI elements") );
		return;
	}

	/* set default 2D values to the hsliders MaxSteps, BaseDelay & StepDelay */
	myGtk_horzSlider_set_value(
		&settings->hsliderMaxSteps,
		SETTINGS_DEF2D_MAXSPINS
		);
	myGtk_horzSlider_set_value(
		&settings->hsliderBaseDelay,
		SETTINGS_DEF2D_BASEDELAY
		);
	myGtk_horzSlider_set_value(
		&settings->hsliderStepDelay,
		SETTINGS_DEF2D_STEPDELAY
		);

}

/*************************************************//**
 * Update the buttons "Default 3D" and "Default 2D"
 * according to the current state of the program.
 *
 *	For example, when the user enables the 3d-effect
 *	the button "Default 2D" gets disabled.
 *****************************************************
 */
static void gui_refresh_default3d2d_buttons( Gui *gui )
{
	GuiSettings *settings = NULL;
	gboolean isAnim2d = FALSE;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	settings = &gui->settings;
	if ( !settings->chkbtnRollEffectWidget
	|| !settings->hsliderMaxSteps.widget || !settings->hsliderMaxSteps.adjustment
	|| !settings->hsliderBaseDelay.widget || !settings->hsliderBaseDelay.adjustment
	|| !settings->hsliderStepDelay.widget || !settings->hsliderStepDelay.adjustment
	|| !settings->btnDefault3dWidget || !settings->btnDefault2dWidget
	){
		DBG_GUI_ERRMSG( gui->appWindow, _("Found invalid GUI elements") );
		return;
	}

	isAnim2d = gtk_toggle_button_get_active(
			GTK_TOGGLE_BUTTON(gui->settings.chkbtnRollEffectWidget)
			);
	gtk_widget_set_sensitive( gui->settings.btnDefault2dWidget, FALSE );
	gtk_widget_set_sensitive( gui->settings.btnDefault3dWidget, FALSE );
	if ( isAnim2d && !gui_has_default2d_settings(gui) ) {
		gtk_widget_set_sensitive( gui->settings.btnDefault2dWidget, TRUE );
	}
	else if ( !isAnim2d && !gui_has_default3d_settings(gui) ) {
		gtk_widget_set_sensitive( gui->settings.btnDefault3dWidget, TRUE );
	}
}

/*************************************************//**
 * Disable visually the specified menu-item and
 * enable all the others, in the Language menu.
 *****************************************************
 */
static gboolean gui_disable_langmenu_item_as_radio( Gui *gui, GtkWidget *menuItem )
{
	/* temp ptrs for better clarity & for saving us some typing later on */
	GtkWidget *en = NULL;
	GtkWidget *el = NULL;
	GtkWidget *envLang = NULL;

	/* sanity checks */
	if ( !gui || !menuItem ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui or menuItem)") );
		return FALSE;
	}
	if ( !gui->menu.itemEnglish ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->menu.itemEnglish)")
			);
		return FALSE;
	}
	if ( !gui->menu.itemGreek ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->menu.itemGreek)")
			);
		return FALSE;
	}
	if ( !gui->menu.itemEnvLang ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->menu.itemEnvLang)")
			);
		return FALSE;
	}

	/* just for brevity & better clarity */
	en	= gui->menu.itemEnglish;
	el	= gui->menu.itemGreek;
	envLang	= gui->menu.itemEnvLang;

	/* first enable all langmenu items */
	gtk_widget_set_sensitive(en, TRUE);
	gtk_widget_set_sensitive(el, TRUE);
	gtk_widget_set_sensitive(envLang, TRUE);

	/* then disable only the requested item */
	if ( menuItem == en ) {
		gtk_widget_set_sensitive(en, FALSE);
	}
	else if ( menuItem == el ) {
		gtk_widget_set_sensitive(el, FALSE);
	}
	else if ( menuItem == envLang ) {
		gtk_widget_set_sensitive(envLang, FALSE);
	}

	return TRUE;
}

/*************************************************//**
 * Callback function connected to the GTK+ "delete-event"
 * signal, for the main window of the application.
 *
 *	When returing FALSE on a "delete-event" signal, GTK+
 *	converts it to a "destroy" signal and re-emits it.
 *	Returning TRUE means we don't want the window to be
 *	destroyed, which may be useful if for example we want
 *	to ask for confirmation before destroying the window.
 *	Here we just return FALSE, thus telling GTK+ to
 *	immediately emit a "destroy" signal.
 *****************************************************
 */
static gboolean on_delete_event_appWindow(
	GtkWidget	*appWindow,
	GdkEvent	*event,
	Gui		*gui
	)
{
	/* avoid compiler warnings for unused parameters */
	if ( !appWindow || !event || !gui )
		return FALSE;

	return FALSE;
}

/*************************************************//**
 * Callback function connected to the "destroy" GTK+ signal,
 * for the main window of the application.
 *
 *	The "destroy" signal is emitted by a GTK+ window when
 *	the user closes the window, usually by clicking on its
 *	close icon (on Windows that's the 'x' icon, located at
 *	the top right corner of the window).
 *
 *	Normally we just want to terminate the GTK+ main-loop
 *	so the flow continues to our cleanup code (if any),
 *	in the main() function and then terminate the program.
 *
 *	However, in this program a signal for destroying the
 *	main application window does not necessarily mean that
 *	the user has requested program temination.
 *
 *	It could be that the signal has been "artificially"
 *	produced by the program itself, when the user requested
 *	a dynamic change of the GUI language, right after
 *	changing the value of the environment-variable LANG.
 *
 *	In those cases, the program schedules the destruction
 *	of all its top-level windows (thus its main window too)
 *	inside the function: gui_reload_gtkGladeFile(), by
 *	calling gtk_widget_destroy() on all of them.
 *
 *	Once this is done, the GUI resources are re-loaded from
 *	the Glade file, which means that they get re-initialized
 *	according to the new value of LANG, due to GNU-gettext.
 *
 *	So obviously, a "destroy" signal sent by the main window
 *	of the application does not always dictate program termination.
 *
 *	To distingusigh whether a "destroy" signal should terminate
 *	the main loop of GTK+ or not, I use the boolean variable:
 *	gui->quitOnDestroyAppWindow. If it is TRUE gtk_main_quit()
 *	is called, otherwise the GTK+ main-loop keeps going on.
 *
 *	See also:
 *		gui_reload_gtkGladeFile(), on_activate_miEnglish(),
 *		on_activate_miGreek() and on_activate_miEnvLang().
 *****************************************************
 */
static void on_destroy_appWindow( GtkWidget *appWindow, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !appWindow ) {
		gtk_main_quit();
	}

	if ( gui && gui->quitOnDestroyAppWindow ) {
		gtk_main_quit();
	}
}

/*************************************************//**
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Language->English".
 *
 *	The menu-item is visualy disabled, LANG is set
 *	to "en" and the whole GUI is reloaded in order
 *	to reflect the change of the language.
 *
 *	NOTE:	To better understand how this works,
 *		see also: gui_reload_gtkGladeFile().
 *****************************************************
 */
static void on_activate_miEnglish( GtkWidget *menuItem, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !menuItem )
		return;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	if ( !gui->appWindow ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->appWindow)")
			);
		return;
	}

	/* set English locale environment, reload GUI resources, update menu-entries */

	set_locale_environment(
		STR_EN_LOCALE,
		STR_EN_LOCALE_LANG,
		STR_EN_LOCALE_LANGUAGE
		);
	gui_reload_gtkGladeFile( gui, FNAME_GLADE );
	gui_disable_langmenu_item_as_radio( gui, gui->menu.itemEnglish );
	gtk_widget_show_all( gui->appWindow );

	dbg_print_locale_environment( "Locale Environmet changed to English:" );

	return;
}

/*************************************************//**
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Language->Greek".
 *
 *	The menu-item is visualy disabled, LANG is set
 *	to "el" and the whole GUI is reloaded in order
 *	to reflect the change of the language.
 *
 *	NOTE:	To better understand how this works,
 *		see also: gui_reload_gtkGladeFile().
 *****************************************************
 */
static void on_activate_miGreek( GtkWidget *menuItem, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !menuItem )
		return;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	if ( !gui->appWindow ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->appWindow)")
			);
		return;
	}


	/* set Greek locale environment, reload GUI resources, update menu-entries */

	set_locale_environment(
		STR_EL_LOCALE,
		STR_EL_LOCALE_LANG,
		STR_EL_LOCALE_LANGUAGE
		);
	gui_reload_gtkGladeFile(gui, FNAME_GLADE);
	gui_disable_langmenu_item_as_radio( gui, gui->menu.itemGreek );
	gtk_widget_show_all( gui->appWindow );

	dbg_print_locale_environment( "Locale Environmet changed to Greek:" );

	return;
}

/*************************************************//**
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Language->use Environment".
 *
 *	The menu-item is visualy disabled, LANG is set to
 *	the value saved in gui->envLang (when the program
 *	started), and the whole GUI is reloaded in order
 *	to reflect the change of the language.
 *
 *	NOTE:	If the saved value of LANG was NULL then
 *		we set it to "en" and we inform the user.
 *
 *		If the saved value of LANG defines a
 *		language with no translation available
 *		then GNU-gettext ensures that it will
 *		fallback to the defualt language, which
 *		is English.
 *
 *		In that case the user is not directly
 *		informed, but he gets a visual hint
 *		because the menu-item stays disabled
 *		inside the menu.
 *
 *		To better understand how this works,
 *		see also: gui_reload_gtkGladeFile().
 *****************************************************
 */
static void on_activate_miEnvLang( GtkWidget *menuItem, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !menuItem )
		return;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	if ( !gui->appWindow ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid GUI element (gui->appWindow)") );
		return;
	}

	if ( !gui->callerLocaleEnv.locale
	|| !gui->callerLocaleEnv.varLang || !gui->callerLocaleEnv.varLanguage
	){
		set_locale_environment(
			STR_EN_LOCALE,
			STR_EN_LOCALE_LANG,
			STR_EN_LOCALE_LANGUAGE
			);
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("No locale environment was found.\n\
ENGLISH is used as fallback language.")
		);
	}
	else {
		set_locale_environment(
			gui->callerLocaleEnv.locale,
			gui->callerLocaleEnv.varLang,
			gui->callerLocaleEnv.varLanguage
			);
	}

	gui_reload_gtkGladeFile(gui, FNAME_GLADE);

	gui_disable_langmenu_item_as_radio( gui, gui->menu.itemEnvLang );
	gtk_widget_show_all( gui->appWindow );

	dbg_print_locale_environment( "Locale Environmet changed to System's:" );

	return;
}

/*************************************************//**
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Menu->About".
 *
 *	Displays the "About" dialog, which is defined
 *	as a GtkAboutDialog window in the Glade file.
 *
 *	This makes it easier to present the desired
 *	information in a standardized manner, but I
 *	haven't found yet an easy way to access the
 *	members of the dialog's action-area, that is
 *	the buttons "Credits", "License" and "Close".
 *
 *	Perhaps someday I dig deeper into the internals
 *	of the GtkAboutDialog type, but right now I see
 *	no easy way to mark the labels of those buttons
 *	as translatable by GNU-gettext.
 *
 *	Furthermore, when running on Win32 OSes, GTK+
 *	has trouble registering properly some common
 *	hyperlink-protocols such as "http://" & "mailto:".
 *	The problem is that the About dialog contains
 *	quite a few of them.
 *
 *	To solve this problem, I overwrite GTK+'s default
 *	behaviour for links in this dialog, by connecting
 *	the callback function: on_activate_link_dlgAbout()
 *	to the "activate-link" signals emitted by the dialog.
 *
 *	The callback function is used only when Win32 is
 *	the compilation target, regulated by a relative
 *	pre-processor directive.
 *	For more info, please see the functions:
 *	gui_init_dlgAbout() and on_activate_link_dlgAbout()
 *****************************************************
 */
static void on_activate_miAbout( GtkWidget *menuItem, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !menuItem )
		return;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	if ( !gui->appWindow || !gui->dlgAbout ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->appWindow or gui->dlgAbout)")
			);
		return;
	}

	gtk_widget_set_sensitive( gui->appWindow, FALSE );

	gtk_dialog_run( GTK_DIALOG( gui->dlgAbout ) );
	gtk_widget_hide( GTK_WIDGET(gui->dlgAbout) );

	gtk_widget_set_sensitive( gui->appWindow, TRUE );

	return;
}

/*************************************************//**
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Menu->Quit".
 *
 *	When the user selects this menu-item we want to
 *	terminate the program, so we first set to TRUE
 *	the boolean variable: gui->quitOnDestroyAppWindow,
 *	and then we call the same callback function we use
 *	when the "destroy" signal is sent by the main window
 *	of our program.
 *
 *	For details about gui->quitOnDestroyAppWindow, please
 *	read the comments of the function: on_destroy_appWindow()
 *****************************************************
 */
static void on_activate_miQuit( GtkWidget *menuItem, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !menuItem )
		return;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}

	gui->quitOnDestroyAppWindow = TRUE;
	on_destroy_appWindow(gui->appWindow, gui);

	return;
}

/*************************************************//**
 * Callback function connected to the GTK+ "clicked" signal,
 * for the button "Roll".
 *
 *	This function implements the animation of the die. It is
 *	called when the user clicks on the button "Roll" or anywhere
 *	inside the event-box that hosts the animation's image-widget.
 *
 *	The basic idea is to have a loop of user-defined 'maxSteps'
 *	iterations. Within each iteration we produce a random integer
 *	from 1 to 6, we load the corresponding image into the image-widget
 *	and we redraw the widget. Then we add some delay (sleep) until
 *	the next iteration. The last generated random integer is the
 *	final result of the whole roll.
 *
 *	However, the code gets much more involved, because we
 *	implement enhanced functionality compared to the basic
 *	one described above.
 *
 *	Besides 'maxSteps', we also let the user to decide the
 *	'baseDelay' to be applied between iterations (steps),
 *	along with an additional 'stepDelay' that gets added
 *	to the 'baseDelay' after every loop iteration, thus
 *	producing a progressive slow-down to the animation
 *	which can be thought as a naive friction-effect.
 *
 *	We also let the user decide whether we should use
 *	flat 2d or (pseudo) 3d images for the animation-effect.
 *	However, first & last frames are always flat 2d images.
 *
 *	I've put quite a few comments inside the function, hoping
 *	they will make it easier to follow the code. Let me add
 *	though that, since this is not a true animation, the term
 *	"Frames" is used in a rough context.
 *
 *	A complete 3d effect uses NFRAMES_3D images (currently 32)
 *	while a complete 2d effect uses NFRAMES_2D images (currently 6).
 *	So, depending on the current value of the "Use 2D rolling-effect"
 *	setting, if 'maxSteps' exceeds the frames limit, the animation
 *	reverses its direction as soon as it reaches the last frame.
 *	When reversed, if it reaches the first frame it changes its
 *	direction again, and so on.
 *****************************************************
 */
static void on_clicked_btnRoll( GtkWidget *button, Gui *gui )
{
	gchar *fmtText = NULL;		/* for creating formatted text via snprintf() */
	gchar msgStatusbar[MAXSIZ_FNAME] = {'\0'};	/* msg displayed on status-bar */
	gchar fnameFrame[MAXSIZ_FNAME] = {'\0'};	/* next image-frame to load */
	gboolean isAnim2d = FALSE;	/* are we currently in 2d or 3d animation mode? */
	gboolean hasNextFrame = TRUE;	/* are images of current animation exhausted? */
	gint maxFrames;			/* 3d uses NFRAMES_3D images, 2d uses NFRAMES_2D */
	gint i = 0, iFrame = 0;		/* current step & image-frame during animation */
	gint maxSteps = 32;		/* max number of moving-steps per roll */
	gulong baseDelay = 500;		/* standard delay between steps (microsecs) */
	gulong stepDelay = 500;		/* step for progressive friction (microsecs)*/
	GuiSettings *settings = NULL;	/* just to save us some typing later on */

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	if ( !gui->linkToCoreData ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("GUI is not linked to Core") );
		return;
	}
	settings = &gui->settings;
	if ( !settings->chkbtnDebugInfoWidget || !settings->chkbtnRollEffectWidget
	|| !settings->hsliderMaxSteps.widget || !settings->hsliderMaxSteps.adjustment
	|| !settings->hsliderBaseDelay.widget || !settings->hsliderBaseDelay.adjustment
	|| !settings->hsliderStepDelay.widget || !settings->hsliderStepDelay.adjustment
	){
		DBG_GUI_ERRMSG( gui->appWindow, _("Found invalid GUI elements") );
		return;
	}

	dbg_print_info( "Roll button clicked\n" );

	/* get current animation settings from GUI */
	maxSteps  = (gint)settings->hsliderMaxSteps.value;
	baseDelay = (gulong)settings->hsliderBaseDelay.value;
	stepDelay = (gulong)settings->hsliderStepDelay.value;
	isAnim2d  = gtk_toggle_button_get_active(
			GTK_TOGGLE_BUTTON(settings->chkbtnRollEffectWidget)
			);

	/* depending on the current 2d/3d mode, init max number of image-frames
	 * for the animation, and decide on the filename template to be used for
	 * loading image-frames during the animation
	 */
	maxFrames = isAnim2d ? NFRAMES_2D : NFRAMES_3D;
	fmtText	  = isAnim2d ? "%s2d_%02d.png" : "%s3d_%02d.png";

	/* clear statusbar, disable roll-button, update button's text */
	gtk_widget_set_sensitive( gui->statusBar.widget, FALSE );
	gtk_statusbar_pop( GTK_STATUSBAR(gui->statusBar.widget), gui->statusBar.contextId );
	gtk_widget_set_sensitive( button, FALSE );
	gtk_button_set_label( GTK_BUTTON(button), TXT_BUTTON_ROLLING );
	myGtk_widget_refresh( button );

	/* main loop of the animation effect
	 * NOTE:iFrame starts with 1 instead of 0, because the first image-frame
	 *	is already displayed (when GUI was initially loaded from the
	 *	Glade file)
	 */
	iFrame = 1;
	for (i=0; i < maxSteps; i++)
	{
		/* show progress in status-bar */
		g_snprintf(
			msgStatusbar,
			MAXSIZ_FNAME,
			TXTF_STATUSBAR_ONROLL,
			i,		/* current step */
			iFrame,		/* current image-frame */
			baseDelay	/* current base-delay + friction */
			);
		gtk_statusbar_push(
			GTK_STATUSBAR(gui->statusBar.widget),
			gui->statusBar.contextId,
			msgStatusbar
			);

		/* produce & save a random integer between 1 and 6, inclusive
		 * and then depending on the currently displayed image-frame
		 * decide whether the animation should change its direction
		 */
		gui->linkToCoreData->resultRolled =
			g_rand_int_range( gui->linkToCoreData->randGen, 1, 7 );
		if ( iFrame > maxFrames-1 )
			hasNextFrame = FALSE;
		if ( iFrame < 2 )
			hasNextFrame = TRUE;
		iFrame = hasNextFrame ? iFrame+1 : iFrame-1;

		/* construct the filename of the next image-frame to be displayed,
		 * load it into the GUI image-widget & refresh the widget
		 */
		g_snprintf( fnameFrame, MAXSIZ_FNAME, fmtText, DIR_GUI, iFrame );
		dbg_print_info( "Frame Image: %s\n", fnameFrame );
		gtk_image_set_from_file(
			GTK_IMAGE(gui->dieArea.imgWidget), fnameFrame
			);
		myGtk_widget_refresh( gui->dieArea.imgWidget );

		/* apply baseDelay + friction until next step */
		g_usleep( baseDelay );
		baseDelay += stepDelay;

		/* clear the status-bar */
		gtk_statusbar_pop(
			GTK_STATUSBAR(gui->statusBar.widget),
			gui->statusBar.contextId
			);
	}
	/* display the final image-frame which is always a 2d image,
	 * reflecting the last random number picked inside the loop
	 */
	g_snprintf(
		fnameFrame,
		MAXSIZ_FNAME,
		"%s2d_%02d.png",
		DIR_GUI,
		gui->linkToCoreData->resultRolled
		);
	gtk_image_set_from_file( GTK_IMAGE(gui->dieArea.imgWidget), fnameFrame );
	myGtk_widget_refresh( gui->dieArea.imgWidget );

	/* enable roll-button & update its text */
	gtk_widget_set_sensitive( button, TRUE );
	gtk_button_set_label( GTK_BUTTON(button), TXT_BUTTON_ROLL2D );

	/* display in the status-bar the last generated random number */
	gtk_widget_set_sensitive( gui->statusBar.widget, TRUE );
	g_snprintf(
		msgStatusbar,
		MAXSIZ_FNAME,
		TXTF_STATUSBAR_RESULT,
		gui->linkToCoreData->resultRolled
		);
	gtk_statusbar_push(
		GTK_STATUSBAR(gui->statusBar.widget),
		gui->statusBar.contextId,
		msgStatusbar
		);

	return;
}

#ifdef G_OS_WIN32
/*************************************************//**
 * Callback function connected to the GTK+ "activate-link"
 * signal, for the dialog About.
 *
 *	When run on Win32, GTK+ (Gio) cannot open links,
 *	thus we overwrite the default behaviour by using
 *	our own Win32 specific callback function.
 *****************************************************
 */
static gboolean on_activate_link_dlgAbout(
	GtkWidget	*dlgAbout,
	gchar		*uri,
	Gui		*gui
	)
{
	/* avoid compiler warnings for unused parameters */
	if ( !dlgAbout || !uri )
		return FALSE;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !gui->dlgAbout ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Found invalid GUI elements") );
		return FALSE;
	}

	/* Win32 API call */
	ShellExecute( NULL, "open", uri, NULL, NULL, SW_SHOWNORMAL );

	dbg_print_info( "Signal \"activate-link\" sent for uri: %s\n", uri );


	/* Returning TRUE means we handled the event, so the signal
	* emission should be stopped (don't call any further
	* callbacks that may be connected). Return FALSE
	* to continue invoking callbacks.
	*/
	return TRUE;
}
#endif	/* #ifdef G_OS_WIN32 */

/*************************************************//**
 * Callback function connected to the GTK+ "button_press_event"
 * signal, for the event-box that hosts the animation's image-widget.
 *
 *	The image-widget is updated by sequentially loading different
 *	images into it during the animation. However, GTK+ image-widgets
 *	are not clickable. In order to catch user-clicks on the image-widget,
 *	we make it a child of an event-box, because event-boxes emit
 *	the "button_press_event" signal.
 *****************************************************
 */
static gboolean on_button_press_event_evboxImg(
	GtkWidget	*eventBox,
	GdkEventButton	*event,
	Gui		*gui
	)
{
	/* avoid compiler warnings for unused parameters */
	if ( !eventBox || !event )
		return FALSE;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !gui->dieArea.btnWidget ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Found invalid GUI elements") );
		return FALSE;
	}

	dbg_print_info( "EventBox2d clicked at %f,%f\n", event->x, event->y );

	/* we can either emit a "clicked" signal for the Roll button ... */
	g_signal_emit_by_name( gui->dieArea.btnWidget, "clicked" );
	/* ... or we can directly call the callback function attached to the button
	 on_clicked_btnRoll( gui->die.btnWidget, gui);
	 */

	/* Returning TRUE means we handled the event, so the signal
	* emission should be stopped (don't call any further
	* callbacks that may be connected). Return FALSE
	* to continue invoking callbacks.
	*/
	return TRUE;
}

#if DISABLED
/*************************************************//**
 * ** UNUSED **
 * Callback function connected to the GTK+ "button_press_event"
 * signal, for the event-box that hosts the 3d-animation-area.
 *
 *	This function is not used in the program (it's a
 *	left-over from previous versions). I keep it here
 *	for some future experimentation.
 *****************************************************
 */
static gboolean on_button_press_event_evboxImg3d(
	GtkWidget	*eventBox,
	GdkEventButton	*event,
	Gui		*gui
	)
{
	char tmpString[MAXSIZ_FNAME] = {'\0'};
	gint i = 0;
	gulong delay = 2500;		/* standard delay between shuffles (microsecs) */
	const gulong delayStep = 5000;	/* step for progressive delay (microsecs) */

	/* avoid compiler warnings for unused parameters */
	if ( !eventBox || !event )
		return FALSE;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !gui->dieArea3d.imgWidget ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Found invalid GUI elements") );
		return FALSE;
	}
	dbg_print_info( "EventBox3d clicked at %f,%f\n", event->x, event->y );
/*	on_clicked_btnRoll( gui->die.btnWidget, gui);
*/
	for (i=1; i <= 32; i++)
	{
		g_snprintf( tmpString, MAXSIZ_FNAME,"%s3d_%02d.png", DIR_GUI, i );
		dbg_print_info( "%s\n", tmpString );
		gtk_image_set_from_file(
			GTK_IMAGE(gui->dieArea3d.imgWidget),
			tmpString
			);
		myGtk_widget_refresh( gui->dieArea3d.imgWidget );
		g_usleep( delay );
		delay += delayStep;
	}

	/* Returning TRUE means we handled the event, so the signal
	* emission should be stopped (don't call any further
	* callbacks that may be connected). Return FALSE
	* to continue invoking callbacks.
	*/
	return TRUE;
}
#endif	/* #if DISABLED */

/*************************************************//**
 * Callback function connected to the GTK+ "toggled"
 * signal, for the check-button "Debuging info".
 *
 *	This button toggles between verbose and normal
 *	output of the program to the stdout & stderr
 *	streams (usually those are both the console).
 *****************************************************
 */
static void on_toggled_chkbtnDebugInfo(
	GtkToggleButton	*chkbutton,
	Gui		*gui
	)
{
	/* avoid compiler warning for unused parameter */
	if ( !chkbutton )
		return;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	if ( !gui->settings.chkbtnDebugInfoWidget ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("check-button has not been properly created")
			);
		return;
	}

	/* */
	global_debugOn = gtk_toggle_button_get_active(
				GTK_TOGGLE_BUTTON(gui->settings.chkbtnDebugInfoWidget)
			);

	g_print( "Debug mode: %s\n", global_debugOn ? "ON" : "OFF" );

	return;
}

/*************************************************//**
 * Callback function connected to the GTK+ "toggled"
 * signal, for the check-button "Use 2D rolling-effect".
 *
 *	This button toggles between the 2D and the 3D
 *	rolling-effect of the die-animation.
 *****************************************************
 */
static void on_toggled_chkbtnUse2dRollEffect(
	GtkToggleButton	*chkbutton,
	Gui		*gui
	)
{
	gboolean isAnim2d = FALSE;

	/* avoid compiler warning for unused parameter */
	if ( !chkbutton )
		return;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	if ( !gui->settings.chkbtnRollEffectWidget ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("check-button has not been properly created")
			);
		return;
	}

	/* */
	isAnim2d = gtk_toggle_button_get_active(
			GTK_TOGGLE_BUTTON(gui->settings.chkbtnRollEffectWidget)
			);
	dbg_print_info( "2D-effect: %s\n", isAnim2d ? "ON" : "OFF" );

	gui_refresh_default3d2d_buttons(gui);

	return;
}

/*************************************************//**
 * Callback function connected to the GTK+ "value_changed"
 * signal, for the GtkRange attached to the horizontal
 * slider "MaxSteps".
 *****************************************************
 */
static void on_value_changed_hsliderMaxSteps(
	GtkRange	*range,
	Gui		*gui
	)
{
	GuiHorzSlider *hslider = NULL;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	hslider = &gui->settings.hsliderMaxSteps;
	if ( !hslider->widget || !hslider->adjustment ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("slider has not been properly created")
			);
		return;
	}

	/* */
	hslider->value = gtk_range_get_value(range);
	dbg_print_info( "MaxSteps  slider new value: %f\n", hslider->value );

	gui_refresh_default3d2d_buttons(gui);

	return;
}

/*************************************************//**
 * Callback function connected to the GTK+ "value_changed"
 * signal, for the GtkRange attached to the horizontal
 * slider "BaseDelay".
 *****************************************************
 */
static void on_value_changed_hsliderBaseDelay(
	GtkRange	*range,
	Gui		*gui
	)
{
	GuiHorzSlider *hslider = NULL;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	hslider = &gui->settings.hsliderBaseDelay;
	if ( !hslider->widget || !hslider->adjustment ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("slider has not been properly created")
			);
		return;
	}

	/* */
	hslider->value = gtk_range_get_value(range);
	dbg_print_info( "BaseDelay slider new value: %f\n", hslider->value );

	gui_refresh_default3d2d_buttons(gui);

	return;
}

/*************************************************//**
 * Callback function connected to the GTK+ "value_changed"
 * signal, for the GtkRange attached to the horizontal
 * slider "Friction (StepDelay)".
 *****************************************************
 */
static void on_value_changed_hsliderStepDelay(
	GtkRange	*range,
	Gui		*gui
	)
{
	GuiHorzSlider *hslider = NULL;

	/* avoid compiler warning for unused parameter */
	if ( !range )
		return;

	/* sanity check */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	hslider = &gui->settings.hsliderStepDelay;
	if ( !hslider->widget || !hslider->adjustment ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("slider has not been properly created")
			);
		return;
	}

	/* */
	hslider->value = gtk_range_get_value(range);
	dbg_print_info( "StepDelay slider new value: %f\n", hslider->value );

	gui_refresh_default3d2d_buttons(gui);

	return;
}

/*************************************************//**
 * Callback function connected to the GTK+ "clicked"
 * signal, for the button "Default 3D".
 *
 *	If 3d-rolling-effect is active, then animation
 *	settings are reset to their default 3d values.
 *****************************************************
 */
static void on_clicked_btnSettingsDefault3d(
	GtkWidget	*button,
	Gui		*gui
	)
{
	/* avoid compiler warning for unused parameter */
	if ( !button )
		return;

	/* sanity check */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}

	if ( !gui_has_default3d_settings(gui) )
	{
		gui_set_default3d_settings( gui );
		gtk_widget_set_sensitive(
			gui->settings.btnDefault3dWidget,
			FALSE
			);
	}
}

/*************************************************//**
 * Callback function connected to the GTK+ "clicked"
 * signal, for the button "Default 2D".
 *
 *	If 2d-rolling-effect is active, then animation
 *	settings are reset to their default 2d values.
 *****************************************************
 */
static void on_clicked_btnSettingsDefault2d(
	GtkWidget	*button,
	Gui		*gui
	)
{
	/* avoid compiler warning for unused parameter */
	if ( !button )
		return;

	/* sanity check */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}

	if ( !gui_has_default2d_settings(gui) )
	{
		gui_set_default2d_settings( gui );
		gtk_widget_set_sensitive(
			gui->settings.btnDefault2dWidget,
			FALSE
			);
	}
}

/*************************************************//**
 * Initialize the button "Default 2D".
 *
 *	Copies the GUI button from the specified GTK+ builder into
 *	my GUI settings abstraction: gui->settings, it connects
 *	callback functions to it and initializes its visual appearance.
 *
 *	NOTE:	The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the widget pointer will be set to NULL
 *		after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_settings_btnDefault2d( Gui *gui, GtkBuilder *builder )
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	gui->settings.btnDefault2dWidget =
		GTK_WIDGET(
			gtk_builder_get_object(builder, "btnSettingsDefault2d")
		);
	g_signal_connect(
		G_OBJECT(gui->settings.btnDefault2dWidget),
		"clicked",
		G_CALLBACK(on_clicked_btnSettingsDefault2d),
		gui
		);
	g_signal_connect(
		G_OBJECT(gui->settings.btnDefault2dWidget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.btnDefault2dWidget
		);
	gtk_widget_set_sensitive( gui->settings.btnDefault2dWidget, FALSE );

	return TRUE;
}

/*************************************************//**
 * Initialize the button "Default 3D".
 *
 *	Copies the GUI button from the specified GTK+ builder into
 *	my GUI settings abstraction: gui->settings, it connects
 *	callback functions to it and initializes its visual appearance.
 *
 *	NOTE:	The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the widget pointer will be set to NULL
 *		after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_settings_btnDefault3d( Gui *gui, GtkBuilder *builder )
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	gui->settings.btnDefault3dWidget =
		GTK_WIDGET(
			gtk_builder_get_object(builder, "btnSettingsDefault3d")
		);
	g_signal_connect(
		G_OBJECT(gui->settings.btnDefault3dWidget),
		"clicked",
		G_CALLBACK(on_clicked_btnSettingsDefault3d),
		gui
		);
	g_signal_connect(
		G_OBJECT(gui->settings.btnDefault3dWidget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.btnDefault3dWidget
		);
	gtk_widget_set_sensitive( gui->settings.btnDefault3dWidget, FALSE );

	return TRUE;
}

/*************************************************//**
 * Initialize the horizontal slider "StepDelay".
 *
 *	Copies the GUI slider from the specified GTK+ builder into
 *	my GUI settings abstraction: gui->settings, it connects
 *	callback functions to it and initializes its visual appearance.
 *
 *	My gui->settings.hsliderXxx abstraction (GuiHorzSlider) consists
 *	of 2 GTK+ graphical elements (an hscale widget and an adjustment)
 *	and some other supporting fields. We get the former from the
 *	builder and we initialize & apply the latter.
 *
 *	NOTE:	The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the widget pointer will be set to NULL
 *		after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_settings_hsliderStepDelay(
	Gui		*gui,
	GtkBuilder	*builder
	)
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	/* label */
	gui->settings.labelStepDelayWidget =
		GTK_WIDGET(
			gtk_builder_get_object(builder, "labelSettingsStepDelay")
		);
	g_signal_connect(
		G_OBJECT(gui->settings.labelStepDelayWidget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.labelStepDelayWidget
		);

	/* hscale */
	gui->settings.hsliderStepDelay.widget =
		GTK_WIDGET(
			gtk_builder_get_object(builder, "hscaleFrameSettingsStepDelay")
		);
	g_signal_connect(
		G_OBJECT(gui->settings.hsliderStepDelay.widget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.hsliderStepDelay.widget
		);

	/* adjustment */
	gui->settings.hsliderStepDelay.adjustment =
		GTK_ADJUSTMENT(
			gtk_builder_get_object(builder, "adjustHscaleSettingsStepDelay")
		);
	g_signal_connect(
		G_OBJECT(gui->settings.hsliderStepDelay.adjustment),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.hsliderStepDelay.adjustment
		);

	myGtk_horzSlider_set(
		&gui->settings.hsliderStepDelay,
		SETTINGS_DEF3D_STEPDELAY,	/* value */
		0,				/* minVal */
		30000,				/* maxVal */
		10,				/* stepInc */
		100,				/* pageInc */
		0				/* digits */
		);

	g_signal_connect(
		G_OBJECT(gui->settings.hsliderStepDelay.widget),
		"value-changed",
		G_CALLBACK(on_value_changed_hsliderStepDelay),
		gui
	);

	return TRUE;
}

/*************************************************//**
 * Initialize the horizontal slider "Friction (StepDelay)".
 *
 *	Copies the GUI slider from the specified GTK+ builder into
 *	my GUI settings abstraction, it connects callback functions
 *	to it and initializes its visual appearance.
 *
 *	My gui->settings.hsliderXxx abstraction (GuiHorzSlider) consists
 *	of 2 GTK+ graphical elements (an hscale widget and an adjustment)
 *	and some other supporting fields. We get the former from the
 *	builder and we initialize & apply the latter.
 *
 *	NOTE:	The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the widget pointer will be set to NULL
 *		after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_settings_hsliderBaseDelay(
	Gui		*gui,
	GtkBuilder	*builder
	)
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	/* label */
	gui->settings.labelBaseDelayWidget =
		GTK_WIDGET(
			gtk_builder_get_object(builder, "labelSettingsBaseDelay")
		);
	g_signal_connect(
		G_OBJECT(gui->settings.labelBaseDelayWidget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.labelBaseDelayWidget
		);

	/* hscale */
	gui->settings.hsliderBaseDelay.widget =
		GTK_WIDGET(
			gtk_builder_get_object(builder, "hscaleFrameSettingsBaseDelay")
		);
	g_signal_connect(
		G_OBJECT(gui->settings.hsliderBaseDelay.widget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.hsliderBaseDelay.widget
		);

	/* adjustment */
	gui->settings.hsliderBaseDelay.adjustment =
		GTK_ADJUSTMENT(
			gtk_builder_get_object(builder, "adjustHscaleSettingsBaseDelay")
		);
	g_signal_connect(
		G_OBJECT(gui->settings.hsliderBaseDelay.adjustment),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.hsliderBaseDelay.adjustment
		);

	myGtk_horzSlider_set(
		&gui->settings.hsliderBaseDelay,
		SETTINGS_DEF3D_BASEDELAY,	/* value */
		300,				/* minVal */
		64000,				/* maxVal */
		100,				/* stepInc */
		1000,				/* pageInc */
		0				/* digits */
		);

	g_signal_connect(
		G_OBJECT(gui->settings.hsliderBaseDelay.widget),
		"value-changed",
		G_CALLBACK(on_value_changed_hsliderBaseDelay),
		gui
	);

	return TRUE;
}

/*************************************************//**
 * Initialize the horizontal slider "MaxSteps".
 *
 *	Copies the GUI slider from the specified GTK+ builder into
 *	my GUI settings abstraction, it connects callback functions
 *	to it and initializes its visual appearance.
 *
 *	My gui->settings.hsliderXxx abstraction (GuiHorzSlider) consists
 *	of 2 GTK+ graphical elements (an hscale widget and an adjustment)
 *	and some other supporting fields. We get the former from the
 *	builder and we initialize & apply the latter.
 *
 *	NOTE:	The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the widget pointer will be set to NULL
 *		after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_settings_hsliderMaxSteps(
	Gui		*gui,
	GtkBuilder	*builder
	)
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	gui->settings.labelMaxStepsWidget =
		GTK_WIDGET(
			gtk_builder_get_object(builder, "labelSettingsMaxSteps")
		);
	g_signal_connect(
		G_OBJECT(gui->settings.labelMaxStepsWidget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.labelMaxStepsWidget
		);

	gui->settings.hsliderMaxSteps.widget =
		GTK_WIDGET(
			gtk_builder_get_object(builder, "hscaleFrameSettingsMaxSpins")
		);
	g_signal_connect(
		G_OBJECT(gui->settings.hsliderMaxSteps.widget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.hsliderMaxSteps.widget
		);

	gui->settings.hsliderMaxSteps.adjustment =
		GTK_ADJUSTMENT(
			gtk_builder_get_object(builder, "adjustHscaleSettingsMaxSpins")
		);
	g_signal_connect(
		G_OBJECT(gui->settings.hsliderMaxSteps.adjustment),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.hsliderMaxSteps.adjustment
		);

	myGtk_horzSlider_set(
		&gui->settings.hsliderMaxSteps,
		SETTINGS_DEF3D_MAXSPINS,	/* value */
		12,				/* minVal */
		128,				/* maxVal */
		1,				/* stepInc */
		8,				/* pageInc */
		0				/* digits */
		);

	g_signal_connect(
		G_OBJECT(gui->settings.hsliderMaxSteps.widget),
		"value-changed",
		G_CALLBACK(on_value_changed_hsliderMaxSteps),
		gui
	);

	return TRUE;
}

/*************************************************//**
 * Initialize the check-button "Use 2D rolling-effect".
 *
 *	Copies the GUI check-button from the specified GTK+
 *	builder into my GUI settings abstraction: gui->settings,
 *	it connects callback functions to it and initializes its
 *	visual appearance.
 *
 *	NOTE:	The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the widget pointer will be set to NULL
 *		after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_settings_chkbtnUse2dRollEffect(
	Gui		*gui,
	GtkBuilder	*builder
	)
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	gui->settings.chkbtnRollEffectWidget =
		GTK_WIDGET(
			gtk_builder_get_object(builder, "chkbtnSettingsRollEffect")
		);

	gtk_toggle_button_set_active(
		GTK_TOGGLE_BUTTON(gui->settings.chkbtnRollEffectWidget),
		FALSE
		);

	g_signal_connect(
		G_OBJECT(gui->settings.chkbtnRollEffectWidget),
		"toggled",
		G_CALLBACK(on_toggled_chkbtnUse2dRollEffect),
		gui
		);

	g_signal_connect(
		G_OBJECT(gui->settings.chkbtnRollEffectWidget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.chkbtnRollEffectWidget
		);

	return TRUE;
}

/*************************************************//**
 * Initialize the check-button "Debugging mode".
 *
 *	Copies the GUI check-button from the specified GTK+
 *	builder into my GUI settings abstraction: gui->settings,
 *	it connects callback functions to it and initializes its
 *	visual appearance.
 *
 *	NOTE:	The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the widget pointer will be set to NULL
 *		after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_settings_chkbtnDebugInfo(
	Gui		*gui,
	GtkBuilder	*builder
	)
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	gui->settings.chkbtnDebugInfoWidget =
		GTK_WIDGET(
			gtk_builder_get_object(builder, "chkbtnSettingsDebugInfo")
		);

	gtk_toggle_button_set_active(
		GTK_TOGGLE_BUTTON(gui->settings.chkbtnDebugInfoWidget),
		global_debugOn
		);

	g_signal_connect(
		G_OBJECT(gui->settings.chkbtnDebugInfoWidget),
		"toggled",
		G_CALLBACK(on_toggled_chkbtnDebugInfo),
		gui
		);

	g_signal_connect(
		G_OBJECT(gui->settings.chkbtnDebugInfoWidget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->settings.chkbtnDebugInfoWidget
		);

	return TRUE;
}

/*************************************************//**
 * NOTE *
 *	This function used to init the 3d-animation area, in
 *	previous versions of the program. Since there is now
 *	just one animation-area for both 2d and 3d animations,
 *	here I'm just loading the old 3d-area (that is the
 *	"vbox3d" container) and then I destroy it. This also
 *	destroys all its children-widgets.
 *	I'm doing so because I want to keep the "vbox3d"
 *	container defined inside the glade-file, for some
 *	future experimentation.
 *****************************************************
 */
static gboolean gui_init_dieArea3d( Gui *gui, GtkBuilder *builder )
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	/* NEWER CODE:
	   delete the "vbox3d" container & all its children */
	gui->dieArea3d.vbox3d =
		GTK_WIDGET( gtk_builder_get_object(builder, "vbox3d") );
	gtk_widget_destroy( gui->dieArea3d.vbox3d );

	/* OLDER CODE
	gui->dieArea3d.evboxWidget =
		GTK_WIDGET( gtk_builder_get_object(builder, "evboxImg3d") );

	gui->dieArea3d.imgWidget =
		GTK_WIDGET( gtk_builder_get_object(builder, "image3d") );

	g_signal_connect(
		G_OBJECT(gui->dieArea3d.evboxWidget),
		"button_press_event",
		G_CALLBACK(on_button_press_event_evboxImg3d),
		gui
	);
	*/

	return TRUE;
}

/*************************************************//**
 * Initialize the "DieArea" of the GUI.
 *
 *	The "DieArea" is a custom GUI abstraction consisting of
 *	three GTK+ widgets: an event-box, an image and a button.
 *
 *	This function copies those widgets from the specified GTK+
 *	builder into my GUI abstraction: gui->dieArea (GuiDieArea),
 *	it connects callback functions to them and it initializes
 *	their visual appearance.
 *
 *	NOTE:	The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the widget pointer will be set to NULL
 *		after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_dieArea( Gui *gui, GtkBuilder *builder )
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	/* event-box */
	gui->dieArea.evboxWidget =
		GTK_WIDGET( gtk_builder_get_object(builder, "evboxImg2d") );
	g_signal_connect(
		G_OBJECT(gui->dieArea.evboxWidget),
		"button_press_event",
		G_CALLBACK(on_button_press_event_evboxImg),
		gui
		);
	g_signal_connect(
		G_OBJECT(gui->dieArea.evboxWidget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->dieArea.evboxWidget
		);

	/* image */
	gui->dieArea.imgWidget =
		GTK_WIDGET( gtk_builder_get_object(builder, "image2d") );
	g_signal_connect(
		G_OBJECT(gui->dieArea.imgWidget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->dieArea.imgWidget
		);

	/* button */
	gui->dieArea.btnWidget =
		GTK_WIDGET( gtk_builder_get_object(builder, "btnRoll") );
	gtk_button_set_label(
		GTK_BUTTON(gui->dieArea.btnWidget),
		TXT_BUTTON_ROLL2D
		);
	g_signal_connect(
		G_OBJECT(gui->dieArea.btnWidget),
		"clicked",
		G_CALLBACK(on_clicked_btnRoll),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->dieArea.btnWidget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->dieArea.btnWidget
		);

	return TRUE;
}

/*************************************************//**
 * Initialize the "About" dialog.
 *
 *	Copies the GUI dialog "About" from the specified GTK+
 *	builder into my GUI abstraction: gui->dlgAbout, it
 *	initializes the dialog's contents and it connects
 *	callback functions.
 *
 *	The "About" dialog is defined of type GtkAboutDialog
 *	in the Glade file, which provides a standardized way
 *	of presenting inormation. However, I haven't found
 *	an easy way to access the members of its action-area,
 *	namely the buttons: "Credits", "License" and "Close".
 *
 *	Consequently, I cannot mark the labels of those buttons
 *	as translatable, for the GNU-gettext library (I could
 *	implement it as a normal GtkDialog, but I prefer to
 *	learn the GtkAboutDialog internals and use them in
 *	a future version).
 *
 *	NOTE:	The callback function: on_activate_link_dlgAbout()
 *		is connected to the "activate-link" signal ONLY
 *		when the program is compiled under Win32. GTK+'s
 *		file-engine (Gio) has trouble registering the
 *		"http://" and "mailto:" protocols under Win32,
 *		so I conditionally use the Win32 API if needed.
 *
 *		The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the dialog's pointer will be set to
 *		NULL after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_dlgAbout( Gui *gui, GtkBuilder *builder )
{
	GdkPixbuf *logo = NULL;
	GError *error = NULL;
	const gchar *authors[] = {TXT_APP_AUTHOR, NULL};
	const gchar *artists[] = {TXT_APP_ARTIST, NULL};

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	/* load the about-dialog into gui, from the GTK+2 builder */
	gui->dlgAbout = GTK_WIDGET( gtk_builder_get_object(builder, "dlgAbout") );
	g_signal_connect(
		G_OBJECT(gui->dlgAbout),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->dlgAbout
		);

	/* set the about-dialog opacity */
	gtk_window_set_opacity( GTK_WINDOW(gui->dlgAbout), 0.80 );

	/* set the about-dialog logo */
	logo = gdk_pixbuf_new_from_file( FNAME_APPLOGO, &error );
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(gui->dlgAbout), logo );

	/* set the remaining prperties */
	gtk_about_dialog_set_program_name(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		gettext(TXT_APP_NAME)
		);

	gtk_about_dialog_set_version(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		gettext(TXT_APP_VERSION)
		);

	gtk_about_dialog_set_comments(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		TXT_APP_DESCRIPTION
		);

	gtk_about_dialog_set_copyright(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		gettext(TXT_APP_COPYRIGHT)
		);

	gtk_about_dialog_set_wrap_license( GTK_ABOUT_DIALOG(gui->dlgAbout), TRUE);
	gtk_about_dialog_set_license(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		TXT_APP_LICENSE
		);

	gtk_about_dialog_set_authors(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		authors
		);

	gtk_about_dialog_set_documenters(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		authors
		);

	gtk_about_dialog_set_artists(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		artists
		);

	gtk_about_dialog_set_website_label(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		gettext(TXT_APP_WEBSITE_LABEL)
		);
	gtk_about_dialog_set_website(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		TXT_APP_WEBSITE
		);

	/*
	 * connect callback functions to signals emitted by the dialog
	 */

#ifdef G_OS_WIN32
	/* when run on Win32, GTK+ (Gio) cannot open links,
	 * thus we overwrite the default behaviour by using
	 * our own Win32 specific callback function
	 */
	g_signal_connect(
		G_OBJECT(gui->dlgAbout),
		"activate-link",
		G_CALLBACK(on_activate_link_dlgAbout),
		gui
		);
#endif

	/* ensure that gui->dlgAbout will be set to NULL after it is destroyed */
	g_signal_connect(
		G_OBJECT(gui->dlgAbout),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),	/* provided by GTK+ */
		&gui->dlgAbout
		);


	return TRUE;
}

/*************************************************//**
 * Initialize the menus of the program.
 *
 *	Copies the menu widgets from the specified GTK+ builder
 *	into my GUI abstraction: gui->menu (GuiMenus), it connects
 *	callback functions to them and it initializes their visual
 *	appearance.
 *
 *	NOTE:	The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the widget pointer will be set to NULL
 *		after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_menus( Gui *gui, GtkBuilder *builder )
{
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	/*
	 * the File menu
	 */

	/* menu-title: File */
	gui->menu.titleFile =
		GTK_WIDGET( gtk_builder_get_object(builder, "menuTitleFile") );
	g_signal_connect(
		G_OBJECT(gui->menu.titleFile),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.titleFile
		);

	/* menu-item: File -> Quit */
	gui->menu.itemQuit =
		GTK_WIDGET( gtk_builder_get_object(builder, "menuItemQuit") );
	g_signal_connect(
		G_OBJECT(gui->menu.itemQuit),
		"activate",
		G_CALLBACK(on_activate_miQuit),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menu.itemQuit),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.itemQuit
		);

	/*
	 * the Language menu
	 */

	/* menu-title: Language */
	gui->menu.titleLang =
		GTK_WIDGET( gtk_builder_get_object(builder, "menuTitleLang") );
	g_signal_connect(
		G_OBJECT(gui->menu.titleLang),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.titleLang
		);

	/* menu: Lang */
	gui->menu.lang =
		GTK_WIDGET( gtk_builder_get_object(builder, "menuLang") );
	g_signal_connect(
		G_OBJECT(gui->menu.lang),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.lang
		);

	/* menu-item: Language -> English */
	gui->menu.itemEnglish =
		GTK_WIDGET( gtk_builder_get_object(builder, "menuItemEnglish") );
	g_signal_connect(
		G_OBJECT(gui->menu.itemEnglish),
		"activate",
		G_CALLBACK(on_activate_miEnglish),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menu.itemEnglish),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.itemEnglish
		);

	/* menu-item: Language -> Greek */
	gui->menu.itemGreek =
		GTK_WIDGET( gtk_builder_get_object(builder, "menuItemGreek") );
	g_signal_connect(
		G_OBJECT(gui->menu.itemGreek),
		"activate",
		G_CALLBACK(on_activate_miGreek),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menu.itemGreek),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.itemGreek
		);

	/* menu-item: Language -> use Environment */
	gui->menu.itemEnvLang =
		GTK_WIDGET( gtk_builder_get_object(builder, "menuItemEnvLang") );
	g_signal_connect(
		G_OBJECT(gui->menu.itemEnvLang),
		"activate",
		G_CALLBACK(on_activate_miEnvLang),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menu.itemEnvLang),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.itemEnvLang
		);
	gtk_widget_set_sensitive( gui->menu.itemEnvLang, FALSE );

	/*
	 * the Help menu
	 */

	/* menu-item: Help -> About */
	gui->menu.itemAbout =
		GTK_WIDGET( gtk_builder_get_object(builder, "menuItemAbout") );
	g_signal_connect(
		G_OBJECT(gui->menu.itemAbout),
		"activate",
		G_CALLBACK(on_activate_miAbout),
		gui
		);
	g_signal_connect(
		G_OBJECT(gui->menu.itemAbout),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.itemAbout
		);


	return TRUE;
}

/*************************************************//**
 * Initialize the status-bar of our main window.
 *
 *	Copies the GUI status-bar from the specified GTK+ builder into
 *	my GUI status-bar abstraction: gui->statusBar (GuiStatusBar),
 *	it connects callback functions to it and initializes its visual
 *	appearance.
 *
 *	My status-bar abstraction consists of a GTK+ statusbar widget,
 *	and a couple of supporting fields for the context-id and the
 *	text-message. We get the former from the builder and we
 *	initialize & apply the latter.
 *
 *	NOTE:	The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the widget pointer will be set to NULL
 *		after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_statusBar( Gui *gui, GtkBuilder *builder )
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	gui->statusBar.widget =
		GTK_WIDGET( gtk_builder_get_object(builder, "statusBar") );

	g_signal_connect(
		G_OBJECT(gui->statusBar.widget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->statusBar.widget
		);

	gui->statusBar.contextId =
		gtk_statusbar_get_context_id(
			GTK_STATUSBAR(gui->statusBar.widget),
			"Info"
			);

	gui->statusBar.currMessageId = 0;

	return TRUE;
}

/*************************************************//**
 * Initialize the main window of the application.
 *
 *	Copies the main window widget from the specified GTK+ builder
 *	to my GUI abstraction: gui->appWindow, it connects callback
 *	functions to it and initializes its visual appearance.
 *
 *	NOTE:	The GTK+ callback function: gtk_widget_destroyed()
 *		ensures that the widget pointer will be set to NULL
 *		after the widget gets destroyed.
 *****************************************************
 */
static gboolean gui_init_appWindow( Gui *gui, GtkBuilder *builder )
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	if ( !gui->appWindow )
		gui->appWindow = GTK_WIDGET( gtk_builder_get_object(builder, "wndMain") );

	gtk_window_set_icon_from_file(
		GTK_WINDOW(gui->appWindow),
		FNAME_APPICON,
		NULL
		);
	gtk_window_set_position( GTK_WINDOW(gui->appWindow), GTK_WIN_POS_CENTER );
	gtk_window_set_title( GTK_WINDOW(gui->appWindow), TXT_APP_NAME );

	g_signal_connect(
		G_OBJECT(gui->appWindow),
		"delete-event",
		G_CALLBACK(on_delete_event_appWindow),
		gui
		);
	g_signal_connect(
		G_OBJECT(gui->appWindow),
		"destroy",
		G_CALLBACK(on_destroy_appWindow),
		gui
		);
	g_signal_connect(
		G_OBJECT(gui->appWindow),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->appWindow
		);

	return TRUE;
}

/*************************************************//**
 *
 *****************************************************
 */
static void gui_get_caller_locale_environment( Gui *gui )
{
	gchar *temp = NULL;

	if ( !gui ) {
		return;
	}

	temp = setlocale(LC_ALL, NULL);
	strncpy(gui->callerLocaleEnv.locale, temp, MAXSIZ_LOCALE-1);
	gui->callerLocaleEnv.varLang	= (gchar *) g_getenv("LANG");
	gui->callerLocaleEnv.varLanguage= (gchar *) g_getenv("LANGUAGE");

	if ( !gui->callerLocaleEnv.locale
	|| !gui->callerLocaleEnv.varLang || !gui->callerLocaleEnv.varLanguage
	){
#if 1
		set_locale_environment(
			STR_EN_LOCALE,
			STR_EN_LOCALE_LANG,
			STR_EN_LOCALE_LANGUAGE
			);
#else
		setlocale( LC_ALL, STR_EN_LOCALE );
		g_setenv( "LANG", STR_EN_LOCALE_LANG, TRUE );
		g_setenv( "LANGUAGE", STR_EN_LOCALE_LANGUAGE, TRUE );
#endif
		DBG_GUI_ERRMSG(
			NULL,
			_("No locale environment was found.\n\
ENGLISH is used as fallback language.")
		);

	}

	return;
}

#if DISABLED
/*************************************************//**
 * UNUSED ***
 *****************************************************
 */
static gboolean gui_unload( Gui *gui )
{
	if ( !gui ) {
		return FALSE;
	}

	if ( gui->dlgAbout ) {
		gtk_widget_destroy( gui->dlgAbout );
		gui->dlgAbout = NULL;
	}
	if ( gui->appWindow ) {
		gui->quitOnDestroyAppWindow = FALSE;
		gtk_widget_destroy( gui->appWindow );
	}

	return TRUE;
}
#endif	/* #if DISABLED */

/*************************************************//**
 * Load GUI resources from the specified Glade file.
 *
 *	This function uses a GTK+ builder object to read the
 *	GUI resources defined in the specified glade-file,
 *	then it copies into my GUI abstraction only those
 *	resources that need further manipulation dynamically,
 *	and it initializes them.
 *****************************************************
 */
static gboolean gui_load_gtkGladeFile( Gui *gui, const gchar *fnameGlade )
{
	GtkBuilder	*builder = NULL;
	GError		*error = NULL;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !fnameGlade ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (fnameGlade)") );
		return FALSE;
	}

	/* create a GTK+2 builder with all the widgets defined in the glade-file */
	builder = gtk_builder_new();
	if ( !gtk_builder_add_from_file( builder, fnameGlade, &error ) )
	{
		DBG_GUI_ERRMSG( gui->appWindow, error->message );
		g_warning( "%s: (fatal error) %s", error->message, __func__ );
		g_error_free( error );
		return FALSE;
	}

	/* init GUI using only the needed elements from the GTK+2 builder */

	gui_init_appWindow( gui, builder );
	gui_init_dlgAbout( gui, builder );
	gui_init_menus( gui, builder );
	gui_init_dieArea( gui, builder );
	gui_init_dieArea3d( gui, builder );
	gui_init_statusBar( gui, builder );

	gui_init_settings_chkbtnDebugInfo( gui, builder );
	gui_init_settings_chkbtnUse2dRollEffect( gui, builder );
	gui_init_settings_hsliderMaxSteps( gui, builder );
	gui_init_settings_hsliderBaseDelay( gui, builder );
	gui_init_settings_hsliderStepDelay( gui, builder );
	gui_init_settings_btnDefault3d( gui, builder );
	gui_init_settings_btnDefault2d( gui, builder );

	/* release the GTK+2 builder */
	g_object_unref( G_OBJECT(builder) );

	return TRUE;
}

/*************************************************//**
 * Reload GUI resources from the specified Glade file.
 *
 *	This function is called when the user requests a dynamic
 *	change of the GUI language, via the "Language" menu.
 *
 *	It schedules the destruction of all the top-level windows
 *	of the program by calling gtk_widget_destroy() on them.
 *
 *	Before scheduling the main window of the application,
 *	it sets 'gui->quitOnDestroyAppWindow' to FALSE so that
 *	the connected callback function: on_destroy_appWindow()
 *	will NOT terminate the program after destructing the window
 *	(remember that 'gui' is passed as user-data to the callback
 *	function).
 *
 *	Once scheduling is done, 'gui->quitOnDestroyAppWindow'
 *	is reset back to TRUE and the GUI resources are loaded
 *	from scratch, from the glade-file into my 'gui' abstarction,
 *	by calling: gui_load_gtkGladeFile().
 *
 *	NOTE:	Before this function is called, the environment-variable
 *		LANG is explicitly set according to the user-request.
 *		Since the GNU-gettext library is runtime-aware with
 *		a LANG dependency, the reloaded GUI is displayed in
 *		the language requested by the user.
 *
 * 	See also:
 *		on_activate_miEnglish(), on_activate_miGreek(),
 *		and on_activate_miEnvLang().
 *****************************************************
 */
static gboolean gui_reload_gtkGladeFile( Gui *gui, const gchar *fnameGlade )
{
	/* sanity check */
	if ( !gui ) {
		return FALSE;
	}

	/* schedule the destruction of all top-level windows */
	if ( gui->dlgAbout ) {
		gtk_widget_destroy( gui->dlgAbout );
		gui->dlgAbout = NULL;
	}
	if ( gui->appWindow ) {
		gui->quitOnDestroyAppWindow = FALSE;
		gtk_widget_destroy( gui->appWindow );
	}

	gui->quitOnDestroyAppWindow = TRUE;
	return gui_load_gtkGladeFile(gui, fnameGlade);
}

/*************************************************//**
 * Cleanup GUI related stuff.
 * Normally, this function is called before program termination.
 *****************************************************
 */
static void gui_cleanup( Gui *gui )
{
	/* sanity check */
	if ( !gui ) {
		return;
	}

	dbg_print_locale_environment( "___ Local Locale Environment ___");

	/* restore locale environment of the caller */
	setlocale(LC_ALL, gui->callerLocaleEnv.locale);
	if ( gui->callerLocaleEnv.varLang ) {
		g_setenv( "LANG", gui->callerLocaleEnv.varLang, TRUE );
	}
	if ( gui->callerLocaleEnv.varLanguage ) {
		g_setenv( "LANGUAGE", gui->callerLocaleEnv.varLanguage, TRUE );
	}

}

/*************************************************//**
 * Initialize the gui abstraction using GTK+2.
 *
 *	This function initializes our GUI abstraction (gui)
 *	as a GTK+ gui & links it with the program's core-data.
 *
 *	NOTE:	For details about the fields: gui->envLang
 *		and gui->quitOnDestroyAppWindow, consult
 *		the comments of the following functions:
 *			on_destroy_appWindow()
 *			gui_reload_gtkGladeFile()
 *			on_activate_miEnglish()
 *			on_activate_miGreek()
 *			on_activate_miEnvLang()
 *			on_activate_miQuit()
 *****************************************************
 */
static gboolean gui_init_as_gtk2(
	Gui		*gui,		/* our (barely) GUI abstraction */
	const Core	*core,		/* for linking gui to app's core-data */
	int		*argc,		/* needed by GTK+ during initialization */
	char		***argv,	/* needed by GTK+ during initialization */
	const gchar	*fnameGladeFile	/* glade-file defining our GTK+ resources */
	)
{
	/* sanity checks */
	if ( !gui || !core || !argc || !argv || !fnameGladeFile ) {
		return FALSE;
	}

	/* initialize GTK+ */
	if ( !gtk_init_check( argc, argv ) ) {
		DBG_STDERR_MSG( "(fatal error) gtk+ not inited!" );
		return FALSE;
	}

	/* ensure gui abstraction starts fresh */
	memset( gui, 0, sizeof(Gui) );

	/* get and save the caller's locale environment */
	gui_get_caller_locale_environment( gui );

	/* initially, destruction of main window results in program-termination */
	gui->quitOnDestroyAppWindow = TRUE;

	/* link gui with the core-data of the program */
	gui->linkToCoreData = (Core *) core;

	/* load GUI resources from Glade file */
	if ( !gui_load_gtkGladeFile(gui, fnameGladeFile) ) {
		DBG_STDERR_MSG( "(fatal error) gui resources not loaded!" );
		return FALSE;
	}

	return TRUE;
}

/*************************************************//**
 * Cleanup core-data of the program.
 * This function is called before program termination.
 *****************************************************
 */
static void core_cleanup( Core *core )
{
	/* sanity check */
	if ( !core ) {
		return;
	}

	/* release memory reserved for the random-generator */
	if ( core->randGen ) {
		g_rand_free( core->randGen );
		core->randGen = NULL;
	}
}

/*************************************************//**
 * Initialize the core-data of the program.
 *
 *	As core-data I define the minimum information
 *	needed for the program to be operational even
 *	without a GUI.
 *
 *	This may prove handy if we decide later on to
 *	implement a fallback vanilla text-interface,
 *	in case the GUI fails to get initialized.
 *
 *	For this little demo program, the core-data
 *	consist of just a GLib random-generator and
 *	the final outcome after rolling the die.
 *****************************************************
 */
static gboolean core_init( Core *core )
{
	/* sanity check */
	if ( !core ) {
		return FALSE;
	}

	memset( core, 0, sizeof(Core) );
	core->randGen		= g_rand_new();	/* GLib random-generator */
	core->resultRolled	= 0;

	return TRUE;
}

/*************************************************//**
 * Application entry point.
 *****************************************************
 */
int main( int argc, char **argv )
{
	Core	core;	/* core-data of the program */
	Gui	gui;	/* my (barely) abstracted GUI */

	global_debugOn = TRUE;
/*	memset( &gui, 0, sizeof(Gui) );
*/

	/* prepare GNU-gettext internationalization */
	if ( !init_localization_gettext( GETTEXT_PACKAGE, LOCALEDIR, OUTCODESET) ) {
		DBG_STDERR_MSG( "(fatal error) localization failed!" );
		goto cleanup_and_exit_failure;
	}

	/* init core-data */
	if ( !core_init(&core) ) {
		DBG_STDERR_MSG( "(fatal error) core data not inited!" );
		goto cleanup_and_exit_failure;
	}

	/* init the gui as GTK+ */
	if ( !gui_init_as_gtk2(&gui, &core, &argc, &argv, FNAME_GLADE) ) {
		DBG_STDERR_MSG( "(fatal error) gui not inited!" );
		goto cleanup_and_exit_failure;
	}
	dbg_print_gui_callerLocaleEnv( &gui, "Caller Locale Environmet was:" );
	dbg_print_locale_environment( "GUI Locale Environmet set to:" );

	/* display the main-window of the application, and all its children */
	if ( gui.appWindow ) {
		gtk_widget_show_all( gui.appWindow );
	}

	/* start the GTK+ event loop */
	gtk_main();

	/* cleanup & exit successfully */
	gui_cleanup( &gui );
	core_cleanup( &core );
	exit( EXIT_SUCCESS );

cleanup_and_exit_failure:

	/* cleanup & exit due to fatal error */
	gui_cleanup( &gui );
	core_cleanup( &core );
	exit( EXIT_FAILURE );
}
