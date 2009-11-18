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
#include <sofa/component/topology/PointSetTopologyModifier.h>
#include <sofa/simulation/common/StateChangeVisitor.h>
#include <sofa/simulation/common/Simulation.h>
#include <sofa/simulation/common/TopologyChangeVisitor.h>
#include <sofa/component/topology/PointSetTopologyChange.h>
#include <sofa/component/topology/PointSetTopologyContainer.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace topology
{
SOFA_DECL_CLASS(PointSetTopologyModifier)
int PointSetTopologyModifierClass = core::RegisterObject("Point set topology modifier")
        .add< PointSetTopologyModifier >();

using namespace std;
using namespace sofa::defaulttype;
using namespace sofa::core::componentmodel::behavior;


void PointSetTopologyModifier::init()
{
    core::componentmodel::topology::TopologyModifier::init();
    this->getContext()->get(m_container);
}


void PointSetTopologyModifier::swapPoints(const int i1, const int i2)
{
    PointsIndicesSwap *e2 = new PointsIndicesSwap( i1, i2 );
    addStateChange(e2);
    propagateStateChanges();

    PointsIndicesSwap *e = new PointsIndicesSwap( i1, i2 );
    this->addTopologyChange(e);
}


void PointSetTopologyModifier::addPointsProcess(const unsigned int nPoints)
{
    m_container->addPoints(nPoints);
}

void PointSetTopologyModifier::addPointsWarning(const unsigned int nPoints, const bool addDOF)
{
    if(addDOF)
    {
        PointsAdded *e2 = new PointsAdded(nPoints);
        addStateChange(e2);
        propagateStateChanges();
    }

    // Warning that vertices just got created
    PointsAdded *e = new PointsAdded(nPoints);
    this->addTopologyChange(e);
}


void PointSetTopologyModifier::addPointsWarning(const unsigned int nPoints,
        const sofa::helper::vector< sofa::helper::vector< unsigned int > > &ancestors,
        const sofa::helper::vector< sofa::helper::vector< double       > >& coefs,
        const bool addDOF)
{
    if(addDOF)
    {
        PointsAdded *e2 = new PointsAdded(nPoints, ancestors, coefs);
        addStateChange(e2);
        propagateStateChanges();
    }

    // Warning that vertices just got created
    PointsAdded *e = new PointsAdded(nPoints, ancestors, coefs);
    this->addTopologyChange(e);
}


void PointSetTopologyModifier::movePointsProcess (const sofa::helper::vector <unsigned int>& id,
        const sofa::helper::vector< sofa::helper::vector< unsigned int > >& ancestors,
        const sofa::helper::vector< sofa::helper::vector< double > >& coefs,
        const bool moveDOF)
{
    if(moveDOF)
    {
        PointsMoved *ev = new PointsMoved(id, ancestors, coefs);
        addStateChange(ev);
        propagateStateChanges();
    }

    // Warning that vertices just been moved
    PointsMoved *ev2 = new PointsMoved(id, ancestors, coefs);
    this->addTopologyChange(ev2);
}



void PointSetTopologyModifier::removePointsWarning(sofa::helper::vector<unsigned int> &indices,
        const bool removeDOF)
{
    // sort points so that they are removed in a descending order
    std::sort( indices.begin(), indices.end(), std::greater<unsigned int>() );

    // Warning that these vertices will be deleted
    PointsRemoved *e = new PointsRemoved(indices);
    this->addTopologyChange(e);

    if(removeDOF)
    {
        PointsRemoved *e2 = new PointsRemoved(indices);
        addStateChange(e2);
    }
}


void PointSetTopologyModifier::removePointsProcess(const sofa::helper::vector<unsigned int> & indices,
        const bool removeDOF)
{
    if(removeDOF)
    {
        propagateStateChanges();
    }
    m_container->removePoints(indices.size());
}


void PointSetTopologyModifier::renumberPointsWarning( const sofa::helper::vector<unsigned int> &index,
        const sofa::helper::vector<unsigned int> &inv_index,
        const bool renumberDOF)
{
    // Warning that these vertices will be deleted
    PointsRenumbering *e = new PointsRenumbering(index, inv_index);
    this->addTopologyChange(e);

    if(renumberDOF)
    {
        PointsRenumbering *e2 = new PointsRenumbering(index, inv_index);
        addStateChange(e2);
    }
}


void PointSetTopologyModifier::renumberPointsProcess( const sofa::helper::vector<unsigned int> &/*index*/,
        const sofa::helper::vector<unsigned int> &/*inv_index*/,
        const bool renumberDOF)
{
    if(renumberDOF)
    {
        propagateStateChanges();
    }
}

void PointSetTopologyModifier::propagateTopologicalChanges()
{
    if (m_container->firstChange() == m_container->lastChange()) return; // nothing to do if no event is stored
    sofa::simulation::TopologyChangeVisitor a(m_container);

// std::cout << getName() << " propagation du truc: " << getContext()->getName() << std::endl;
// for( std::list<const core::componentmodel::topology::TopologyChange *>::const_iterator it = m_container->firstChange(); it != m_container->lastChange(); it++)
// std:: cout << (*it)->getChangeType() << std::endl;

    getContext()->executeVisitor(&a);
    // remove the changes we just propagated, so that we don't send then again next time
    m_container->resetTopologyChangeList();
}

void PointSetTopologyModifier::propagateStateChanges()
{
    if (m_container->firstStateChange() == m_container->lastStateChange()) return; // nothing to do if no event is stored
    sofa::simulation::StateChangeVisitor a(m_container);
    getContext()->executeVisitor(&a);

    // remove the changes we just propagated, so that we don't send then again next time
    m_container->resetStateChangeList();
}

void PointSetTopologyModifier::notifyEndingEvent()
{
    sofa::core::componentmodel::topology::EndingEvent *e=new sofa::core::componentmodel::topology::EndingEvent();
    m_container->addTopologyChange(e);
}

} // namespace topology

} // namespace component

} // namespace sofa

