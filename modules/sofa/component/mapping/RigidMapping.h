/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_MAPPING_RIGIDMAPPING_H
#define SOFA_COMPONENT_MAPPING_RIGIDMAPPING_H

#include <sofa/core/componentmodel/behavior/MechanicalMapping.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/componentmodel/behavior/MappedModel.h>
#include <sofa/component/component.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/core/objectmodel/DataFileName.h>
#include <vector>

namespace sofa
{

namespace component
{

namespace mapping
{

/// This class can be overridden if needed for additionnal storage within template specializations.
template<class InDataTypes, class OutDataTypes>
class RigidMappingInternalData
{
public:
};


template <class BasicMapping>
class RigidMapping : public BasicMapping, public virtual core::objectmodel::BaseObject
{
public:
    typedef BasicMapping Inherit;
    typedef typename Inherit::In In;
    typedef typename Inherit::Out Out;
    typedef typename Out::VecCoord VecCoord;
    typedef typename Out::VecDeriv VecDeriv;
    typedef typename Out::Coord Coord;
    typedef typename Out::Deriv Deriv;
    typedef typename In::Deriv InDeriv;
    typedef typename defaulttype::SparseConstraint<Deriv> OutSparseConstraint;
    typedef typename OutSparseConstraint::const_data_iterator OutConstraintIterator;
    typedef typename Coord::value_type Real;
    enum { N=Coord::static_size };
    typedef defaulttype::Mat<N,N,Real> Mat;
    typedef defaulttype::Vec<N,Real> Vector ;

    Data< VecCoord > points;
    VecCoord rotatedPoints;
    RigidMappingInternalData<typename In::DataTypes, typename Out::DataTypes> data;
    Data<unsigned int> index;
    sofa::core::objectmodel::DataFileName fileRigidMapping;
    Data< bool > useX0;
    Data< bool > indexFromEnd;

    core::componentmodel::behavior::BaseMechanicalState::ParticleMask* maskFrom;
    core::componentmodel::behavior::BaseMechanicalState::ParticleMask* maskTo;


    RigidMapping ( In* from, Out* to )
        : Inherit ( from, to ),
          points ( initData ( &points,"initialPoints", "Local Coordinates of the points" ) ),
          index ( initData ( &index, ( unsigned ) 0,"index","input DOF index" ) ),
          fileRigidMapping ( initData ( &fileRigidMapping,"fileRigidMapping","Filename" ) ),
          useX0( initData ( &useX0,false,"useX0","Use x0 instead of local copy of initial positions (to support topo changes)") ),
          indexFromEnd( initData ( &indexFromEnd,false,"indexFromEnd","input DOF index starts from the end of input DOFs vector") ),
          repartition ( initData ( &repartition,"repartition","number of dest dofs per entry dof" ) )
    {
        addAlias(&fileRigidMapping,"filename");
        maskFrom = NULL;
        if (core::componentmodel::behavior::BaseMechanicalState *stateFrom = dynamic_cast< core::componentmodel::behavior::BaseMechanicalState *>(from))
            maskFrom = &stateFrom->forceMask;
        maskTo = NULL;
        if (core::componentmodel::behavior::BaseMechanicalState *stateTo = dynamic_cast< core::componentmodel::behavior::BaseMechanicalState *>(to))
            maskTo = &stateTo->forceMask;
    }

    virtual ~RigidMapping()
    {}

    int addPoint ( const Coord& c );
    int addPoint ( const Coord& c, int indexFrom );

    void init();

    //void disable(); //useless now that points are saved in a Data

    virtual void apply ( typename Out::VecCoord& out, const typename In::VecCoord& in );

    virtual void applyJ ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );

    virtual void applyJT ( typename In::VecDeriv& out, const typename Out::VecDeriv& in );

    void applyJT ( typename In::VecConst& out, const typename Out::VecConst& in );

    void draw();

    void clear ( int reserve=0 );

    void setRepartition ( unsigned int value );
    void setRepartition ( sofa::helper::vector<unsigned int> values );

protected:
    class Loader;
    void load ( const char* filename );
    Data<sofa::helper::vector<unsigned int> >  repartition;
    const VecCoord& getPoints();
};

using core::Mapping;
using core::componentmodel::behavior::MechanicalMapping;
using core::componentmodel::behavior::MappedModel;
using core::componentmodel::behavior::State;
using core::componentmodel::behavior::MechanicalState;

using sofa::defaulttype::Vec2dTypes;
using sofa::defaulttype::Vec3dTypes;
using sofa::defaulttype::Vec2fTypes;
using sofa::defaulttype::Vec3fTypes;
using sofa::defaulttype::ExtVec2fTypes;
using sofa::defaulttype::ExtVec3fTypes;
using sofa::defaulttype::Rigid2dTypes;
using sofa::defaulttype::Rigid3dTypes;
using sofa::defaulttype::Rigid2fTypes;
using sofa::defaulttype::Rigid3fTypes;

#if defined(WIN32) && !defined(SOFA_COMPONENT_MAPPING_RIGIDMAPPING_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< MechanicalMapping<MechanicalState<Rigid3dTypes>, MechanicalState<Vec3dTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< MechanicalMapping<MechanicalState<Rigid2dTypes>, MechanicalState<Vec2dTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< Mapping< State<Rigid3dTypes>, MappedModel<Vec3dTypes> > >;
// extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< Mapping< State<Rigid3dTypes>, MappedModel<ExtVec3dTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< Mapping< State<Rigid3dTypes>, MappedModel<ExtVec3fTypes> > >;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< MechanicalMapping<MechanicalState<Rigid3fTypes>, MechanicalState<Vec3fTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< MechanicalMapping<MechanicalState<Rigid2fTypes>, MechanicalState<Vec2fTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< Mapping< State<Rigid3fTypes>, MappedModel<Vec3fTypes> > >;
// extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< Mapping< State<Rigid3fTypes>, MappedModel<ExtVec3dTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< Mapping< State<Rigid3fTypes>, MappedModel<ExtVec3fTypes> > >;
#endif

#ifndef SOFA_FLOAT
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< MechanicalMapping<MechanicalState<Rigid3dTypes>, MechanicalState<Vec3fTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< MechanicalMapping<MechanicalState<Rigid3fTypes>, MechanicalState<Vec3dTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< MechanicalMapping<MechanicalState<Rigid2dTypes>, MechanicalState<Vec2fTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< MechanicalMapping<MechanicalState<Rigid2fTypes>, MechanicalState<Vec2dTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< Mapping< State<Rigid3dTypes>, MappedModel<Vec3fTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API RigidMapping< Mapping< State<Rigid3fTypes>, MappedModel<Vec3dTypes> > >;
#endif
#endif
#endif

} // namespace mapping

} // namespace component

} // namespace sofa

#endif
