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
#include <SofaBaseTopology/TopologySubsetData.h>
#include <SofaBaseTopology/TopologyData.inl>
#include <SofaBaseTopology/TopologyDataHandler.inl>

namespace sofa::component::topology
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   Generic Topology Data Implementation   /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TopologyElementType, typename VecT>
TopologySubsetData <TopologyElementType, VecT>::TopologySubsetData(const typename sofa::core::topology::BaseTopologyData< VecT >::InitData& data)
    : sofa::component::topology::TopologyData< TopologyElementType, VecT >(data)
    , m_usingMap(false)
{

}

///////////////////// Private functions on TopologySubsetData changes /////////////////////////////
template <typename TopologyElementType, typename VecT>
void TopologySubsetData <TopologyElementType, VecT>::swap(Index i1, Index i2)
{
    container_type& data = *(this->beginEdit());
    
    if (i1 >= data.size() || i2 >= data.size())
    {
        msg_warning("TopologySubsetData") << "swap indices out of bouds: i1: " << i1 << " | i2: " << i2 << " out of data size: " << data.size();
        this->endEdit();
        return;
    }

    if (m_usingMap)
    {
        if (data.size() != m_map2Elements.size())
        {
            msg_warning("TopologySubsetData") << "swap indices, map not the same size as data stored. Map size:" << m_map2Elements.size() << " out of data size: " << data.size();
            this->endEdit();
            return;
        }

        //apply same change to map:
        Index tmp2 = m_map2Elements[i1];
        m_map2Elements[i1] = m_map2Elements[i2];
        m_map2Elements[i2] = tmp2;
    }
    
    value_type tmp = data[i1];
    data[i1] = data[i2];
    data[i2] = tmp;

    this->endEdit();
}


template <typename TopologyElementType, typename VecT>
void TopologySubsetData <TopologyElementType, VecT>::setMap2Elements(const sofa::helper::vector<Index> _map2Elements)
{
    m_map2Elements = _map2Elements;
    m_usingMap = true;
}

template <typename TopologyElementType, typename VecT>
Index TopologySubsetData <TopologyElementType, VecT>::indexOfElement(Index index)
{
    if (!m_usingMap)
        return sofa::InvalidID;

    for (unsigned int i = 0; i < m_map2Elements.size(); ++i)
        if (index == m_map2Elements[i])
            return i;

    return sofa::InvalidID;
}

template <typename TopologyElementType, typename VecT>
void TopologySubsetData <TopologyElementType, VecT>::add(sofa::Size nbElements,
    const sofa::helper::vector<sofa::helper::vector<Index> >& ancestors,
    const sofa::helper::vector<sofa::helper::vector<double> >& coefs)
{
    // Using default values
    container_type& data = *(this->beginEdit());

    Size size = data.size();
    data.resize(size + nbElements);
    
    // Call for specific callback if handler has been set
    if (this->m_topologyHandler)
    {
        value_type t;
        for (std::size_t i = 0; i < nbElements; ++i)
        {
            if (ancestors.empty() || coefs.empty())
            {
                const sofa::helper::vector< Index > empty_vecint;
                const sofa::helper::vector< double > empty_vecdouble;

                this->m_topologyHandler->applyCreateFunction(Index(size + i), t, empty_vecint, empty_vecdouble);
            }
            else {
                this->m_topologyHandler->applyCreateFunction(Index(size + i), t, ancestors[i], coefs[i]);
            }
        }
    }

    // update map if needed
    if (m_usingMap)
    {
        for (unsigned int i = 0; i < nbElements; ++i)
        {
            this->lastElementIndex++;
            m_map2Elements.push_back(this->lastElementIndex);
        }
    }
    else
    {
        this->lastElementIndex += nbElements;
    }
    
    this->endEdit();
}


template <typename TopologyElementType, typename VecT>
void TopologySubsetData <TopologyElementType, VecT>::add(sofa::Size nbElements,
    const sofa::helper::vector< TopologyElementType >&,
    const sofa::helper::vector<sofa::helper::vector<Index> >& ancestors,
    const sofa::helper::vector<sofa::helper::vector<double> >& coefs)
{
    this->add(nbElements, ancestors, coefs);
}


template <typename TopologyElementType, typename VecT>
void TopologySubsetData <TopologyElementType, VecT>::add(const sofa::helper::vector<Index>& index,
    const sofa::helper::vector< TopologyElementType >& elems,
    const sofa::helper::vector< sofa::helper::vector< Index > >& ancestors,
    const sofa::helper::vector< sofa::helper::vector< double > >& coefs,
    const sofa::helper::vector< AncestorElem >& ancestorElems)
{
    SOFA_UNUSED(elems);
    SOFA_UNUSED(ancestorElems);
    this->add(index.size(), ancestors, coefs);
}


template <typename TopologyElementType, typename VecT>
void TopologySubsetData <TopologyElementType, VecT>::move(const sofa::helper::vector<Index>&,
    const sofa::helper::vector< sofa::helper::vector< Index > >&,
    const sofa::helper::vector< sofa::helper::vector< double > >&)
{

}


template <typename TopologyElementType, typename VecT>
void TopologySubsetData <TopologyElementType, VecT>::remove(const sofa::helper::vector<Index>& index)
{
    container_type& data = *(this->beginEdit());
    
    unsigned int cptDone = 0;
    Index last = data.size() - 1;

    // check for each element remove if it concern this subsetData
    for (Index idRemove : index)
    {
        Index idElem = sofa::InvalidID;
        if (m_usingMap)
        {
            idElem = this->indexOfElement(idRemove);
            if (idElem == sofa::InvalidID)
                continue;

            if (this->m_topologyHandler)
            {
                this->m_topologyHandler->applyDestroyFunction(idElem, data[idElem]);
            }
        }
        else
        {
            bool found = false;
            for (idElem = 0; idElem < data.size(); idElem++)
            {
                if (data[idElem] == idRemove) // TODO: change that, this won't work if template is not an Index
                {
                    found = true;
                    break;
                }
            }

            if (!found) // element to remove not in this subset
                continue;
        }

        this->swap(idElem, last);
        cptDone++;
        if (last == 0)
            break;
        else
            --last;     
    }

    if (m_usingMap) {
        m_map2Elements.resize(data.size() - cptDone);
    }
    data.resize(data.size() - cptDone);
    this->lastElementIndex = last;
    this->endEdit();
}


template <typename TopologyElementType, typename VecT>
void TopologySubsetData <TopologyElementType, VecT>::renumber(const sofa::helper::vector<Index>& index)
{
    container_type& data = *(this->beginEdit());
    container_type copy = this->getValue(); // not very efficient memory-wise, but I can see no better solution...

    for (std::size_t i = 0; i < data.size(); ++i)
    {
        data[i] = copy[index[i]];
    }
    this->endEdit();
}


template <typename TopologyElementType, typename VecT>
void TopologySubsetData <TopologyElementType, VecT>::addOnMovedPosition(const sofa::helper::vector<Index>&,
    const sofa::helper::vector<TopologyElementType>&)
{
    dmsg_error("TopologySubsetData") << "addOnMovedPosition event on topology subsetData is not yet handled.";
}


template <typename TopologyElementType, typename VecT>
void TopologySubsetData <TopologyElementType, VecT>::removeOnMovedPosition(const sofa::helper::vector<Index>&)
{
    dmsg_error("TopologySubsetData") << "removeOnMovedPosition event on topology subsetData is not yet handled";
}


} //namespace sofa::component::topology
