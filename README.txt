----------------
DIE - GTK+2 Demo
----------------

DESCRIPTION:	Small GTK+2 demo, faking an animation of a die (2D & 3D).
AUTHOR:		migf1 <mig_f1@hotmail.com> | http://x-karagiannis.gr/prg/
WEBSITE:	https://github.com/migf1/gtk_dice
LICENSE:	Free for all

PROGR.LANGUAGE:	C (ANSI 89 / ISO C90)
PLATFORM:	Cross-platfrom

REQUIREMENTS:	for Execution:
			Runtime of GTK+ v2.24.8 or newer (*not* GTK+3)
		for Compilation:
			GTK+ v2.24.8 or newer DevPackages (*not* GTK+3)
			(they include the runtime)

BUILT INFO:	Developed on Windows XP SP3, using:
			GTK+2 v2.24.8 *all-in-one* bundle
			MinGW32 GCC v4.70
			Glade v3.8.3 (*not* v3.14.x, that's for GTK+3)
		Tested on:
			Windows XP SP3	(32bit)
			Windows 7	(64bit)
			Ubuntu 11.10	(32bit)

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

		[ GNU Linux/Unix ]
		GTK+ downloads:
			http://www.gtk.org/download/linux.php
		Building the GTK+2 libraries:
			http://developer.gnome.org/gtk2/stable/gtk-building.html

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
a little demonstration of my efforts to refresh and/or expand whatever
GTK+ skills of mine. I'm quite familiar with the C programming language
though.

Initially I wrote it in response to a question at insomnia.gr asking
for a simple rolling-the-die effect: http://tinyurl.com/amb837e (it
is all in Greek). Then I decided to tidy up a bit the code and make
it public, just in case it proves useful to anyone.
 
I'm occasionally playing around with this little program, but I'm
doing it in my spare time, which is sadly rather limited these days.
This also means that proper off-source documentation is unfortunately
out of question, at least for the moment.

However, I've included plenty of comments in the sources, hoping they
will be helpful in understanding what is going on and why. Nevertheless,
here is a small list of some concepts I consider worth looking at in the
code (and/or perhaps improve on them):

- 	localization using GLib's support for the GNU-gettext library
- 	basic separation of the GUI from the core-data
- 	basic organization-abstractions of GTK+ graphical elements
- 	Using GtkBuilder to load the GUI from XML UI definition file (Glade file)
- 	on-the-fly switching of the GUI language

The sources is just one .c file (there's not even a separate header file).
I may split it up into proper source modules later on, depending on my
free time (frankly, I think it is not relly needed, since this is just
a tiny toy-project).


----------------
DOWNLOADS
----------------

I've set up a primitive GitHub project for the... "beast", so you can
browse the code on-line at: https://github.com/migf1/gtk_dice.

The "Downloads" directory contains downloadable ZIP files, labeled
as following (XXx stands for the version):

	public_gtk_dice_XXx.zip

You may download any of those ZIP files, by first clicking on it
and then selecting the option "Raw".

Each of those ZIP files contains, among other things, the following
pre-compiled executables:

	gtk_dice_win32.exe	(32bit Win32 executable, needs XP SP3 or better)
	gtk_dice_ubu1110	(32bit Ubuntu 11.10 executable)

In order to run the binaries, you must already have installed and
configured on your system the GTK+ 2.24.8 (or newer) runtime.
As of this writing, the latest GTK+2 version available for Win32
was 2.24.10 (please see the section "Useful Links" at the top of
this file).

Please note that the program has been *partially* tested with GTK+3
*only* on Ubuntu 11.10 32bit with the Gnome Desktop, so there are
no guarantees if you run it under the GTK+3 runtime on other systems
and/or configurations.

If you launch the program from the command-line, then you will see
extra debugging messages written on the console during its execution.
This is the default behavior of the program, but it can be disabled
by un-ticking the Settings option: "Debugging Mode". 


-------------------
RUNNING THE PROGRAM
-------------------

The functionality of the program is rather trivial, it just fakes
a rolling-effect animation for a standard die. In 2D and pseudo 3D.

It's not a true animation, in the sense that no frames (image files)
are generated dynamically by some kind of animation-engine and also
no frames have been generated (statically) by any animation-engine.

For each animation, 2D and 3D, there is a fixed number of images:
32 and 6, respectively. Those images are simply loaded sequentially
one after the other into the same visual area. The delay between
two consecutive images is defined by the user, and it can be set
to get progressively increased (pseudo friction).

Almost all GUI elements pop up a tooltip on mouse-over, explaining
what they do (and often how they do it).

Controlling the GUI language
----------------------------

An interesting fetaure of this little program is that the GUI language
can be switched on-the-fly (not a common behavior among GTK+ applications).
Also, the user can control the language to be used when the program starts,
by setting up the environment variables LANG and LANGUAGE.

On GNU Linux/Unix, those environment variables are usually already set by
the system (via the Regional Settings) so the program inherits automatically
the already defined system language.

On Windows, those environment variables are not usually set, so the program
fallbacks to English (its default languge).

In any case, regardless of the inherited locale, the GUI language can be
explicitly changed on-the-fly from within the program, via the "Language"
menu.

For the moment only Engish and Greek are directly supported, but there's 
also another option in the menu called: "use Environment". Say for example
that your system is set with a French locale, via the environment variables
LANG and LANGUAGE.

Let assume for example that they are set as following:

	LANG = fr_FR.UTF-8
	LANGUAGE = fr

Providing a French translation is availabe (which is not :P) then when
starting the program, the GUI will be displayed in French. By selecting
"Language->Greek" you can switch to Greek. Then you can switch back to
French by selecting "Language->use Environment".

Making a translation
--------------------

Assuming you would like to contribute (say for example) a French translation, 
please follow these simple steps:

1.	Download & install PoEdit from: http://www.poedit.net/download.php

2.	Open the file "./lang/en_default.pot" into PoEdit

3.	Select "Catalogue -> Properties -> [Translation properties]" and change
	the fields "Team" and "Team's mail address" so they reflect your name
	and e-mail.

	In the field "Language" replace English with French.

	If you are on Linux/Unix, change also the field "Source code charset"
	from windows-1253 to UTF-8.

4.	Select "File->Save as..." to save this file as: "gtk_dice.po"

	This will create the following 2 files inside the "./lang" directory:
		gtk_dice.po
		gtk_dice.mo

5.	Still inside PoEdit, use its friendly interface to translate all
	original English strings to their French counterparts. Feel free
	to periodically "File->Save" your work.

6.	Quit PoEdit and create a "fr" directory inside "./lang"
		For a list of valid names for this directory
		for other languages, please consult this page:
		http://www.gnu.org/software/gettext/manual/gettext.html#Language-Codes

	Then create a sub-directory "./lang/fr/LC_MESSAGES"

	Lastly, move the files: "gtk_dice.po" and "gtk_dice.mo"
	from "./lang" into "./lang/fr/LC_MESSAGES"

7.	You are done.

Providing you have set the environment variables LANG and LANGUAGE according
to the example given in the previous section, the program will start up with
a French GUI. Moreover, every time you select "Language -> use Environment"
the GUI language will switch back to French.

If you modify the sources of the program, which will probably lead to new GUI
strings, then you should read the file: "_xgettetxt.info" for instructions
on how to create an updated "en_default.pot" file, which should then be given
to the translators (alternatively, you may use PoEdit again for the whole
process).

---------------------
COMPILING THE SOURCES
---------------------

You may compile the program yourselves, providing you have already
installed & configured on your system the DevPackages of GTK+2.24.8
(or newer). As of this writing, the latest GTK+2 version available
for Win32 was 2.24.10 (please see the section "Useful Links" at the
top of this file).

Please note that the program has been *partially* tested with GTK+3
*only* on Ubuntu 11.10 32bit with the Gnome Desktop, so there are
no guarantees if you compile it against GTK+3 on other systems and/or
configurations.

Compiling on GNU Linux/Unix
---------------------------

Most Linux distributions come with one or more versions of the GTK+2
runtime pre-installed, especially those relying on the Gnome project.
However, they may not include the GTK+ developer packages, or even if
they do, you may need to upgrade them to version 2.24.8 or better,
required for compiling this program.

Sadly, the exact procedure differs across different distributions. It
may be even necessary to built the desired GTK+ DevPackages directly
from their sources, if nothing else works (e.g. you cannot find them
as an official or 3rd-party package).

On Ubuntu it's quite simple to install/update the GTK+ DevPackages,
although it may take some time if you're on a slow internet connection:

	sudo apt-get install libgtk2.0-dev

*** NOTE:
***
*** On a side note, for GTK+3 you can use: sudo apt-get install libgtk-3-dev
*** it will be installed along side of GTK+2 (they won't interfere).
***
*** Similarly, for GTK+2 Glade you may use: sudo apt-get install glade-gtk2
*** and for GTK+3 Glade you may use: apt-get install glade
***

In any case, once everything is set up you can compile the program
by calling the GCC compiler with the following command:

	gcc -o gtk_dice gtk_dice.c `pkg-config --cflags --libs gtk+-2.0`

"gtk_dice" is the name of the executable file to be produced, which
you can then invoke either from the terminal or from the Desktop.

See the following section ("Compiling on Windows") for some alternative
GCC command-line flags you can use (e.g. for optimizing the executable
or for compiling with warnings and debugging information enabled).

Compiling on Windows
--------------------

Although you may use any compiler (see at the end of this section),
the recommended way to compile the program on Windows is via the
MinGW32 tool-chain (http://www.mingw.org/). Providing of course
that the GTK+2 DevPackages are already installed and properly
configured for both your system and the MinGW32 tool-chain.

Then you should use the (MinGW32) MSYS command-line terminal to
navigate into the directory you extracted the sources into, and
type:

	gcc -finput-charset=cp1253 -o gtk_dice.exe gtk_dice.c `pkg-config --cflags --libs gtk+-2.0`

If you want the compiler to add debbuging information to the executable
and emit all kind of warnings during the compilation, add the flags "-g3"
"-Wall" and "-Wextra":

	gcc -g3 -Wall -Wextra -finput-charset=cp1253 -o gtk_dice.exe gtk_dice.c `pkg-config --cflags --libs gtk+-2.0`

For the final "release" compilation, you may wish to compile against
the ANSI C89 standard along with optimizations for the executable:

	gcc -std=c89 -s -O2 -finput-charset=cp1253 -o gtk_dice.exe gtk_dice.c `pkg-config --cflags --libs gtk+-2.0`

*** NOTE ***
*** 
*** In all cases above, the pkg-config part is enclosed in back-quotes,
*** which are NOT the same with the normal single-quotes on your keyboard.
*** Actually that's the reason why you cannot use the normal command-prompt
*** console for compiling the sources using pkg-config on Windows (it does
*** not "understand" the back-quotes).
*** 
*** Furthermore, in order to ensure that pkg-config will apply the GTK+2
*** dependencies, make sure that you have set the environment variable
*** PKG_CONFIG_PATH containing the path of the file: gtk+2.0.pc
*** 
*** Say for example that your GTK+2 DevPackages are installed in: C:\GTK+2
*** Then PKG_CONFIG_PATH should contain the path: C:\GTK+2\lib\pkgconfig
*** 

The "-finput-charset=cp1253" flag tells GCC that the source files are 
saved in the "Windows 1253" text-encoding, which is indeed the encoding
I have used to save the source files.

However, if you edit & save any of the source files you *must* make
sure that your editor does *not* alter their text-encoding. If it gives
you the option to specify the desired text-encoding before saving, then
select "Windows 1253" or "CP1253" (whichever is available). If your editor
does not let you specify the text-encoding, then see what encoding it uses
and replace "cp1253" in the command-line of GCC.

For a list of the text-encodings GCC understands, have a look at this page:
	http://www.gnu.org/software/libiconv/

You may also compile the sources directly from the standard command-prompt
console of Windows, thus removing the need of using pkg-config, but it will
require a bit more effort from your part in order to set it up. However, it
is an one-time effort, since afterwards you'll be able to compile any GTK+2
program right from within the standard command-prompt console of Windows.

Frankly, the latter is the method I use and I have explained in details in
the README.TXT file of an older GTK+2 toy-project of mine: GTK TIC_TAC_TOE.

Feel free to download it from my website, at:
	http://tinyurl.com/b29n3vs

It also explains step-by-step how to set up GTK+2 and Glade on Windows,
along with information on how to compile GTK+2 programs using Pelles-C.
Pelles-C is a pure Win32 IDE + C compiler, and most certainly it will
make you feel much more comfortable if you are a Win32 kind of guy :)

After compiling the sources, the executable file: gtk_dice.exe will
be present in the same directory, and you can launch it either from
the command-line or by double-clicking on it from the Desktop.

*** NOTE ***
***
*** As a final note, you can use the "-mwindows" linker flag when
*** invoking the MinGW32 GCC from the command-line. This will cause
*** the executable to run without opening a console window next to
*** the normal GUI window.
***
*** This, in turn, means that no debugging messages will be displayed,
*** even if the setting "Debug mode" is enabled. Not even if you start
*** the program directly from the command-line.
***