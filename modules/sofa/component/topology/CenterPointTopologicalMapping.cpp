/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#include "CenterPointTopologicalMapping.h"

#include <sofa/core/ObjectFactory.h>

#include <sofa/component/topology/PointSetTopologyModifier.h>
#include <sofa/component/topology/PointSetGeometryAlgorithms.h>
#include <sofa/component/topology/HexahedronSetTopologyChange.h>

#include <sofa/defaulttype/Vec.h>
#include <map>
#include <sofa/defaulttype/VecTypes.h>

namespace sofa
{
namespace component
{
namespace topology
{
using namespace sofa::defaulttype;
using namespace sofa::component::topology;
using namespace sofa::core::componentmodel::topology;

SOFA_DECL_CLASS ( CenterPointTopologicalMapping )

// Register in the Factory
int CenterPointTopologicalMappingClass = core::RegisterObject ( "" )
        .add< CenterPointTopologicalMapping >()
        ;

// Implementation
CenterPointTopologicalMapping::CenterPointTopologicalMapping ( In* from, Out* to )
    : TopologicalMapping ( from, to ),
      object1(initData(&object1, std::string("../.."), "object1", "First object to map")),
      object2(initData(&object2, std::string(".."), "object2", "Second object to map"))
{
}

void CenterPointTopologicalMapping::init()
{
    if(fromModel && toModel)
    {
        toModel->setNbPoints(fromModel->getNbHexas());

#ifdef SOFA_FLOAT
        typedef Vec3fTypes DataTypes;
#else
        typedef Vec3dTypes DataTypes;
#endif

        PointSetGeometryAlgorithms<DataTypes> *geomAlgo = NULL;
        toModel->getContext()->get(geomAlgo);

        geomAlgo->getDOF()->resize(fromModel->getNbHexas());
    }
}

void CenterPointTopologicalMapping::updateTopologicalMappingTopDown()
{
    if(fromModel && toModel)
    {
        std::list<const TopologyChange *>::const_iterator changeIt = fromModel->firstChange();
        std::list<const TopologyChange *>::const_iterator itEnd = fromModel->lastChange();

        PointSetTopologyModifier *to_pstm;
        toModel->getContext()->get(to_pstm);

        while( changeIt != itEnd )
        {
            TopologyChangeType changeType = (*changeIt)->getChangeType();

            switch( changeType )
            {
            case core::componentmodel::topology::HEXAHEDRAADDED:
            {
                const unsigned int nbHexaAdded = ( static_cast< const HexahedraAdded *>( *changeIt ) )->getNbAddedHexahedra();
                to_pstm->addPointsProcess(nbHexaAdded);
                to_pstm->addPointsWarning(nbHexaAdded, true);
                to_pstm->propagateTopologicalChanges();
                break;
            }
            case core::componentmodel::topology::HEXAHEDRAREMOVED:
            {
                sofa::helper::vector<unsigned int> tab = ( static_cast< const HexahedraRemoved *>( *changeIt ) )->getArray();
                to_pstm->removePointsWarning(tab, true);
                to_pstm->propagateTopologicalChanges();
                to_pstm->removePointsProcess(tab, true);
                break;
            }
            }
            ++changeIt;
        }
    }
}

} // namespace topology
} // namespace component
} // namespace sofa

