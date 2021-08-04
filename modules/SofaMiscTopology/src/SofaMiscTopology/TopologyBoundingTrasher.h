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

#include <SofaMiscTopology/config.h>
#include <sofa/core/DataEngine.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/topology/BaseMeshTopology.h>

#include <sofa/simulation/AnimateBeginEvent.h>
#include <sofa/simulation/AnimateEndEvent.h>


namespace sofa::component::topology
{
    /// forward declaration to avoid adding includes in .h
    class EdgeSetTopologyModifier;
    class TriangleSetTopologyModifier;
    class TetrahedronSetTopologyModifier;
    class QuadSetTopologyModifier;
    class HexahedronSetTopologyModifier;
}

namespace sofa::component::misc
{

    using core::topology::BaseMeshTopology;
    using core::behavior::MechanicalState;

/** Read file containing topological modification. Or apply input modifications
 * A timestep has to be established for each modification.
 *
*/
template <class DataTypes>
class SOFA_SOFAMISCTOPOLOGY_API TopologyBoundingTrasher: public virtual core::objectmodel::BaseObject
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(TopologyBoundingTrasher, DataTypes), core::objectmodel::BaseObject);
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Real Real;
    typedef type::Vec<3, Real> Vec3;
    typedef type::Vec<6, Real> Vec6;

    using Index = sofa::Index;
    using Vector3 = type::Vector3;

protected:
    TopologyBoundingTrasher();

    ~TopologyBoundingTrasher() override;

    void filterElementsToRemove();
    void removeElements();

    constexpr bool isPointOutside(const Coord& value, const Vec6& bb);

public:
    void init() override;
    void reinit() override;

    void handleEvent(sofa::core::objectmodel::Event* event) override;

    void draw(const core::visual::VisualParams* vparams) override;

public:
    Data<VecCoord> d_positions;
    Data<Vec6>  d_borders;
    Data<bool>  m_drawBox; ///< draw bounding box

    /// Link to be set to the topology container in the component graph.
    SingleLink<TopologyBoundingTrasher, sofa::core::topology::BaseMeshTopology, BaseLink::FLAG_STOREPATH | BaseLink::FLAG_STRONGLINK> l_topology;


protected:
    core::topology::BaseMeshTopology* m_topology;
    sofa::core::topology::TopologyElementType m_topologyType;

    sofa::component::topology::EdgeSetTopologyModifier* edgeModifier;
    sofa::component::topology::TriangleSetTopologyModifier* triangleModifier;
    sofa::component::topology::QuadSetTopologyModifier* quadModifier;
    sofa::component::topology::TetrahedronSetTopologyModifier* tetraModifier;
    sofa::component::topology::HexahedronSetTopologyModifier* hexaModifier;

    type::vector<Index> m_indicesToRemove;
};

#if  !defined(SOFA_COMPONENT_MISC_TOPOLOGYBOUNDINGTRASHER_CPP)
extern template class SOFA_SOFAMISCTOPOLOGY_API TopologyBoundingTrasher<sofa::defaulttype::Vec3Types>;
#endif //  !defined(SOFA_COMPONENT_MISC_TOPOLOGYBOUNDINGTRASHER_CPP)


} // namespace sofa::component::misc
