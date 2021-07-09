/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
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
#define SOFA_COMPONENT_ENGINE_MERGEVECTORS_CPP
#include <SofaGeneralEngine/MergeVectors.inl>
#include <sofa/core/ObjectFactory.h>

namespace sofa::component::engine
{

int MergeVectorsClass = core::RegisterObject("Apply a merge operation to combine several inputs")
    .add< MergeVectors< type::vector<double> > >(true)
    .add< MergeVectors< type::vector<int> > >()
    .add< MergeVectors< type::vector<bool> > >()
    //.add< MergeVectors< type::vector<std::string> > >()
    .add< MergeVectors< type::vector<type::Vec2u> > >()
    .add< MergeVectors< type::vector<type::Vec2d> > >()
    .add< MergeVectors< type::vector<type::Vec3d> > >()
    .add< MergeVectors< type::vector<type::Vec4d> > >()
    .add< MergeVectors< defaulttype::Rigid2Types::VecCoord > >()
    .add< MergeVectors< defaulttype::Rigid2Types::VecDeriv > >()
    .add< MergeVectors< defaulttype::Rigid3Types::VecCoord > >()
    .add< MergeVectors< defaulttype::Rigid3Types::VecDeriv > >()
 
        ;

template class SOFA_SOFAGENERALENGINE_API MergeVectors< type::vector<int> >;
template class SOFA_SOFAGENERALENGINE_API MergeVectors< type::vector<bool> >;
//template class SOFA_SOFAGENERALENGINE_API MergeVectors< type::vector<std::string> >;
template class SOFA_SOFAGENERALENGINE_API MergeVectors< type::vector<type::Vec2u> >;

template class SOFA_SOFAGENERALENGINE_API MergeVectors< type::vector<double> >;
template class SOFA_SOFAGENERALENGINE_API MergeVectors< type::vector<type::Vec2d> >;
template class SOFA_SOFAGENERALENGINE_API MergeVectors< type::vector<type::Vec3d> >;
template class SOFA_SOFAGENERALENGINE_API MergeVectors< type::vector<type::Vec4d> >;
template class SOFA_SOFAGENERALENGINE_API MergeVectors< defaulttype::Rigid2Types::VecCoord >;
template class SOFA_SOFAGENERALENGINE_API MergeVectors< defaulttype::Rigid2Types::VecDeriv >;
template class SOFA_SOFAGENERALENGINE_API MergeVectors< defaulttype::Rigid3Types::VecCoord >;
template class SOFA_SOFAGENERALENGINE_API MergeVectors< defaulttype::Rigid3Types::VecDeriv >;
 


} //namespace sofa::component::engine
