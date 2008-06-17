SOFA_DIR=../../../..
TEMPLATE = app

include($${SOFA_DIR}/sofa.cfg)

TARGET = Modeler$$SUFFIX
DESTDIR = $$SOFA_DIR/bin
CONFIG += $$CONFIGPROJECTGUI 
LIBS += $$SOFA_GUI_LIBS
LIBS += $$SOFA_LIBS
LIBS += -lsofamodeler$$LIBSUFFIX

macx : {
	CONFIG +=app_bundle
	RC_FILE = Modeler.icns
	QMAKE_INFO_PLIST = Info.plist
        QMAKE_BUNDLE_DATA += $$APP_BUNDLE_DATA
#	QMAKE_POST_LINK = cp -r ../../../../share/* ../../../../bin/Modeler$$SUFFIX.app/Contents/Resources/.
}

# The following is a workaround to get KDevelop to detect the name of the program to start
unix {
	!macx: QMAKE_POST_LINK = ln -sf Modeler$$SUFFIX $$DESTDIR/Modeler-latest
}

# The following create enables to start runSofa from the command line as well as graphically
macx {
	QMAKE_POST_LINK = ln -sf runSofa.app/Contents/MacOS/Modeler$$SUFFIX $$DESTDIR/Modeler$$SUFFIX
}

!macx : RC_FILE = sofa.rc

SOURCES = Main.cpp
HEADERS = 
