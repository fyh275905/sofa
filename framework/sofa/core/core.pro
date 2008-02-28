# File generate by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./framework/sofa/core
# Target is a library:  sofacore$$LIBSUFFIX

HEADERS += \
          BaseMapping.h \
          BehaviorModel.h \
          CollisionElement.h \
          CollisionModel.h \
          componentmodel/behavior/BaseConstraint.h \
          componentmodel/behavior/BaseConstraintCorrection.h \
          componentmodel/behavior/BaseForceField.h \
          componentmodel/behavior/BaseMass.h \
          componentmodel/behavior/BaseMechanicalMapping.h \
          componentmodel/behavior/BaseMechanicalState.h \
          componentmodel/behavior/Constraint.h \
          componentmodel/behavior/Constraint.inl \
          componentmodel/behavior/ForceField.h \
          componentmodel/behavior/ForceField.inl \
          componentmodel/behavior/InteractionForceField.h \
          componentmodel/behavior/InteractionConstraint.h \
          componentmodel/behavior/PairInteractionForceField.h \
          componentmodel/behavior/PairInteractionForceField.inl \
	  componentmodel/behavior/MixedInteractionForceField.h \
	  componentmodel/behavior/MixedInteractionForceField.inl \
          componentmodel/behavior/PairInteractionConstraint.h \
          componentmodel/behavior/PairInteractionConstraint.inl \
	  componentmodel/behavior/MixedInteractionConstraint.h \
	  componentmodel/behavior/MixedInteractionConstraint.inl \
	  componentmodel/behavior/MappedModel.h \
          componentmodel/behavior/Mass.h \
          componentmodel/behavior/Mass.inl \
          componentmodel/behavior/MasterSolver.h \
          componentmodel/behavior/MechanicalMapping.h \
          componentmodel/behavior/MechanicalMapping.inl \
          componentmodel/behavior/MechanicalState.h \
          componentmodel/behavior/OdeSolver.h \
          componentmodel/collision/BroadPhaseDetection.h \
          componentmodel/collision/CollisionAlgorithm.h \
          componentmodel/collision/CollisionGroupManager.h \
          componentmodel/collision/Contact.h \
          componentmodel/collision/ContactManager.h \
          componentmodel/collision/Detection.h \
          componentmodel/collision/DetectionOutput.h \
          componentmodel/collision/Intersection.h \
          componentmodel/collision/Intersection.inl \
          componentmodel/collision/NarrowPhaseDetection.h \
          componentmodel/collision/Pipeline.h \
          componentmodel/topology/BaseTopology.h \
          componentmodel/topology/TopologicalMapping.h \
          componentmodel/topology/Topology.h \
          Mapping.h \
          Mapping.inl \
          objectmodel/Base.h \
          objectmodel/BaseContext.h \
          objectmodel/BaseNode.h \
          objectmodel/BaseObject.h \
          objectmodel/BaseObjectDescription.h \
          objectmodel/ClassInfo.h \
          objectmodel/Context.h \
          objectmodel/ContextObject.h \
          objectmodel/Data.h \
          objectmodel/DetachNodeEvent.h \
          objectmodel/Event.h \
          objectmodel/DataPtr.h \
          objectmodel/BaseData.h \
          objectmodel/KeypressedEvent.h \
	  objectmodel/KeyreleasedEvent.h \
	  objectmodel/MouseEvent.h \
          objectmodel/VDataPtr.h \
          objectmodel/XDataPtr.h \
          VisualModel.h \
          ObjectFactory.h
 
SOURCES +=  \
          CollisionModel.cpp \
          componentmodel/behavior/MasterSolver.cpp \
          componentmodel/behavior/OdeSolver.cpp \
          componentmodel/collision/Contact.cpp \
          componentmodel/collision/Intersection.cpp \
          componentmodel/collision/Pipeline.cpp \
          componentmodel/topology/BaseTopology.cpp \
          objectmodel/Base.cpp \
          objectmodel/BaseContext.cpp \
          objectmodel/BaseObject.cpp \
          objectmodel/BaseObjectDescription.cpp \
          objectmodel/ClassInfo.cpp \
          objectmodel/Context.cpp \
          objectmodel/DetachNodeEvent.cpp \
          objectmodel/Event.cpp \
          objectmodel/DataPtr.cpp \
          objectmodel/KeypressedEvent.cpp \
          objectmodel/KeyreleasedEvent.cpp \
	  objectmodel/MouseEvent.cpp \
          ObjectFactory.cpp \
          Mapping.cpp \
          componentmodel/behavior/BaseConstraint.cpp \
          componentmodel/behavior/BaseForceField.cpp \
          componentmodel/behavior/ForceField.cpp \
          componentmodel/behavior/Mass.cpp \
          componentmodel/behavior/MechanicalMapping.cpp \
          componentmodel/behavior/PairInteractionForceField.cpp \
          componentmodel/behavior/MixedInteractionForceField.cpp \
          componentmodel/behavior/PairInteractionConstraint.cpp \
          componentmodel/behavior/MixedInteractionConstraint.cpp 

SOFA_DIR = ../../..
TEMPLATE = lib
include($${SOFA_DIR}/sofa.cfg)
TARGET = sofacore$$LIBSUFFIX
CONFIG += $$CONFIGLIBRARIES
LIBS += -lsofahelper$$LIBSUFFIX -lsofadefaulttype$$LIBSUFFIX
LIBS += $$SOFA_EXT_LIBS

# Make sure there are no cross-dependencies
INCLUDEPATH -= $$SOFA_DIR/modules
INCLUDEPATH -= $$SOFA_DIR/applications
