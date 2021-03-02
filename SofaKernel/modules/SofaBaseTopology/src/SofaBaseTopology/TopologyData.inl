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
#include <SofaBaseTopology/TopologyData.h>
#include <SofaBaseTopology/TopologyDataEngine.inl>
#include <SofaBaseTopology/TopologyDataHandler.inl>

namespace sofa::component::topology
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   Generic Topology Data Implementation   /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TopologyElementType, typename VecT>
TopologyData <TopologyElementType, VecT>::TopologyData(const typename sofa::core::topology::BaseTopologyData< VecT >::InitData& data)
    : sofa::core::topology::BaseTopologyData< VecT >(data)
    , m_topologicalEngine(nullptr)
{

}



template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::createTopologicalEngine(sofa::core::topology::BaseMeshTopology* _topology)
{
    this->m_topology = _topology;

    this->m_topologicalEngine = new TopologyDataEngine< TopologyElementType, VecT>(this, _topology);
    this->m_topologicalEngine->setNamePrefix(std::string(sofa::core::topology::TopologyElementInfo<TopologyElementType>::name()) + std::string("Engine_"));
    if (this->getOwner() && dynamic_cast<sofa::core::objectmodel::BaseObject*>(this->getOwner()))
        dynamic_cast<sofa::core::objectmodel::BaseObject*>(this->getOwner())->addSlave(this->m_topologicalEngine);
    this->m_topologicalEngine->init();
    this->linkToElementDataArray((TopologyElementType*)nullptr);
    msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " initialized with dynamic " << _topology->getClassName() << "Topology.";
}


template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::createTopologicalEngine(sofa::core::topology::BaseMeshTopology* _topology, sofa::core::topology::TopologyEngine* topoEngine)
{
    this->m_topology = _topology;
    this->m_topologicalEngine = topoEngine;

    this->m_topologicalEngine->registerTopology(_topology);

    this->m_topologicalEngine->setNamePrefix(std::string(sofa::core::topology::TopologyElementInfo<TopologyElementType>::name()) + std::string("Engine_"));
    if (this->getOwner() && dynamic_cast<sofa::core::objectmodel::BaseObject*>(this->getOwner())) 
        dynamic_cast<sofa::core::objectmodel::BaseObject*>(this->getOwner())->addSlave(this->m_topologicalEngine);
    this->m_topologicalEngine->init();
    this->linkToElementDataArray((TopologyElementType*)nullptr);
    msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " initialized with dynamic " << _topology->getClassName() << "Topology.";   
}


template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::registerTopologicalData()
{
    if (this->m_topologicalEngine)
        this->m_topologicalEngine->registerTopology(this->m_topology);
    else if (!this->m_topology)
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " has no engine. Topological changes will be disabled. Use createTopologicalEngine method before registerTopologicalData to allow topological changes." ;
}

template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::addInputData(sofa::core::objectmodel::BaseData *_data)
{
    if (this->m_topologicalEngine)
        this->m_topologicalEngine->addInput(_data);
    else if (!this->m_topology)
        msg_info(this->getOwner()) <<"Warning: TopologyData: " << this->getName() << " has no engine. Use createTopologicalEngine function before addInputData." ;
}




/// Method used to link Data to point Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToPointDataArray()
{
    if(m_topologicalEngine)
        m_topologicalEngine->linkToPointDataArray();
}

/// Method used to link Data to edge Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToEdgeDataArray()
{
    if(m_topologicalEngine)
        m_topologicalEngine->linkToEdgeDataArray();
}

/// Method used to link Data to triangle Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToTriangleDataArray()
{
    if(m_topologicalEngine)
        m_topologicalEngine->linkToTriangleDataArray();
}

/// Method used to link Data to quad Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToQuadDataArray()
{
    if(m_topologicalEngine)
        m_topologicalEngine->linkToQuadDataArray();
}

/// Method used to link Data to tetrahedron Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToTetrahedronDataArray()
{
    if(m_topologicalEngine)
        m_topologicalEngine->linkToTetrahedronDataArray();
}

/// Method used to link Data to hexahedron Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToHexahedronDataArray()
{
    if(m_topologicalEngine)
        m_topologicalEngine->linkToHexahedronDataArray();
}


} //namespace sofa::component::topology
