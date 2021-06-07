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

#include <sofa/helper/vector.h>

#include <sofa/core/topology/BaseTopologyData.h>
#include <SofaBaseTopology/TopologyDataHandler.h>


namespace sofa::component::topology
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   Generic Topology Data Implementation   /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** \brief A class for storing topology related data. Automatically manages topology changes.
*
* This class is a wrapper of class helper::vector that is made to take care transparently of all topology changes that might
* happen (non exhaustive list: element added, removed, fused, renumbered).
*/
template< class TopologyElementType, class VecT>
class TopologyData : public sofa::core::topology::BaseTopologyData<VecT>
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
    typedef core::topology::TopologyElementInfo<TopologyElementType> ElementInfo;
    typedef core::topology::TopologyChangeElementInfo<TopologyElementType> ChangeElementInfo;
    typedef typename ChangeElementInfo::AncestorElem    AncestorElem;


    /// Constructor
    TopologyData(const typename sofa::core::topology::BaseTopologyData< VecT >::InitData& data);


    /** Public functions to handle topological engine creation */
    /// To create topological engine link to this Data. Pointer to current topology is needed.
    void createTopologyHandler(sofa::core::topology::BaseMeshTopology* _topology);

    /** Public functions to handle topological engine creation */
    /// To create topological engine link to this Data. Pointer to current topology is needed.
    void createTopologyHandler(sofa::core::topology::BaseMeshTopology* _topology, sofa::component::topology::TopologyDataHandler< TopologyElementType, VecT>* topoEngine);

    /// Link Data to topology arrays
    void linkToPointDataArray();
    void linkToEdgeDataArray();
    void linkToTriangleDataArray();
    void linkToQuadDataArray();
    void linkToTetrahedronDataArray();
    void linkToHexahedronDataArray();

    /// Swaps values at indices i1 and i2.
    void swap(Index i1, Index i2) override;

    /// Remove the values corresponding to the elements removed.
    void remove(const sofa::helper::vector<Index>& index) override;

    /// Add some values. Values are added at the end of the vector.
    /// This (new) version gives more information for element indices and ancestry
    virtual void add(const sofa::helper::vector<Index>& index,
        const sofa::helper::vector< TopologyElementType >& elems,
        const sofa::helper::vector< sofa::helper::vector< Index > >& ancestors,
        const sofa::helper::vector< sofa::helper::vector< double > >& coefs,
        const sofa::helper::vector< AncestorElem >& ancestorElems) override;

    /// Reorder the values.
    void renumber(const sofa::helper::vector<Index>& index) override;

    /// Move a list of points
    void move(const sofa::helper::vector<Index>& indexList,
        const sofa::helper::vector< sofa::helper::vector< Index > >& ancestors,
        const sofa::helper::vector< sofa::helper::vector< double > >& coefs) override;

    /// Add Element after a displacement of vertices, ie. add element based on previous position topology revision.
    virtual void addOnMovedPosition(const sofa::helper::vector<Index>& indexList,
        const sofa::helper::vector< TopologyElementType >& elems);

    /// Remove Element after a displacement of vertices, ie. add element based on previous position topology revision.
    virtual void removeOnMovedPosition(const sofa::helper::vector<Index>& indices);

    /** Method to add a callback when a element is deleted from this container. It will be called by @sa remove method for example.
    * This is only to specify a specific behevior/computation when removing an element from this container. Otherwise normal deletion is applyed.
    * Parameters are @param Index of the element which is detroyed and @value_type value hold by this container.
    */
    void applyDestroyFunction(std::function<void(Index, value_type&)> func) { m_DestroyFunction = func; }
    
    /** Method to add a callback when a element is created in this container. It will be called by @sa add method for example.
    * This is only to specify a specific behevior/computation when adding an element in this container. Otherwise default constructor of the element is used.
    * @param Index of the element which is created.
    * @param value_type value hold by this container.
    * @param TopologyElementType type of topologyElement created.
    * @param List of ancestor indices.
    * @param List of coefficient respect to the ancestor indices.
    */
    void applyCreateFunction(std::function<void(Index, value_type&, const TopologyElementType&, const sofa::helper::vector< Index >&, const sofa::helper::vector< double >&)> func) { m_CreateFunction = func; }

    std::function<void(Index, value_type&)> m_DestroyFunction;
    std::function<void(Index, value_type&, const TopologyElementType&, const sofa::helper::vector< Index >&, const sofa::helper::vector< double >&)> m_CreateFunction;

    ////////////////////////////////////// DEPRECATED ///////////////////////////////////////////
    SOFA_ATTRIBUTE_DISABLED("v21.06 (PR#2082)", "v21.06 (PR#2082)", "This method has been removed as it is not part of the new topology change design.")
    void addInputData(sofa::core::objectmodel::BaseData* _data) = delete;

    SOFA_ATTRIBUTE_DISABLED("v21.06 (PR#2082)", "v21.06 (PR#2082)", "This method was deleted because it presented risks. Use Write/Read Accessor instead.")
    const value_type& operator[](int i) const = delete;

    SOFA_ATTRIBUTE_DISABLED("v21.06 (PR#2082)", "v21.06 (PR#2082)", "This method was deleted because it presented risks. Use Write/Read Accessor instead.")
    value_type& operator[](int i) = delete;

    SOFA_ATTRIBUTE_DISABLED("v21.06 (PR#2086)", "v21.06 (PR#2086)", "This method has been removed as it's mechanism is now automatically done in TopologyHandler.")
    void registerTopologicalData() = delete;
    
protected:
    sofa::component::topology::TopologyDataHandler< TopologyElementType, VecT>* m_topologyHandler;

    bool m_isTopologyDynamic;

    void linkToElementDataArray(sofa::core::topology::BaseMeshTopology::Point*      ) { linkToPointDataArray();       }
    void linkToElementDataArray(sofa::core::topology::BaseMeshTopology::Edge*       ) { linkToEdgeDataArray();        }
    void linkToElementDataArray(sofa::core::topology::BaseMeshTopology::Triangle*   ) { linkToTriangleDataArray();    }
    void linkToElementDataArray(sofa::core::topology::BaseMeshTopology::Quad*       ) { linkToQuadDataArray();        }
    void linkToElementDataArray(sofa::core::topology::BaseMeshTopology::Tetrahedron*) { linkToTetrahedronDataArray(); }
    void linkToElementDataArray(sofa::core::topology::BaseMeshTopology::Hexahedron* ) { linkToHexahedronDataArray();  }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   Element Topology Data Implementation   ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< class VecT > using PointData       = TopologyData<core::topology::BaseMeshTopology::Point, VecT>;
template< class VecT > using EdgeData        = TopologyData<core::topology::BaseMeshTopology::Edge, VecT>;
template< class VecT > using TriangleData    = TopologyData<core::topology::BaseMeshTopology::Triangle, VecT>;
template< class VecT > using QuadData        = TopologyData<core::topology::BaseMeshTopology::Quad, VecT>;
template< class VecT > using TetrahedronData = TopologyData<core::topology::BaseMeshTopology::Tetrahedron, VecT>;
template< class VecT > using HexahedronData  = TopologyData<core::topology::BaseMeshTopology::Hexahedron, VecT>;


} //namespace sofa::component::topology
