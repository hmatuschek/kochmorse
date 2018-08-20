SYSTEM_LIBRARY_DIR = /opt/local/lib
SYSTEM_BINARY_DIR = /opt/local/bin
QT_BINARY_DIR = /opt/local/libexec/qt5/bin
QTCORE_FRAMEWORK = /opt/local/libexec/qt5/lib/QtCore.framework/Versions/5/QtCore
QTGUI_FRAMEWORK = /opt/local/libexec/qt5/lib/QtGui.framework/Versions/5/QtGui
QTMULTIMEDIA_FRAMEWORK = /opt/local/QtMultimedia.framework/Versions/5/QtMultimedia
QTWIDGETS_FRAMEWORK = /opt/local/libexec/qt5/lib/QtWidgets.framework/Versions/5/QtWidgets
QTXML_FRAMEWORK = /opt/local/libexec/qt5/lib/QtXml.framework/Versions/5/QtXml
QTSVG_FRAMEWORK = /opt/local/libexec/qt5/lib/QtSvg.framework/Versions/5/QtSvg
QTPRINT_FRAMEWORK = /opt/local/libexec/qt5/lib/QtPrintSupport.framework/Versions/5/QtPrintSupport
QT_PLUGINS = /opt/local/libexec/qt5/plugins
QT_PLUGINS_PLATFORM =${QT_PLUGINS}/platforms
QT_PLUGINS_PRINT =${QT_PLUGINS}/printsupport
QT_PLUGINS_ACCESSIBLE = ${QT_PLUGINS}/accessible
QT_PLUGINS_IMAGE =${QT_PLUGINS}/imageformats
APP_LIBRARY_DIR = KochMorse.app/Contents/Frameworks
APP_BINARY_DIR  = KochMorse.app/Contents/MacOS
APP_PLUGIN_DIR  = KochMorse.app/Contents/PlugIns
APP_RESOURCE_DIR  = KochMorse.app/Contents/Resources
VERSION = 3.3.0

app: dmg

clean:
	rm -f ${APP_BINARY_DIR}/kochmorse
	rm -f ${APP_LIBRARY_DIR}/*
	rm -f KochMorse-${VERSION}.dmg

dmg: kochmorse-app
	cp kochmorse.icns ${APP_RESOURCE_DIR}
	hdiutil create -srcfolder KochMorse.app kochmorse-${VERSION}.dmg


kochmorse-app: 
	cp $(SYSTEM_BINARY_DIR)/kochmorse ${APP_BINARY_DIR}
	${QT_BINARY_DIR}/macdeployqt KochMorse.app

