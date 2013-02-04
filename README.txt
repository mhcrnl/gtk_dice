----------------
DIE - GTK+2 Demo
----------------

DESCRIPTION:	Small GTK+2 demo, faking an animation of a die (2D & 3D).
AUTHOR:		migf1 <mig_f1@hotmail.com>
WEBSITE:	https://github.com/migf1/gtk_dice | http://x-karagiannis.gr/prg/
LICENSE:	Free for all

PROGR.LANGUAGE:	C (ANSI 89 / ISO C90)
PLATFORM:	Cross-platfrom

REQUIREMENTS:	for Execution:
			Runtime of GTK+ v2.24.8 or newer (*not* GTK+3)
		for Compilation:
			GTK+ v2.24.8 or newer DevPackages (*not* GTK+3)
			(they include the runtime)

BUILT INFO:	Developed on Windows XP SP3, using:
			GTK+2 v2.24.8all-in-one* bundle
			MinGW32 GCC v4.70
			Glade v3.8.3 (*not* v3.14.x, that's for GTK+3)
		Tested on:
			Windows XP SP3	(32bit)
			Windows 7	(64bit)
			Ubuntu 1.10	(32bit)

USEFUL LINKS:	[ All platforms ]
		GTK+ website:
			http://www.gtk.org/
		GTK+2 tutorial:
			http://developer.gnome.org/gtk-tutorial/2.24/
		GTK+2 reference manual:
			http://developer.gnome.org/gtk2/
		Glade website:
			http://glade.gnome.org/
		Gnome Developer Center:
			http://developer.gnome.org/
		GLib i18n Internationalization:
			http://developer.gnome.org/glib/2.31/glib-I18N.html

		[ Windows ]
		GTK+2 Win32 downloads:
			http://www.gtk.org/download/win32.php
		GTK+2 for Windows Runtime Environment:
			http://gtk-win.sourceforge.net/
		GTK+2&3 for Windows (MinGW):
			http://sourceforge.net/projects/gtk-mingw/

		[ MacOSX ]
		GTK+ Mac OS X (gtk-osx):
			http://sourceforge.net/projects/gtk-osx/
		GTK+2 OSX Packages (Huntsville Macintosh Users Group):
			http://www.hmug.org/pub/MacOS_X/X/Libraries/Graphics/gtk+2/
		Building GTK-OSX:
			https://live.gnome.org/GTK+/OSX/Building

----------------
WHY
----------------

I'm not a GTK+ guru, not even close. This small program is meant as
a little demonstration of my efforts to refresh and/or expand my GTK+
skills. I'm quite familiar with the C programming language though.

I'm continuously playing-around with the program, but I'm doing it in
my spare time, which sadly is not that much these days. This also means
that proper off-source documentation is unfortunately out of question,
at least for the moment.

However, I've included plenty of comments in the sources, hoping they
will be helpful in understanding what is going on and why.

Nevertheless, here is a smal list of some things I consider worth looking
at (and/or perhaps improving) in the code:

- GTK+/GLib localization/internationalizarion, using the GNU-gettext lib
- Separation between GUI and Core data
- Organizing GTK+ graphical elements in GUI abstractions, for easier handling
- Using the GTK+2 builder object for loading resources from (XML) Glade file
- Switching the GUI language dynamically

For the time being the sources is just one big .c file, but it will be
split up into proper source modules later on.

The whole thing can be downloaded as one ZIP file (including the Win32 binary)
from the "Downloads" directory, in GitHub (select "Raw").
