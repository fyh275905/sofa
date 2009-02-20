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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CORE_COMPONENTMODEL_TOPOLOGY_TOPOLOGY_H
#define SOFA_CORE_COMPONENTMODEL_TOPOLOGY_TOPOLOGY_H

#include <stdlib.h>
#include <string>
#include <iostream>
#include <sofa/core/objectmodel/BaseObject.h>

#include <sofa/helper/vector.h>
namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace topology
{

/// The enumeration used to give unique identifiers to TopologyChange objects.
enum TopologyChangeType
{
    BASE,               ///< For TopologyChange class, should never be used.
    ENDING_EVENT,       ///< To notify the end for the current sequence of topological change events

    POINTSINDICESSWAP,  ///< For PointsIndicesSwap class.
    POINTSADDED,        ///< For PointsAdded class.
    POINTSREMOVED,      ///< For PointsRemoved class.
    POINTSRENUMBERING,  ///< For PointsRenumbering class.

    EDGESADDED,         ///< For EdgesAdded class.
    EDGESREMOVED,       ///< For EdgesRemoved class.
    EDGESRENUMBERING,    ///< For EdgesRenumbering class.

    TRIANGLESADDED,     ///< For TrianglesAdded class.
    TRIANGLESREMOVED,   ///< For TrianglesRemoved class.
    TRIANGLESRENUMBERING, ///< For TrianglesRenumbering class.

    TETRAHEDRAADDED,     ///< For TrianglesAdded class.
    TETRAHEDRAREMOVED,   ///< For TrianglesRemoved class.
    TETRAHEDRARENUMBERING, ///< For TrianglesRenumbering class.

    QUADSADDED,          ///< For QuadsAdded class.
    QUADSREMOVED,        ///< For QuadsRemoved class.
    QUADSRENUMBERING,    ///< For QuadsRenumbering class.

    HEXAHEDRAADDED,      ///< For TrianglesAdded class.
    HEXAHEDRAREMOVED,    ///< For TrianglesRemoved class.
    HEXAHEDRARENUMBERING, ///< For TrianglesRenumbering class.

    TOPOLOGYCHANGE_LASTID ///< user defined topology changes can start here
};


/** \brief Base class to indicate a topology change occurred.
*
* All topological changes taking place in a given BaseTopology will issue a TopologyChange in the
* BaseTopology's changeList, so that BasicTopologies mapped to it can know what happened and decide how to
* react.
* Classes inheriting from this one describe a given topolopy change (e.g. RemovedPoint, AddedEdge, etc).
* The exact type of topology change is given by member changeType.
*/
class TopologyChange
{
public:
    /** \ brief Destructor.
    *
    * Must be virtual for TopologyChange to be a Polymorphic type.
    */
    virtual ~TopologyChange() {};

    /** \brief Returns the code of this TopologyChange. */
    TopologyChangeType getChangeType() const { return m_changeType;}

protected:
    TopologyChange( TopologyChangeType changeType = BASE )
        : m_changeType(changeType)
    {}

    TopologyChangeType m_changeType; ///< A code that tells the nature of the Topology modification event (could be an enum).
};

/** notifies the end for the current sequence of topological change events */
class EndingEvent : public core::componentmodel::topology::TopologyChange
{
public:
    EndingEvent()
        : core::componentmodel::topology::TopologyChange(core::componentmodel::topology::ENDING_EVENT)
    {}
};


class Topology : public virtual core::objectmodel::BaseObject
{
public:
    Topology():BaseObject() {}
    virtual ~Topology()
    {}

    // Access to embedded position information (in case the topology is a regular grid for instance)
    // This is not very clean and is quit slow but it should only be used during initialization

    virtual bool hasPos() const { return false; }
    virtual int getNbPoints() const { return 0; }
    virtual void setNbPoints(int /*n*/) {}
    virtual double getPX(int /*i*/) const { return 0.0; }
    virtual double getPY(int /*i*/) const { return 0.0; }
    virtual double getPZ(int /*i*/) const { return 0.0; }
};

} // namespace topology

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
