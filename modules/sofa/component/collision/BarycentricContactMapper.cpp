/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#include <sofa/component/collision/BarycentricContactMapper.inl>
#include <sofa/helper/Factory.inl>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace defaulttype;

SOFA_DECL_CLASS(BarycentricContactMapper)

ContactMapperCreator< ContactMapper<SphereModel> > SphereContactMapperClass("default",true);
ContactMapperCreator< ContactMapper<SphereTreeModel> > SphereTreeContactClass("default", true);
ContactMapperCreator< ContactMapper<PointModel> > PointContactMapperClass("default",true);
ContactMapperCreator< ContactMapper<LineModel> > LineContactMapperClass("default",true);
ContactMapperCreator< ContactMapper<TriangleModel> > TriangleContactMapperClass("default",true);
ContactMapperCreator< ContactMapper<TetrahedronModel> > TetrahedronContactMapperClass("default",true);
ContactMapperCreator< ContactMapper<RigidDistanceGridCollisionModel> > DistanceGridContactMapperClass("default", true);
ContactMapperCreator< ContactMapper<FFDDistanceGridCollisionModel> > FFDDistanceGridContactMapperClass("default", true);

} // namespace collision

} // namespace component

} // namespace sofa

