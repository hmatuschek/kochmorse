SET QT_PATH=C:/Qt/Qt5.3.2
SET QT_BIN_PATH=%QT_PATH%/5.3/mingw482_32/bin
SET LIB_PATH=C:/MinGW/bin
SET NSIS_PATH=C:/Program Files (x86)/NSIS
SET PATH=%NSIS_PATH%;%PAHT%

COPY "%QT_BIN_PATH%/Qt5Core.dll" .
COPY "%QT_BIN_PATH%/Qt5Gui.dll" .
COPY "%QT_BIN_PATH%/Qt5Widgets.dll" .
COPY "%QT_BIN_PATH%/Qt5Svg.dll" .
COPY "%QT_BIN_PATH%/icuin52.dll" .
COPY "%QT_BIN_PATH%/icuuc52.dll" .
COPY "%QT_BIN_PATH%/icudt52.dll" .
COPY "%QT_BIN_PATH%/libwinpthread-1.dll" .
COPY "%LIB_PATH%/libportaudio-2.dll" .
COPY "%LIB_PATH%/libgcc_s_dw2-1.dll" .
COPY "%LIB_PATH%/libstdc++-6.dll" .

makensis kochmorse.nsi
