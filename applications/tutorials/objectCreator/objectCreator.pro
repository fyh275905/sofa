SOFA_DIR=../../..
TEMPLATE = lib
TARGET = sofaobjectcreator$$LIBSUFFIX

include($${SOFA_DIR}/sofa.cfg)

CONFIG += $$CONFIGLIBRARIES
LIBS += $$SOFA_LIBS
LIBS += $$SOFA_EXT_LIBS

INCLUDEPATH += $$SOFA_DIR/extlibs

SOURCES = ObjectCreator.cpp

HEADERS = ObjectCreator.h
