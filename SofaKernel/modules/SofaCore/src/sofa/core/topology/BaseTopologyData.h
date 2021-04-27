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
#ifndef SOFA_COMPONENT_TOPOLOGY_BASETOPOLOGYDATA_H
#define SOFA_COMPONENT_TOPOLOGY_BASETOPOLOGYDATA_H

#include <sofa/core/objectmodel/Data.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/core/topology/TopologyChange.h>

namespace sofa
{

namespace core
{

namespace topology
{

typedef Topology::Point            Point;
typedef Topology::Edge             Edge;
typedef Topology::Triangle         Triangle;
typedef Topology::Quad             Quad;
typedef Topology::Tetrahedron      Tetrahedron;
typedef Topology::Hexahedron       Hexahedron;

//TODO(dmarchal 2017-05-13):
// When someone want to deprecate something....please help other contributors by providing
// details on:
//   - why is deprecated
//   - when it have been deprecated
//   - when can we remove the classe
//   - how are we suppose to update classes that make use of BaseTopologyData
//   - who is supposed to do the update...and if it is not the person that deprecate the
//     code how your co-worker will be notified they have something to do.
/** A class that define topological Data general methods

      DEPRECATED

*/
template < class T = void* >
class BaseTopologyData : public sofa::core::objectmodel::Data <T>
{
public:
    //SOFA_CLASS(SOFA_TEMPLATE2(BaseTopologyData,T,VecT), SOFA_TEMPLATE(sofa::core::objectmodel::Data, T));

    class InitData : public sofa::core::objectmodel::BaseData::BaseInitData
    {
    public:
        InitData() : value(T()) {}
        InitData(const T& v) : value(v) {}
        InitData(const sofa::core::objectmodel::BaseData::BaseInitData& i) : sofa::core::objectmodel::BaseData::BaseInitData(i), value(T()) {}

        T value;
    };

    /** \copydoc Data(const BaseData::BaseInitData&) */
    explicit BaseTopologyData(const sofa::core::objectmodel::BaseData::BaseInitData& init)
        : Data<T>(init)
    {
    }

    /** \copydoc Data(const InitData&) */
    explicit BaseTopologyData(const InitData& init)
        : Data<T>(init)
    {
    }


    /** \copydoc Data(const char*, bool, bool) */
    BaseTopologyData( const char* helpMsg=nullptr, bool isDisplayed=true, bool isReadOnly=false)
        : Data<T>(helpMsg, isDisplayed, isReadOnly)
    {

    }

    /** \copydoc Data(const T&, const char*, bool, bool) */
    BaseTopologyData( const T& /*value*/, const char* helpMsg=nullptr, bool isDisplayed=true, bool isReadOnly=false)
        : Data<T>(helpMsg, isDisplayed, isReadOnly)
    {
    }


    /// Add some values. Values are added at the end of the vector.
    virtual void add(const sofa::helper::vector< Topology::PointID >& ,
        const sofa::helper::vector< Topology::Point >& ,
        const sofa::helper::vector< sofa::helper::vector< Topology::PointID > >&,
        const sofa::helper::vector< sofa::helper::vector< SReal > >& ,
        const sofa::helper::vector< PointAncestorElem >&) {}

    /// Temporary Hack: find a way to have a generic description of topological element:
    /// add Edge
    virtual void add(const sofa::helper::vector< Topology::EdgeID >&,
        const sofa::helper::vector< Topology::Edge >& ,
        const sofa::helper::vector< sofa::helper::vector< Topology::EdgeID > >&,
        const sofa::helper::vector< sofa::helper::vector< SReal > >& ,
        const sofa::helper::vector< EdgeAncestorElem >&) {}

    /// add Triangle
    virtual void add(const sofa::helper::vector< Topology::TriangleID >&,
        const sofa::helper::vector< Topology::Triangle >& ,
        const sofa::helper::vector< sofa::helper::vector< Topology::TriangleID > > &,
        const sofa::helper::vector< sofa::helper::vector< SReal > >& ,
        const sofa::helper::vector< TriangleAncestorElem >&) {}

    /// add Quad & Tetrahedron
    virtual void add(const sofa::helper::vector< Topology::TetrahedronID >&,
        const sofa::helper::vector< Topology::Tetrahedron >& ,
        const sofa::helper::vector< sofa::helper::vector< Topology::TetrahedronID > > &,
        const sofa::helper::vector< sofa::helper::vector< SReal > >& ,
        const sofa::helper::vector< TetrahedronAncestorElem >&) {}

    virtual void add(const sofa::helper::vector< Topology::QuadID >&,
        const sofa::helper::vector< Topology::Quad >&,
        const sofa::helper::vector< sofa::helper::vector< Topology::QuadID > >&,
        const sofa::helper::vector< sofa::helper::vector< SReal > >&,
        const sofa::helper::vector< QuadAncestorElem >&) {}

    /// add Hexahedron
    virtual void add(const sofa::helper::vector< Topology::HexahedronID >&,
        const sofa::helper::vector< Topology::Hexahedron >& ,
        const sofa::helper::vector< sofa::helper::vector< Topology::HexahedronID > > &,
        const sofa::helper::vector< sofa::helper::vector< SReal > >& ,
        const sofa::helper::vector< HexahedronAncestorElem >&) {}


    /// Remove the values corresponding to the points removed.
    virtual void remove( const sofa::helper::vector<unsigned int>& ) {}

    /// Swaps values at indices i1 and i2.
    virtual void swap( unsigned int , unsigned int ) {}

    /// Reorder the values.
    virtual void renumber( const sofa::helper::vector<unsigned int>& ) {}

    /// Move a list of points
    virtual void move( const sofa::helper::vector<unsigned int>& ,
            const sofa::helper::vector< sofa::helper::vector< unsigned int > >& ,
            const sofa::helper::vector< sofa::helper::vector< SReal > >& ) {}

    sofa::core::topology::BaseMeshTopology* getTopology()
    {
        return m_topology;
    }

    /// to handle PointSubsetData
    void setDataSetArraySize(const Index s) { lastElementIndex = s - 1; }

protected:
    sofa::core::topology::BaseMeshTopology* m_topology = nullptr;

    /// to handle properly the removal of items, the container must know the index of the last element
    Index lastElementIndex = 0;
};


} // namespace topology

} // namespace component

} // namespace sofa

#endif //SOFA_COMPONENT_TOPOLOGY_BASETOPOLOGYDATA_H
