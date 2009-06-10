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
#ifndef SOFA_COMPONENT_FORCEFIELD_QUADBENDINGSPRINGS_INL
#define SOFA_COMPONENT_FORCEFIELD_QUADBENDINGSPRINGS_INL

#include <sofa/component/forcefield/QuadBendingSprings.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <iostream>

namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace core::componentmodel::behavior;
using sofa::component::container::MechanicalObject;

template<class DataTypes>
QuadBendingSprings<DataTypes>::QuadBendingSprings()
    : localRange( initData(&localRange, defaulttype::Vec<2,int>(-1,-1), "localRange", "optional range of local DOF indices. Any computation involving only indices outside of this range are discarded (useful for parallelization using mesh partitionning)" ) )
    , dof(NULL)
{
}


template<class DataTypes>
QuadBendingSprings<DataTypes>::~QuadBendingSprings()
{}

template<class DataTypes>
void QuadBendingSprings<DataTypes>::addSpring( unsigned a, unsigned b, std::set<IndexPair>& springSet )
{
    if (localRange.getValue()[0] >= 0)
    {
        if ((int)a < localRange.getValue()[0] && (int)b < localRange.getValue()[0]) return;
    }
    if (localRange.getValue()[1] >= 0)
    {
        if ((int)a > localRange.getValue()[1] && (int)b > localRange.getValue()[1]) return;
    }
    IndexPair ab(a<b?a:b, a<b?b:a);
    if (springSet.find(ab) != springSet.end()) return;
    springSet.insert(ab);
    const VecCoord& x = *dof->getX();
    Real s = (Real)this->ks.getValue();
    Real d = (Real)this->kd.getValue();
    Real l = (x[a]-x[b]).norm();
    this->SpringForceField<DataTypes>::addSpring(a,b, s, d, l );
    //sout<<"=================================QuadBendingSprings<DataTypes>::addSpring "<<a<<", "<<b<<sendl;
}

template<class DataTypes>
void QuadBendingSprings<DataTypes>::registerEdge( IndexPair ab, IndexPair cd, std::map<IndexPair, IndexPair>& edgeMap, std::set<IndexPair>& springSet)
{
    if (ab.first > ab.second)
    {
        ab = std::make_pair(ab.second,ab.first);
        cd = std::make_pair(cd.second,cd.first);
    }
    if (edgeMap.find(ab) == edgeMap.end())
    {
        edgeMap[ab] = cd;
    }
    else
    {
        // create a spring between the opposite
        IndexPair ef = edgeMap[ab];
        this->addSpring(cd.first, ef.first, springSet);
        this->addSpring(cd.second, ef.second, springSet);
    }
}

template<class DataTypes>
void QuadBendingSprings<DataTypes>::init()
{
    dof = dynamic_cast<MechanicalObject<DataTypes>*>( this->getContext()->getMechanicalState() );
    assert(dof);
    //sout<<"==================================QuadBendingSprings<DataTypes>::init(), dof size = "<<dof->getX()->size()<<sendl;

    // Set the bending springs

    std::map< IndexPair, IndexPair > edgeMap;
    std::set< IndexPair > springSet;

    sofa::core::componentmodel::topology::BaseMeshTopology* topology = this->getContext()->getMeshTopology();
    assert( topology );

    const sofa::core::componentmodel::topology::BaseMeshTopology::SeqQuads& quads = topology->getQuads();
    //sout<<"==================================QuadBendingSprings<DataTypes>::init(), quads size = "<<quads.size()<<sendl;
    for( unsigned i= 0; i<quads.size(); ++i )
    {
        const sofa::core::componentmodel::topology::BaseMeshTopology::Quad& face = quads[i];
        {
            registerEdge( std::make_pair(face[0], face[1]), std::make_pair(face[3], face[2]), edgeMap, springSet );
            registerEdge( std::make_pair(face[1], face[2]), std::make_pair(face[0], face[3]), edgeMap, springSet );
            registerEdge( std::make_pair(face[2], face[3]), std::make_pair(face[1], face[0]), edgeMap, springSet );
            registerEdge( std::make_pair(face[3], face[0]), std::make_pair(face[2], face[1]), edgeMap, springSet );
        }
    }

    // init the parent class
    StiffSpringForceField<DataTypes>::init();
}


} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
