SOFA_DIR =../../..
TEMPLATE = lib
TARGET = sofaframe
include($${SOFA_DIR}/sofa.cfg)

DESTDIR = $$SOFA_DIR/lib/sofa-plugins

CONFIG += $$CONFIGLIBRARIES

!contains(CONFIGSTATIC, static) {
	CONFIG -= staticlib
CONFIG += dll
}

DEFINES += SOFA_BUILD_FRAME

LIBS += $$SOFA_FRAMEWORK_LIBS
LIBS += $$SOFA_EXT_LIBS
LIBS += -lsofasimulation$$LIBSUFFIX
LIBS += -lsofatree$$LIBSUFFIX
LIBS += -lsofacomponentprojectiveconstraintset$$LIBSUFFIX
LIBS += -lsofacomponentvisualmodel$$LIBSUFFIX
LIBS += -lsofacomponentbase$$LIBSUFFIX

# Make sure there are no cross-dependencies
INCLUDEPATH -= $$SOFA_DIR/applications

HEADERS += \
           initFrame.h \
           AffineTypes.h \
           DualQuatTypes.h \
           QuadraticTypes.h \
           CorotationalForceField.h \
           CorotationalForceField.inl \
           GreenLagrangeForceField.h \
           GreenLagrangeForceField.inl \
           FrameVolumePreservationForceField.h \
           FrameVolumePreservationForceField.inl \
           DeformationGradientTypes.h \
           MappingTypes.h \
           FrameBlendingMapping.h \
           FrameBlendingMapping.inl \
           LinearBlendTypes.inl \
           DualQuatBlendTypes.inl \
           FrameConstantForceField.h \
           FrameMass.h \
           FrameDiagonalMass.h \
           FrameDiagonalMass.inl \
           FrameFixedConstraint.h \
           FrameFixedConstraint.inl \
           FrameLinearMovementConstraint.h \
           FrameRigidConstraint.h \
           FrameRigidConstraint.inl \
           FrameMechanicalObject.h \
           GridMaterial.h \
           GridMaterial.inl \
           MeshGenerater.h \
           MeshGenerater.inl \
           NewMaterial.h \
           NewHookeMaterial.h \
           NewHookeMaterial.inl
           #TetrahedronFEMForceFieldWithExternalMaterial.h \
           #TetrahedronFEMForceFieldWithExternalMaterial.inl
SOURCES += \
           initFrame.cpp \
           CorotationalForceField.cpp \
           GreenLagrangeForceField.cpp \
           FrameVolumePreservationForceField.cpp \
           FrameBlendingMapping.cpp \
           FrameConstantForceField.cpp \
           FrameDiagonalMass.cpp \
           FrameFixedConstraint.cpp \
           FrameLinearMovementConstraint.cpp \
           FrameRigidConstraint.cpp \
           FrameMechanicalObject.cpp \
           GridMaterial.cpp \
           MeshGenerater.cpp \
           NewHookeMaterial.cpp
           #TetrahedronFEMForceFieldWithExternalMaterial.cpp

contains(DEFINES,SOFA_GPU_CUDA){ # BEGIN SOFA_GPU_CUDA
LIBS += -lsofagpucuda$$LIBSUFFIX

HEADERS += \
           HexaRemover.h \
           HexaRemover.inl

SOURCES += \
           HexaRemover.cpp
}
