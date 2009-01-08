SOFA_DIR =.
TEMPLATE = subdirs

include($${SOFA_DIR}/sofa.cfg) 

SUBDIRS += extlibs/newmat

contains(DEFINES,SOFA_DEV){ # BEGIN SOFA_DEV
#SUBDIRS += extlibs/SLC
} # END SOFA_DEV

SUBDIRS += extlibs/qwt

# FlowVR
	SUBDIRS += extlibs/miniFlowVR
contains(DEFINES,SOFA_HAVE_FLOWVR){
	SUBDIRS -= extlibs/miniFlowVR
}

#QGLViewer

contains(DEFINES,SOFA_GUI_QGLVIEWER){
	SUBDIRS += extlibs/libQGLViewer/QGLViewer
}

contains(DEFINES,SOFA_HAVE_COLLADADOM){
	SUBDIRS += extlibs/colladadom/dom/colladadom.pro
}

# PML
	SUBDIRS += extlibs/PML
	SUBDIRS += extlibs/LML
!contains(DEFINES,SOFA_PML){
	SUBDIRS -= extlibs/PML
	SUBDIRS -= extlibs/LML
}

SUBDIRS += framework
SUBDIRS += modules
SUBDIRS += applications

# Print current config

message( "====== SOFA Build Configuration ======")

contains(DEFINES,SOFA_DEV){ # BEGIN SOFA_DEV
message( "==== UNSTABLE DEVELOPMENT VERSION ====")
} # END SOFA_DEV

win32 {
  message( "|  Platform: Windows")
}
else:macx {
  message( "|  Platform: MacOS")
}
else:unix {
  message( "|  Platform: Linux/Unix")
}

contains (CONFIGDEBUG, debug) {
  message( "|  Mode: DEBUG")
}
contains (CONFIGDEBUG, release) {
  contains (QMAKE_CXXFLAGS,-g) {
    message( "|  Mode: RELEASE with debug symbols")
  }
  else {
    contains (CONFIGDEBUG, profile) {
      message( "|  Mode: RELEASE with profiling")
    }
    else {
      message( "|  Mode: RELEASE")
    }
  }
}

contains(DEFINES,SOFA_QT4) {
  message( "|  Qt version: 4.x")
}
else {
  message( "|  Qt version: 3.x")
}

contains(DEFINES,SOFA_RDTSC) {
  message( "|  RDTSC timer: ENABLED")
}
else {
  message( "|  RDTSC timer: DISABLED")
}

contains(DEFINES,SOFA_HAVE_PNG) {
  message( "|  PNG support: ENABLED")
}
else {
  message( "|  PNG support: DISABLED")
}

contains(DEFINES,SOFA_HAVE_GLEW) {
  message( "|  OpenGL Extensions support using GLEW: ENABLED")
}
else {
  message( "|  OpenGL Extensions support using GLEW: DISABLED")
}

contains(DEFINES,SOFA_GPU_CUDA) {
  message( "|  GPU support using CUDA: ENABLED")
}
else {
  message( "|  GPU support using CUDA: DISABLED")
}

contains(DEFINES,SOFA_PML) {
  message( "|  PML/LML support: ENABLED")
}
else {
  message( "|  PML/LML support: DISABLED")
}


contains(DEFINES,SOFA_GUI_GLUT) {
  message( "|  GLUT GUI: ENABLED")
}
else {
  message( "|  GLUT GUI: DISABLED")
}

contains(DEFINES,SOFA_DEV){ # BEGIN SOFA_DEV
contains(DEFINES,SOFA_GUI_FLTK) {
  message( "|  FLTK GUI: ENABLED")
}
else {
  message( "|  FLTK GUI: DISABLED")
}
} # END SOFA_DEV

!contains(DEFINES,SOFA_GUI_QTVIEWER) {
!contains(DEFINES,SOFA_GUI_QGLVIEWER) {
!contains(DEFINES,SOFA_GUI_QTOGREVIEWER) {
  message( "|  Qt GUI: DISABLED")
}
else {
  message( "|  Qt GUI: ENABLED")
}
}
else {
  message( "|  Qt GUI: ENABLED")
}
}
else {
  message( "|  Qt GUI: ENABLED")
}

contains(DEFINES,SOFA_GUI_QTVIEWER) {
  message( "|  -  Qt OpenGL viewer: ENABLED")
}
else {
  message( "|  -  Qt OpenGL viewer: DISABLED")
}

contains(DEFINES,SOFA_GUI_QGLVIEWER) {
  message( "|  -  Qt QGLViewer viewer: ENABLED")
}
else {
  message( "|  -  Qt QGLViewer viewer: DISABLED")
}
contains(DEFINES,SOFA_GUI_QTOGREVIEWER) {
  message( "|  -  Qt OGRE 3D viewer: ENABLED")
}
else {
  message( "|  -  Qt OGRE 3D viewer: DISABLED")
}

message( "======================================")
