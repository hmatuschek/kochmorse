SET PATH=C:/Program Files (x86)/NSIS;%PAHT%
SET QT_PATH=C:/Qt/Qt5.3.2/5.3/mingw482_32/bin
SET QT_PLUGIN_PATH=C:/Qt/Qt5.3.2/5.3/mingw482_32/plugins
SET LIB_PATH=C:/MinGW/bin
SET NSIS_PATH=C:/Program Files (x86)/NSIS
SET PATH=%NSIS_PATH%;%PAHT%

COPY "src/kochmorse.exe" .
COPY "%QT_BIN_PATH%/Qt5Core.dll" .
COPY "%QT_BIN_PATH%/Qt5Gui.dll" .
COPY "%QT_BIN_PATH%/Qt5Widgets.dll" .
COPY "%QT_BIN_PATH%/Qt5Multimedia.dll" .
COPY "%QT_BIN_PATH%/Qt5Svg.dll" .
COPY "%QT_BIN_PATH%/Qt5Xml.dll" .
COPY "%QT_BIN_PATH%/icuin52.dll" .
COPY "%QT_BIN_PATH%/icuuc52.dll" .
COPY "%QT_BIN_PATH%/icudt52.dll" .
COPY "%QT_BIN_PATH%/libwinpthread-1.dll" .
COPY "%LIB_PATH%/libgcc_s_dw2-1.dll" .
COPY "%LIB_PATH%/libstdc++-6.dll" .
MKDIR platforms
COPY "%QT_PLUGIN_PATH%/platforms/qwindows.dll" platforms
MKDIR imageformats 
COPY "%QT_PLUGIN_PATH%/imageformats/qsvg.dll" imageformats
MKDIR iconengines
COPY "%QT_PLUGIN_PATH%/iconengines/qsvgicon.dll" iconengines

makensis kochmorse.nsi
