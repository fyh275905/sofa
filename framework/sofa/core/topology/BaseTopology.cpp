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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/core/topology/BaseTopology.h>

namespace sofa
{

namespace core
{

namespace topology
{
// GeometryAlgorithms implementation

void GeometryAlgorithms::init()
{
}

// TopologyAlgorithms implementation

void TopologyAlgorithms::init()
{
    this->getContext()->get(m_topologyContainer);
}

void TopologyAlgorithms::addTopologyChange(const TopologyChange *topologyChange)
{
    m_topologyContainer->addTopologyChange(topologyChange);
}

// TopologyModifier implementation

void TopologyModifier::init()
{
    this->getContext()->get(m_topologyContainer);
}

void TopologyModifier::addTopologyChange(const TopologyChange *topologyChange)
{
    m_topologyContainer->addTopologyChange(topologyChange);
}

void TopologyModifier::addStateChange(const TopologyChange *topologyChange)
{
    m_topologyContainer->addStateChange(topologyChange);
}

void TopologyModifier::propagateStateChanges() {}
void TopologyModifier::propagateTopologicalChanges() {}
void TopologyModifier::notifyEndingEvent() {}
void TopologyModifier::removeItems(sofa::helper::vector< unsigned int >& /*items*/) {}

// TopologyContainer implementation

void TopologyContainer::init()
{
    core::topology::BaseMeshTopology::init();
    core::topology::BaseTopologyObject::init();
}


void TopologyContainer::addTopologyChange(const TopologyChange *topologyChange)
{
    sofa::helper::list <const TopologyChange *>& my_changeList = *(m_changeList.beginEdit());
    my_changeList.push_back(topologyChange);
    m_changeList.endEdit();
}

void TopologyContainer::addStateChange(const TopologyChange *topologyChange)
{
    sofa::helper::list <const TopologyChange *>& my_stateChangeList = *(m_stateChangeList.beginEdit());
    my_stateChangeList.push_back(topologyChange);
    m_stateChangeList.endEdit();
}

void TopologyContainer::addTopologyEngine(const TopologyEngine *_topologyEngine)
{
    m_topologyEngineList.push_back(_topologyEngine);
}


sofa::helper::list<const TopologyChange *>::const_iterator TopologyContainer::lastChange() const
{
    return (m_changeList.getValue()).end();
}

sofa::helper::list<const TopologyChange *>::const_iterator TopologyContainer::firstChange() const
{
    return (m_changeList.getValue()).begin();
}

sofa::helper::list<const TopologyChange *>::const_iterator TopologyContainer::lastStateChange() const
{
    return (m_stateChangeList.getValue()).end();
}

sofa::helper::list<const TopologyChange *>::const_iterator TopologyContainer::firstStateChange() const
{
    return (m_stateChangeList.getValue()).begin();
}

sofa::helper::list<const TopologyEngine *>::const_iterator TopologyContainer::lastTopologyEngine() const
{
    return m_topologyEngineList.end();
}

sofa::helper::list<const TopologyEngine *>::const_iterator TopologyContainer::firstTopologyEngine() const
{
    return m_topologyEngineList.begin();
}

void TopologyContainer::resetTopologyChangeList()
{
    sofa::helper::list <const TopologyChange *>& my_changeList = *(m_changeList.beginEdit());
    for (std::list<const TopologyChange *>::iterator it=my_changeList.begin();
            it!=my_changeList.end(); ++it)
    {
        delete (*it);
    }

    my_changeList.clear();
    m_changeList.endEdit();
}

void TopologyContainer::resetStateChangeList()
{
    sofa::helper::list <const TopologyChange *>& my_stateChangeList = *(m_stateChangeList.beginEdit());
    for (std::list<const TopologyChange *>::iterator it=my_stateChangeList.begin();
            it!=my_stateChangeList.end(); ++it)
    {
        delete (*it);
    }

    my_stateChangeList.clear();
    m_stateChangeList.endEdit();
}

void TopologyContainer::resetTopologyEngineList()
{
    for (std::list<const TopologyEngine *>::iterator it=m_topologyEngineList.begin();
            it!=m_topologyEngineList.end(); ++it)
    {
        delete (*it);
    }

    m_topologyEngineList.clear();
}


// TopologyEngine implementation
TopologyEngine::TopologyEngine(): m_topologicalData(NULL)
{}

TopologyEngine::~TopologyEngine()
{
    if (this->m_topologicalData != NULL)
        this->removeTopoligicalData();
}

void TopologyEngine::init()
{
    DataEngine::init();

    this->addInput(&m_changeList);
}

void TopologyEngine::registerTopologicalData(sofa::core::objectmodel::Data< sofa::helper::vector <void*> >* topologicalData)
{
    m_topologicalData = topologicalData;
}

void TopologyEngine::removeTopoligicalData()
{
    if (this->m_topologicalData)
        delete this->m_topologicalData;
}


} // namespace topology

} // namespace core

} // namespace sofa

