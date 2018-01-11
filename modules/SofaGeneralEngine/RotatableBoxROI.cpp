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
#define SOFA_COMPONENT_ENGINE_ROTATABLEBOXROI_CPP
#include <SofaGeneralEngine/RotatableBoxROI.inl>
#include <sofa/core/ObjectFactory.h>
#include <sofa/defaulttype/Vec3Types.h>

namespace sofa
{

namespace component
{

namespace engine
{


using namespace sofa::defaulttype;

SOFA_DECL_CLASS(RotatableBoxROI)

int RotatableBoxROIClass = core::RegisterObject("Find the primitives (vertex/edge/triangle/quad/tetrahedron/hexahedron) inside given rotated boxes")
#ifdef SOFA_WITH_DOUBLE
        .add< RotatableBoxROI<Vec3dTypes> >(true) //default
        .add< RotatableBoxROI<Rigid3dTypes> >()
        .add< RotatableBoxROI<Vec6dTypes> >()
#endif //SOFA_WITH_DOUBLE
#ifdef SOFA_WITH_FLOAT
        .add< RotatableBoxROI<Vec3fTypes> >()
        .add< RotatableBoxROI<Rigid3fTypes> >()
        .add< RotatableBoxROI<Vec6fTypes> >()
#endif //SOFA_WITH_FLOAT
        ;

#ifdef SOFA_WITH_DOUBLE
template class SOFA_ENGINE_API RotatableBoxROI<Vec3dTypes>;
template class SOFA_ENGINE_API RotatableBoxROI<Rigid3dTypes>;
template class SOFA_ENGINE_API RotatableBoxROI<Vec6dTypes>;
#endif // SOFA_WITH_DOUBLE
#ifdef SOFA_WITH_FLOAT
template class SOFA_ENGINE_API RotatableBoxROI<Vec3fTypes>;
template class SOFA_ENGINE_API RotatableBoxROI<Rigid3fTypes>;
template class SOFA_ENGINE_API RotatableBoxROI<Vec6fTypes>;
#endif //SOFA_WITH_FLOAT


} // namespace constraint

} // namespace component

} // namespace sofa

