SOFA_DIR=../../../../..
TEMPLATE = app
TARGET = centerOfMassMapping

include($${SOFA_DIR}/sofa.cfg)

#Uncomment if you are compiling the boost kernel

DESTDIR = $$SOFA_DIR/bin
CONFIG += $$CONFIGPROJECTGUI
LIBS += $$SOFA_GUI_LIBS
LIBS += $$SOFA_LIBS
LIBS += -lsofaobjectcreator$$LIBSUFFIX

# The following is a workaround to get KDevelop to detect the name of the program to start
unix {
QMAKE_POST_LINK = ln -sf multimapping$$SUFFIX $$DESTDIR/centerOfMassMapping-latest
}


SOURCES = Main.cpp 
HEADERS = 
