#Koch morse-code tutor

This is a C++/Qt reimplementation of my "old" python morse-code tutor (also called "kochmorse", https://kochmorse.googlecode.com). As the "old" implementation relied on ALSA for sound output, it was restricted to Linux. This reimplementation now uses PortAudio for the sound output and the GUI is implemented using Qt5, it is therefore more platform independent and runs under Linux, MacOS X and Windows.

For this reimplementation, I simplified the GUI a lot. All settings are now "hidden" in a preferences dialog. All features of the "old" version are now present in this one and even some more...

 
