SET PATH=C:/Program Files (x86)/NSIS;%PAHT%
SET QT_PATH=C:/Qt/Qt5.3.2/5.3/mingw482_32/bin
SET QT_PLUGIN_PATH=C:/Qt/Qt5.3.2/5.3/mingw482_32/plugins
SET LIB_PATH=C:/MinGW/bin

COPY "%QT_PATH%/Qt5Core.dll" .
COPY "%QT_PATH%/Qt5Gui.dll" .
COPY "%QT_PATH%/Qt5Widgets.dll" .
COPY "%QT_PATH%/Qt5Svg.dll" .
COPY "%QT_PATH%/icuin52.dll" .
COPY "%QT_PATH%/icuuc52.dll" .
COPY "%QT_PATH%/icudt52.dll" .
COPY "%QT_PATH%/libwinpthread-1.dll" .
COPY "%LIB_PATH%/libportaudio-2.dll" .
COPY "%LIB_PATH%/libgcc_s_dw2-1.dll" .
COPY "%LIB_PATH%/libstdc++-6.dll" .
COPY "%QT_PLUGIN_PATH/platform/qwindows.dll" .
COPY "%QT_PLUGIN_PATH/imageformats/qsvg.dll" .

makensis kochmorse.nsi
