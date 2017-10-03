/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2017 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include "deprecatedcomponents.h"
namespace sofa
{

namespace helper
{

namespace deprecatedcomponents
{

std::map<std::string, std::string>  messages =
{
    {"deprecated-17.12", " has been deprecated since sofa 17.12. Please consider updating your scene as using "
                         " deprecated component may result in poor performance and undefined behavior."
                         " If this component is crucial to you please report that to sofa-dev@ so we can  "
                         " reconsider this component for future re-integration. "
    },
    {"removed-17.12", " has been removed since sofa 17.12. Please consider updating your scene."
                      " If this component is crucial to you please report that to sofa-dev@ so we can  "
                      " reconsider this component for future re-integration. "
    },
} ;


////// Here is the list of component that are removed or deprecated.
/// Component name, the error message to use among
std::map<std::string, std::vector<std::string>> components =
{
    {"WashingMachineForceField", {"deprecated-17.12"}},

    {"CatmullRomSplineMapping.cpp", {"deprecated-17.12"}},
    {"CenterPointMechanicalMapping.cpp", {"deprecated-17.12"}},
    {"CurveMapping.cpp", {"deprecated-17.12"}},
    {"ExternalInterpolationMapping.cpp", {"deprecated-17.12"}},
    {"ProjectionToLineMapping", {"deprecated-17.12"}},
    {"ProjectionToTargetLineMapping_test", {"deprecated-17.12"}},
    {"ProjectionToPlaneMapping", {"deprecated-17.12"}},
    {"ProjectionToTargetPlaneMapping_test.cpp", {"deprecated-17.12"}},

    /// SofaUserInteraction
    {"AddRecordedCameraPerformer", {"deprecated-17.12"}},
    {"ArticulatedHierarchyBVHController", {"deprecated-17.12"}},
    {"ArticulatedHierarchyController", {"deprecated-17.12"}},
    {"CuttingPoint", {"deprecated-17.12"}},
    {"DisabledContact", {"deprecated-17.12"}},
    {"EdgeSetController", {"deprecated-17.12"}},
    {"FixParticlePerformer", {"deprecated-17.12"}},
    {"GraspingManager", {"deprecated-17.12"}},
    {"InciseAlongPathPerformer", {"deprecated-17.12"}},
    {"InterpolationController", {"deprecated-17.12"}},
    {"NodeToggleController", {"deprecated-17.12"}}
};

} /// namespace deprecatedcomponents

} /// namespace helper

} /// namespace sofa

