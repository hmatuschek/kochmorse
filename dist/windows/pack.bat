SET QT_LIB_PATH=C:/Qt/Qt5.11.1/5.11.1/mingw53_32/bin
SET QT_PLUGIN_PATH=C:/Qt/Qt5.11.1/5.11.1/mingw53_32/plugins
SET NSIS_PATH=C:/Program Files (x86)/NSIS
SET PATH=%NSIS_PATH%;%PAHT%

COPY "src/kochmorse.exe" .
COPY "%QT_LIB_PATH%/Qt5Core.dll" .
COPY "%QT_LIB_PATH%/Qt5Gui.dll" .
COPY "%QT_LIB_PATH%/Qt5Widgets.dll" .
COPY "%QT_LIB_PATH%/Qt5Multimedia.dll" .
COPY "%QT_LIB_PATH%/Qt5Network.dll" .
COPY "%QT_LIB_PATH%/Qt5Svg.dll" .
COPY "%QT_LIB_PATH%/Qt5Xml.dll" .
COPY "%QT_LIB_PATH%/libgcc_s_dw2-1.dll" .
COPY "%QT_LIB_PATH%/libstdc++-6.dll" .
COPY "%QT_LIB_PATH%/libwinpthread-1.dll" .
MKDIR audio
COPY "%QT_PLUGIN_PATH%/audio/qtaudio_windows.dll" audio
MKDIR platforms
COPY "%QT_PLUGIN_PATH%/platforms/qwindows.dll" platforms
MKDIR imageformats 
COPY "%QT_PLUGIN_PATH%/imageformats/qsvg.dll" imageformats
MKDIR iconengines
COPY "%QT_PLUGIN_PATH%/iconengines/qsvgicon.dll" iconengines

makensis kochmorse.nsi
