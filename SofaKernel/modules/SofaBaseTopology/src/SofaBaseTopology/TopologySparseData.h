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
#pragma once
#include <SofaBaseTopology/config.h>

#include <SofaBaseTopology/TopologyData.h>
#include <SofaBaseTopology/TopologyDataEngine.h>
#include <SofaBaseTopology/TopologySparseDataHandler.h>

namespace sofa::component::topology
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   Generic Topology Data Implementation   /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< class TopologyElementType, class VecT>
class TopologySparseData : public sofa::component::topology::TopologyData<TopologyElementType, VecT>
{

public:
    typedef VecT container_type;
    typedef typename container_type::value_type value_type;

    /// Size
    typedef typename container_type::Size Size;
    /// reference to a value (read-write)
    typedef typename container_type::reference reference;
    /// const reference to a value (read only)
    typedef typename container_type::const_reference const_reference;
    /// const iterator
    typedef typename container_type::const_iterator const_iterator;

    using Index = sofa::Index;


    /// Constructor
    TopologySparseData( const typename sofa::core::topology::BaseTopologyData< VecT >::InitData& data)
        : sofa::component::topology::TopologyData< TopologyElementType, VecT >(data)
        , m_topologyHandler(nullptr)
        , m_isConcerned(false)
    {}

    virtual ~TopologySparseData();

    /** Public functions to handle topological engine creation */
    /// To create topological engine link to this Data. Pointer to current topology is needed.
    virtual void createTopologicalEngine(sofa::core::topology::BaseMeshTopology* _topology, sofa::core::topology::TopologyHandler* _topologyHandler);

    /** Public functions to handle topological engine creation */
    /// To create topological engine link to this Data. Pointer to current topology is needed.
    virtual void createTopologicalEngine(sofa::core::topology::BaseMeshTopology* _topology);



    void setMap2Elements(const sofa::helper::vector<Index> _map2Elements)
    {
        m_map2Elements = _map2Elements;
    }

    sofa::helper::vector<Index>& getMap2Elements() {return m_map2Elements;}

    bool getSparseDataStatus() {return m_isConcerned;}

    void activateSparseData() {m_isConcerned = true;}
    void desactivateSparseData() {m_isConcerned = false;}

    size_t size() {return m_map2Elements.size();}

    Index indexOfElement(Index index)
    {
        for (unsigned int i=0; i<m_map2Elements.size(); ++i)
            if (index == m_map2Elements[i])
                return i;

        return sofa::core::topology::Topology::InvalidID;
    }


protected:

    virtual void createTopologyHandler() {}

    // same size as SparseData but contain id of element link to each data[]
    sofa::helper::vector<Index> m_map2Elements;
    sofa::component::topology::TopologySparseDataHandler<TopologyElementType,VecT>* m_topologyHandler;

    bool m_isConcerned;

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   Element Topology Data Implementation   ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< class VecT > using PointSparseData = TopologySparseData<core::topology::BaseMeshTopology::Point, VecT>;
template< class VecT > using EdgeSparseData = TopologySparseData<core::topology::BaseMeshTopology::Edge, VecT>;
template< class VecT > using TriangleSparseData = TopologySparseData<core::topology::BaseMeshTopology::Triangle, VecT>;
template< class VecT > using QuadSparseData = TopologySparseData<core::topology::BaseMeshTopology::Quad, VecT>;
template< class VecT > using TetrahedronSparseData = TopologySparseData<core::topology::BaseMeshTopology::Tetrahedron, VecT>;
template< class VecT > using HexahedronSparseData = TopologySparseData<core::topology::BaseMeshTopology::Hexahedron, VecT>;


} //namespace sofa::component::topology
