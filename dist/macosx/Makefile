SYSTEM_LIBRARY_DIR = /opt/local/lib
SYSTEM_BINARY_DIR = /opt/local/bin
QTCORE_FRAMEWORK = /opt/local/Library/Frameworks/QtCore.framework/Versions/5/QtCore
QTGUI_FRAMEWORK = /opt/local/Library/Frameworks/QtGui.framework/Versions/5/QtGui
QTWIDGETS_FRAMEWORK = /opt/local/Library/Frameworks/QtWidgets.framework/Versions/5/QtWidgets
QTXML_FRAMEWORK = /opt/local/Library/Frameworks/QtXml.framework/Versions/5/QtXml
QTSVG_FRAMEWORK = /opt/local/Library/Frameworks/QtSvg.framework/Versions/5/QtSvg
QTPRINT_FRAMEWORK = /opt/local/Library/Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport
QT_PLUGINS = /opt/local/share/qt5/plugins
QT_PLUGINS_PLATFORM =${QT_PLUGINS}/platforms
QT_PLUGINS_PRINT =${QT_PLUGINS}/printsupport
QT_PLUGINS_ACCESSIBLE = ${QT_PLUGINS}/accessible
QT_PLUGINS_IMAGE =${QT_PLUGINS}/imageformats
APP_LIBRARY_DIR = KochMorse.app/Contents/Frameworks
APP_BINARY_DIR  = KochMorse.app/Contents/MacOS
APP_PLUGIN_DIR  = KochMorse.app/Contents/PlugIns
APP_RESOURCE_DIR  = KochMorse.app/Contents/Resources
VERSION = 3.2.2

app: dmg

clean:
	rm -f ${APP_BINARY_DIR}/kochmorse
	rm -f ${APP_LIBRARY_DIR}/*
	rm -f KochMorse-${VERSION}.dmg

dmg: kochmorse-app
	cp kochmorse.icns ${APP_RESOURCE_DIR}
	hdiutil create -srcfolder KochMorse.app kochmorse-${VERSION}.dmg


kochmorse-app: QtGui QtSvg QtCore QtWidgets QtPlugins portaudio
	cp $(SYSTEM_BINARY_DIR)/kochmorse ${APP_BINARY_DIR}
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_BINARY_DIR}/kochmorse
	install_name_tool -change ${QTGUI_FRAMEWORK} @executable_path/../Frameworks/QtGui ${APP_BINARY_DIR}/kochmorse
	install_name_tool -change ${QTWIDGETS_FRAMEWORK} @executable_path/../Frameworks/QtWidgets ${APP_BINARY_DIR}/kochmorse
	install_name_tool -change ${QTSVG_FRAMEWORK} @executable_path/../Frameworks/QtSvg ${APP_BINARY_DIR}/kochmorse
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libportaudio.2.dylib @executable_path/../Frameworks/libportaudio.2.dylib ${APP_BINARY_DIR}/kochmorse 

portaudio:
	cp $(SYSTEM_LIBRARY_DIR)/libportaudio.2.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/portaudio.2.dylib ${APP_LIBRARY_DIR}/libportaudio.2.dylib

QtPlugins: QtPrintSupport libz 
	cp ${QT_PLUGINS_PLATFORM}/libqcocoa.dylib ${APP_PLUGIN_DIR}/platforms
	install_name_tool -id @executable_path/../PlugIns/platforms/libqcocoa.dylib ${APP_PLUGIN_DIR}/platforms/libqcocoa.dylib
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_PLUGIN_DIR}/platforms/libqcocoa.dylib
	install_name_tool -change ${QTGUI_FRAMEWORK} @executable_path/../Frameworks/QtGui ${APP_PLUGIN_DIR}/platforms/libqcocoa.dylib
	install_name_tool -change ${QTWIDGETS_FRAMEWORK} @executable_path/../Frameworks/QtWidgets ${APP_PLUGIN_DIR}/platforms/libqcocoa.dylib
	install_name_tool -change ${QTPRINT_FRAMEWORK} @executable_path/../Frameworks/QtPrintSupport ${APP_PLUGIN_DIR}/platforms/libqcocoa.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_PLUGIN_DIR}/platforms/libqcocoa.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libglib-2.0.0.dylib @executable_path/../Frameworks/libglib-2.0.0.dylib ${APP_PLUGIN_DIR}/platforms/libqcocoa.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libgthread-2.0.0.dylib @executable_path/../Frameworks/libgthread-2.0.0.dylib ${APP_PLUGIN_DIR}/platforms/libqcocoa.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libintl.8.dylib @executable_path/../Frameworks/libintl.8.dylib ${APP_PLUGIN_DIR}/platforms/libqcocoa.dylib
	cp ${QT_PLUGINS_PRINT}/libcocoaprintersupport.dylib ${APP_PLUGIN_DIR}/printsupport
	install_name_tool -id @executable_path/../PlugIns/printsupport/libcocoaprintersupport.dylib ${APP_PLUGIN_DIR}/printsupport/libcocoaprintersupport.dylib
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_PLUGIN_DIR}/printsupport/libcocoaprintersupport.dylib
	install_name_tool -change ${QTGUI_FRAMEWORK} @executable_path/../Frameworks/QtGui ${APP_PLUGIN_DIR}/printsupport/libcocoaprintersupport.dylib
	install_name_tool -change ${QTWIDGETS_FRAMEWORK} @executable_path/../Frameworks/QtWidgets ${APP_PLUGIN_DIR}/printsupport/libcocoaprintersupport.dylib
	install_name_tool -change ${QTPRINT_FRAMEWORK} @executable_path/../Frameworks/QtPrintSupport ${APP_PLUGIN_DIR}/printsupport/libcocoaprintersupport.dylib
	cp ${QT_PLUGINS_ACCESSIBLE}/libqtaccessiblewidgets.dylib ${APP_PLUGIN_DIR}/accessible
	install_name_tool -id @executable_path/../PlugIns/accessible/libqtaccessiblewidgets.dylib ${APP_PLUGIN_DIR}/accessible/libqtaccessiblewidgets.dylib
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_PLUGIN_DIR}/accessible/libqtaccessiblewidgets.dylib
	install_name_tool -change ${QTGUI_FRAMEWORK} @executable_path/../Frameworks/QtGui ${APP_PLUGIN_DIR}/accessible/libqtaccessiblewidgets.dylib
	install_name_tool -change ${QTWIDGETS_FRAMEWORK} @executable_path/../Frameworks/QtWidgets ${APP_PLUGIN_DIR}/accessible/libqtaccessiblewidgets.dylib
	mkdir -p ${APP_PLUGIN_DIR}/imageformats
	cp ${QT_PLUGINS_IMAGE}/libqsvg.dylib ${APP_PLUGIN_DIR}/imageformats
	install_name_tool -id @executable_path/../PlugIns/imageformats/libqsvg.dylib ${APP_PLUGIN_DIR}/imageformats/libqsvg.dylib
	install_name_tool -change ${QTSVG_FRAMEWORK} @executable_path/../Frameworks/QtSvg ${APP_PLUGIN_DIR}/imageformats/libqsvg.dylib
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_PLUGIN_DIR}/imageformats/libqsvg.dylib
	install_name_tool -change ${QTGUI_FRAMEWORK} @executable_path/../Frameworks/QtGui ${APP_PLUGIN_DIR}/imageformats/libqsvg.dylib
	install_name_tool -change ${QTWIDGETS_FRAMEWORK} @executable_path/../Frameworks/QtWidgets ${APP_PLUGIN_DIR}/imageformats/libqsvg.dylib
 

QtGui: QtCore libz libpng
	cp ${QTGUI_FRAMEWORK} ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtGui ${APP_LIBRARY_DIR}/QtGui
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_LIBRARY_DIR}/QtGui
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/QtGui
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libpng16.16.dylib @executable_path/../Frameworks/libpng16.16.dylib ${APP_LIBRARY_DIR}/QtGui

QtWidgets: QtCore QtGui libz
	cp ${QTWIDGETS_FRAMEWORK} ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtWidgets ${APP_LIBRARY_DIR}/QtWidgets
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_LIBRARY_DIR}/QtWidgets
	install_name_tool -change ${QTGUI_FRAMEWORK} @executable_path/../Frameworks/QtGui ${APP_LIBRARY_DIR}/QtWidgets
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/QtWidgets

QtXml: QtCore
	cp ${QTXML_FRAMEWORK} ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtXml ${APP_LIBRARY_DIR}/QtXml
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_LIBRARY_DIR}/QtXml

QtSvg: QtCore QtXml libz
	cp ${QTSVG_FRAMEWORK} ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtSvg ${APP_LIBRARY_DIR}/QtSvg
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_LIBRARY_DIR}/QtSvg
	install_name_tool -change ${QTGUI_FRAMEWORK} @executable_path/../Frameworks/QtGui ${APP_LIBRARY_DIR}/QtSvg
	install_name_tool -change ${QTWIDGETS_FRAMEWORK} @executable_path/../Frameworks/QtWidgets ${APP_LIBRARY_DIR}/QtSvg
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/QtSvg

QtCore: libz libicu libpcre libgthread libglib libintl
	cp ${QTCORE_FRAMEWORK} ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtCore ${APP_LIBRARY_DIR}/QtCore
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/QtCore
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libicui18n.55.dylib @executable_path/../Frameworks/libicui18n.55.dylib ${APP_LIBRARY_DIR}/QtCore
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libicuuc.55.dylib @executable_path/../Frameworks/libicuuc.55.dylib ${APP_LIBRARY_DIR}/QtCore
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libicudata.55.dylib @executable_path/../Frameworks/libicudata.55.dylib ${APP_LIBRARY_DIR}/QtCore
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libpcre16.0.dylib @executable_path/../Frameworks/libpcre16.0.dylib ${APP_LIBRARY_DIR}/QtCore
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libglib-2.0.0.dylib @executable_path/../Frameworks/libglib-2.0.0.dylib ${APP_LIBRARY_DIR}/QtCore
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libgthread-2.0.0.dylib @executable_path/../Frameworks/libgthread-2.0.0.dylib ${APP_LIBRARY_DIR}/QtCore
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libintl.8.dylib @executable_path/../Frameworks/libintl.8.dylib ${APP_LIBRARY_DIR}/QtCore

QtPrintSupport: QtCore QtGui QtWidgets
	cp ${QTPRINT_FRAMEWORK} ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtPrintSupport ${APP_LIBRARY_DIR}/QtPrintSupport
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_LIBRARY_DIR}/QtPrintSupport
	install_name_tool -change ${QTGUI_FRAMEWORK} @executable_path/../Frameworks/QtGui ${APP_LIBRARY_DIR}/QtPrintSupport
	install_name_tool -change ${QTWIDGETS_FRAMEWORK} @executable_path/../Frameworks/QtWidgets ${APP_LIBRARY_DIR}/QtPrintSupport
	
libz:
	cp ${SYSTEM_LIBRARY_DIR}/libz.1.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libz.1.dylib

libicu:
	cp ${SYSTEM_LIBRARY_DIR}/libicudata.55.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libicudata.55.dylib ${APP_LIBRARY_DIR}/libicudata.55.dylib
	cp ${SYSTEM_LIBRARY_DIR}/libicuuc.55.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libicuuc.55.dylib ${APP_LIBRARY_DIR}/libicuuc.55.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libicudata.55.dylib @executable_path/../Frameworks/libicudata.55.dylib ${APP_LIBRARY_DIR}/libicuuc.55.dylib
	cp ${SYSTEM_LIBRARY_DIR}/libicui18n.55.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libicui18n.55.dylib ${APP_LIBRARY_DIR}/libicui18n.55.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libicudata.55.dylib @executable_path/../Frameworks/libicudata.55.dylib ${APP_LIBRARY_DIR}/libicui18n.55.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libicuuc.55.dylib @executable_path/../Frameworks/libicuuc.55.dylib ${APP_LIBRARY_DIR}/libicui18n.55.dylib

libpcre:
	cp ${SYSTEM_LIBRARY_DIR}/libpcre16.0.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libpcre16.0.dylib ${APP_LIBRARY_DIR}/libpcre16.0.dylib

libgthread:
	cp ${SYSTEM_LIBRARY_DIR}/libgthread-2.0.0.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libgthread-2.0.0.dylib ${APP_LIBRARY_DIR}/libgthread-2.0.0.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libintl.8.dylib @executable_path/../Frameworks/libintl.8.dylib ${APP_LIBRARY_DIR}/libgthread-2.0.0.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libglib-2.0.0.dylib @executable_path/../Frameworks/libglib-2.0.0.dylib ${APP_LIBRARY_DIR}/libgthread-2.0.0.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libiconv.2.dylib @executable_path/../Frameworks/libiconv.2.dylib ${APP_LIBRARY_DIR}/libgthread-2.0.0.dylib

libglib:
	cp ${SYSTEM_LIBRARY_DIR}/libglib-2.0.0.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libglib-2.0.0.dylib ${APP_LIBRARY_DIR}/libglib-2.0.0.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libintl.8.dylib @executable_path/../Frameworks/libintl.8.dylib ${APP_LIBRARY_DIR}/libglib-2.0.0.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libiconv.2.dylib @executable_path/../Frameworks/libiconv.2.dylib ${APP_LIBRARY_DIR}/libglib-2.0.0.dylib

libintl: libiconv
	cp ${SYSTEM_LIBRARY_DIR}/libintl.8.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libintl.8.dylib ${APP_LIBRARY_DIR}/libintl.8.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libiconv.2.dylib @executable_path/../Frameworks/libiconv.2.dylib ${APP_LIBRARY_DIR}/libintl.8.dylib

libiconv:
	cp ${SYSTEM_LIBRARY_DIR}/libiconv.2.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libiconv.2.dylib ${APP_LIBRARY_DIR}/libiconv.2.dylib

libpng: libz
	cp ${SYSTEM_LIBRARY_DIR}/libpng16.16.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libpng16.16.dylib ${APP_LIBRARY_DIR}/libpng16.16.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libpng16.16.dylib

libbz2:
	cp ${SYSTEM_LIBRARY_DIR}/libbz2.1.0.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libbz2.1.0.dylib ${APP_LIBRARY_DIR}/libbz2.1.0.dylib

