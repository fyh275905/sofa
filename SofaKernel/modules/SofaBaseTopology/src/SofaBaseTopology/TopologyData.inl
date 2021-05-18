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
#include <SofaBaseTopology/TopologyDataHandler.inl>

namespace sofa::component::topology
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   Generic Topology Data Implementation   /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TopologyElementType, typename VecT>
TopologyData <TopologyElementType, VecT>::TopologyData(const typename sofa::core::topology::BaseTopologyData< VecT >::InitData& data)
    : sofa::core::topology::BaseTopologyData< VecT >(data)
    , m_topologyHandler(nullptr)
    , m_isTopologyDynamic(false)
{
    this->lastElementIndex = 0;
}


template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::createTopologyHandler(sofa::core::topology::BaseMeshTopology* _topology)
{
    if (_topology == nullptr)
    {
        msg_error(this->getOwner()) << "Topology used to register this TopologyData: " << this->getName() << " is invalid. TopologyData won't be registered.";
        return;
    }
    this->m_topology = _topology;

    // Create Topology engine
    this->m_topologyHandler = new TopologyDataHandler< TopologyElementType, VecT>(this, _topology);
    this->m_topologyHandler->setNamePrefix(std::string(sofa::core::topology::TopologyElementInfo<TopologyElementType>::name()) + std::string("Engine_"));
    this->m_topologyHandler->init();

    // Register the engine
    m_isTopologyDynamic = this->m_topologyHandler->registerTopology();
    if (m_isTopologyDynamic)
    {
        this->linkToElementDataArray((TopologyElementType*)nullptr);
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " initialized with dynamic " << _topology->getClassName() << "Topology.";
    }

    //if (this->getOwner() && dynamic_cast<sofa::core::objectmodel::BaseObject*>(this->getOwner()))
    //    dynamic_cast<sofa::core::objectmodel::BaseObject*>(this->getOwner())->addSlave(this->m_topologyHandler);
}


template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::createTopologyHandler(sofa::core::topology::BaseMeshTopology* _topology, sofa::component::topology::TopologyDataHandler< TopologyElementType, VecT>* topoEngine)
{
    if (_topology == nullptr)
    {
        msg_error(this->getOwner()) << "Topology used to register this TopologyData: " << this->getName() << " is invalid. TopologyData won't be registered.";
        return;
    }

    this->m_topology = _topology;

    // Set Topology engine
    this->m_topologyHandler = topoEngine;
    this->m_topologyHandler->setNamePrefix(std::string(sofa::core::topology::TopologyElementInfo<TopologyElementType>::name()) + std::string("Engine_"));
    this->m_topologyHandler->init();

    // Register the engine
    m_isTopologyDynamic = this->m_topologyHandler->registerTopology(_topology);
    if (m_isTopologyDynamic)
    {
        this->linkToElementDataArray((TopologyElementType*)nullptr);
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " initialized with dynamic " << this->m_topology->getClassName() << "Topology.";
    }
    
    //if (this->getOwner() && dynamic_cast<sofa::core::objectmodel::BaseObject*>(this->getOwner())) 
    //    dynamic_cast<sofa::core::objectmodel::BaseObject*>(this->getOwner())->addSlave(this->m_topologyHandler);   
}


template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::registerTopologicalData()
{
    if (this->m_topologyHandler)
        this->m_topologyHandler->registerTopology(this->m_topology);
    else if (!this->m_topology)
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " has no engine. Topological changes will be disabled. Use createTopologyHandler method before registerTopologicalData to allow topological changes." ;
}

template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::addInputData(sofa::core::objectmodel::BaseData *_data)
{
    if (this->m_topologyHandler)
        this->m_topologyHandler->addInput(_data);
    else if (!this->m_topology)
        msg_info(this->getOwner()) <<"Warning: TopologyData: " << this->getName() << " has no engine. Use createTopologyHandler function before addInputData." ;
}




/// Method used to link Data to point Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToPointDataArray()
{
    if (this->m_topologyHandler && m_isTopologyDynamic)
    {
        this->m_topologyHandler->linkToPointDataArray();
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " linkToPointDataArray ";
    }
    else
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " won't be linkToPointDataArray as toplogy is not dynamic";
}

/// Method used to link Data to edge Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToEdgeDataArray()
{
    if (this->m_topologyHandler && m_isTopologyDynamic)
    {
        this->m_topologyHandler->linkToEdgeDataArray();
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " linkToEdgeDataArray ";
    }
    else
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " won't be linkToEdgeDataArray as toplogy is not dynamic";
}

/// Method used to link Data to triangle Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToTriangleDataArray()
{
    if (this->m_topologyHandler && m_isTopologyDynamic)
    {
        this->m_topologyHandler->linkToTriangleDataArray();
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " linkToTriangleDataArray ";
    }
    else
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " won't be linkToTriangleDataArray as toplogy is not dynamic";
}

/// Method used to link Data to quad Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToQuadDataArray()
{
    if (this->m_topologyHandler && m_isTopologyDynamic)
    {
        this->m_topologyHandler->linkToQuadDataArray();
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " linkToQuadDataArray ";
    }
    else
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " won't be linkToQuadDataArray as toplogy is not dynamic";
}

/// Method used to link Data to tetrahedron Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToTetrahedronDataArray()
{
    if (this->m_topologyHandler && m_isTopologyDynamic)
    {
        this->m_topologyHandler->linkToTetrahedronDataArray();
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " linkToTetrahedronDataArray ";
    }
    else
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " won't be linkToTetrahedronDataArray as toplogy is not dynamic";
}

/// Method used to link Data to hexahedron Data array, using the engine's method
template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::linkToHexahedronDataArray()
{
    if (this->m_topologyHandler && m_isTopologyDynamic)
    {
        this->m_topologyHandler->linkToHexahedronDataArray();
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " linkToHexahedronDataArray ";
    }
    else
        msg_info(this->getOwner()) << "TopologyData: " << this->getName() << " won't be linkToHexahedronDataArray as toplogy is not dynamic";
}


///////////////////// Protected functions on TopologyData changes /////////////////////////////

template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::swap(Index i1, Index i2)
{
    container_type& data = *(this->beginEdit());
    value_type tmp = data[i1];
    data[i1] = data[i2];
    data[i2] = tmp;
    this->endEdit();
}


template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::remove(const sofa::helper::vector<Index>& index)
{

    container_type& data = *(this->beginEdit());
    if (data.size() > 0)
    {
        Index last = Index(data.size() - 1);

        for (std::size_t i = 0; i < index.size(); ++i)
        {
            if (this->m_topologyHandler) {
                this->m_topologyHandler->applyDestroyFunction(index[i], data[index[i]]);
            }
            this->swap(index[i], last);
            --last;
        }

        data.resize(data.size() - index.size());
    }
    this->endEdit();
}


template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::add(const sofa::helper::vector<Index>& index,
    const sofa::helper::vector< TopologyElementType >& elems,
    const sofa::helper::vector<sofa::helper::vector<Index> >& ancestors,
    const sofa::helper::vector<sofa::helper::vector<double> >& coefs,
    const sofa::helper::vector< AncestorElem >& ancestorElems)
{
    std::size_t nbElements = index.size();
    if (nbElements == 0) return;
    // Using default values
    container_type& data = *(this->beginEdit());
    std::size_t i0 = data.size();
    if (i0 != index[0])
    {
        msg_error(this->getOwner()) << "TopologyDataHandler SIZE MISMATCH in Data "
            << this->getName() << ": " << nbElements << " "
            << core::topology::TopologyElementInfo<TopologyElementType>::name()
            << " ADDED starting from index " << index[0]
            << " while vector size is " << i0;
        i0 = index[0];
    }
    data.resize(i0 + nbElements);

    const sofa::helper::vector< Index > empty_vecint;
    const sofa::helper::vector< double > empty_vecdouble;

    if (this->m_topologyHandler)
    {
        for (Index i = 0; i < nbElements; ++i)
        {
            value_type& t = data[i0 + i];
        
            this->m_topologyHandler->applyCreateFunction(Index(i0 + i), t, elems[i],
                    (ancestors.empty() || coefs.empty()) ? empty_vecint : ancestors[i],
                    (ancestors.empty() || coefs.empty()) ? empty_vecdouble : coefs[i],
                    (ancestorElems.empty()) ? nullptr : &ancestorElems[i]);
        
        }
    }
    this->endEdit();
}


template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::move(const sofa::helper::vector<Index>& indexList,
    const sofa::helper::vector< sofa::helper::vector< Index > >& ancestors,
    const sofa::helper::vector< sofa::helper::vector< double > >& coefs)
{
    container_type& data = *(this->beginEdit());

    if (this->m_topologyHandler)
    {
        for (std::size_t i = 0; i < indexList.size(); i++)
        {
            this->m_topologyHandler->applyDestroyFunction(indexList[i], data[indexList[i]]);
            this->m_topologyHandler->applyCreateFunction(indexList[i], data[indexList[i]], ancestors[i], coefs[i]);
        }
    }

    this->endEdit();
}



template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::renumber(const sofa::helper::vector<Index>& index)
{
    container_type& data = *(this->beginEdit());

    container_type copy = this->getValue(); // not very efficient memory-wise, but I can see no better solution...
    for (std::size_t i = 0; i < index.size(); ++i)
        data[i] = copy[index[i]];

    this->endEdit();
}


template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::addOnMovedPosition(const sofa::helper::vector<Index>& indexList,
    const sofa::helper::vector<TopologyElementType>& elems)
{
    container_type& data = *(this->beginEdit());

    // Recompute data
    sofa::helper::vector< Index > ancestors;
    sofa::helper::vector< double >  coefs;
    coefs.push_back(1.0);
    ancestors.resize(1);

    if (this->m_topologyHandler)
    {
        for (std::size_t i = 0; i < indexList.size(); i++)
        {
            ancestors[0] = indexList[i];
            this->m_topologyHandler->applyCreateFunction(indexList[i], data[indexList[i]], elems[i], ancestors, coefs);
        }
    }
    this->endEdit();
}


template <typename TopologyElementType, typename VecT>
void TopologyData <TopologyElementType, VecT>::removeOnMovedPosition(const sofa::helper::vector<Index>& indices)
{
    container_type& data = *(this->beginEdit());

    if (this->m_topologyHandler)
    {
        for (std::size_t i = 0; i < indices.size(); i++) {
            this->m_topologyHandler->applyDestroyFunction(indices[i], data[indices[i]]);
        }
    }

    this->endEdit();

    // TODO check why this call.
    //this->remove( indices );
}


} //namespace sofa::component::topology
