#
# physicalModel.pro
# 

SOFA_DIR = ../..
TEMPLATE = lib
include($${SOFA_DIR}/sofa.cfg)

TARGET = physicalmodel$$LIBSUFFIX
CONFIG += $$CONFIGLIBRARIES
LIBS *= $$SOFA_LIBS

# INCLUDEPATH += ./PhysicalProperties 


# --------------- Sources --------------------
SOURCES += Atom.cpp \
           BasicAtomProperties.cpp \
           Cell.cpp \
           Component.cpp \
           BasicCellProperties.cpp \
           MultiComponent.cpp \
           PhysicalModel.cpp \
           StructuralComponent.cpp \
           Structure.cpp \
           StructureProperties.cpp \
           PMLTransform.cpp \
           PhysicalProperties/AtomProperties.cpp \
           PhysicalProperties/CellProperties.cpp \
           PhysicalProperties/StructuralComponentProperties.cpp \
           Properties.cpp \
           RenderingMode.cpp

HEADERS += Atom.h \
           BasicAtomProperties.h \
           Cell.h \
           BasicCellProperties.h \
           Component.h \
           MultiComponent.h \
           PhysicalModel.h \
           PhysicalModelIO.h \
           Properties.h \
           StructuralComponent.h \
           BasicSCProperties.h \
           Structure.h \
           StructureProperties.h \
           AbortException.h \
           PMLTransform.h \
           RenderingMode.h \
           PhysicalProperties/AtomProperties.h \
           PhysicalProperties/CellProperties.h 



