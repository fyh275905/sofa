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
#ifndef SOFA_CORE_COMPONENTMODEL_TOPOLOGY_BASETOPOLOGY_H
#define SOFA_CORE_COMPONENTMODEL_TOPOLOGY_BASETOPOLOGY_H

#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/core/componentmodel/topology/BaseTopologyObject.h>

#include <sofa/helper/vector.h>
#include <list>
#include <string>


namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace topology
{
using core::componentmodel::topology::BaseMeshTopology;

// forward declarations:

/// Provides high-level topology algorithms (e.g. CutAlongPlane, DecimateTopology, etc).
class TopologyAlgorithms;

/// Provides some geometric functions (e.g. ComputeTriangleNormal, ComputeShell, etc).
class GeometryAlgorithms;

/// Provides low-level topology methods (e.g. AddPoint, RemoveEdge, etc).
class TopologyModifier;

/// Contains the actual topology data and give acces to it.
class TopologyContainer;

/// Translates topology events (TopologyChange objects) from a topology so that they apply on another one.
class TopologicalMapping;


/** A class that contains a set of high-level (user frisendly) methods that perform topological changes */
class SOFA_CORE_API TopologyAlgorithms : public sofa::core::componentmodel::topology::BaseTopologyObject
{
protected:
    /** \brief Constructor.
    *
    */
    TopologyAlgorithms()
    {}

public:
    /// Destructor
    virtual ~TopologyAlgorithms()
    {}

    virtual void init();

protected:
    /** \brief Adds a TopologyChange object to the list of the topology this object describes.
    */
    void addTopologyChange(const TopologyChange *topologyChange);

protected:
    /// Contains the actual topology data and give acces to it (nature of these data heavily depends on the kind of topology).
    TopologyContainer *m_topologyContainer;
};

/** A class that contains a set of methods that describes the geometry of the object */
class SOFA_CORE_API GeometryAlgorithms : public sofa::core::componentmodel::topology::BaseTopologyObject
{
protected:
    /** \brief Constructor.
    *
    */
    GeometryAlgorithms()
    {}

public:
    /// Destructor
    virtual ~GeometryAlgorithms()
    {}

    virtual void init();
};

/** A class that contains a set of low-level methods that perform topological changes */
class SOFA_CORE_API TopologyModifier : public sofa::core::componentmodel::topology::BaseTopologyObject
{
protected:
    /** \brief Constructor.
    *
    */
    TopologyModifier()
        : m_topologyContainer(NULL)
    { }

public:
    /// Destructor
    virtual ~TopologyModifier()
    { }

    virtual void init();

    /** \brief Called by a topology to warn the Mechanical Object component that points have been added or will be removed.
    *
    * StateChangeList should contain all TopologyChange objects corresponding to vertex changes in this topology
    * that just happened (in the case of creation) or are about to happen (in the case of destruction) since
    * last call to propagateTopologicalChanges.
    *
    * @sa firstChange()
    * @sa lastChange()
    */
    virtual void propagateStateChanges();

    /** \brief Called by a topology to warn specific topologies linked to it that TopologyChange objects happened.
    *
    * ChangeList should contain all TopologyChange objects corresponding to changes in this topology
    * that just happened (in the case of creation) or are about to happen (in the case of destruction) since
    * last call to propagateTopologicalChanges.
    *
    * @sa firstChange()
    * @sa lastChange()
    */
    virtual void propagateTopologicalChanges();

    /** \notify the end for the current sequence of topological change events.
    */
    virtual void notifyEndingEvent();

    /** \brief Generic method to remove a list of items.
    */
    virtual void removeItems(sofa::helper::vector< unsigned int >& /*items*/);

protected:
    /** \brief Adds a TopologyChange object to the list of the topology this object describes.
    */
    void addTopologyChange(const TopologyChange *topologyChange);

    /** \brief Adds a StateChange object to the list of the topology this object describes.
    */
    void addStateChange(const TopologyChange *topologyChange);

protected:
    /// Contains the actual topology data and give acces to it (nature of these data heavily depends on the kind of topology).
    TopologyContainer *m_topologyContainer;
};

/** A class that contains a description of the topology (set of edges, triangles, adjacency information, ...) */
class SOFA_CORE_API TopologyContainer : public sofa::core::componentmodel::topology::BaseTopologyObject,
    public core::componentmodel::topology::BaseMeshTopology
{
protected:
    /** \brief Constructor.
    *
    */
    TopologyContainer()
    {}

public:
    /// Destructor
    virtual ~TopologyContainer()
    {}

    virtual void init();

    /// BaseMeshTopology API
    /// @{
    virtual const SeqEdges& getEdges()         { static SeqEdges     empty; return empty; }
    virtual const SeqTriangles& getTriangles() { static SeqTriangles empty; return empty; }
    virtual const SeqQuads& getQuads()         { static SeqQuads     empty; return empty; }
    virtual const SeqTetras& getTetras()       { static SeqTetras    empty; return empty; }
    virtual const SeqHexas& getHexas()         { static SeqHexas     empty; return empty; }
    /// @}


    const std::list<const TopologyChange *> &getChangeList() const { return m_changeList; }

    const std::list<const TopologyChange *> &getStateChangeList() const { return m_stateChangeList; }

    /** \brief Adds a TopologyChange to the list.
    *
    * Needed by topologies linked to this one to know what happened and what to do to take it into account.
    *
    */
    void addTopologyChange(const TopologyChange *topologyChange)
    {
        m_changeList.push_back(topologyChange);
    }

    /** \brief Adds a StateChange to the list.
    *
    * Needed by topologies linked to this one to know what happened and what to do to take it into account.
    *
    */
    void addStateChange(const TopologyChange *topologyChange)
    {
        m_stateChangeList.push_back(topologyChange);
    }

    /** \brief Provides an iterator on the first element in the list of TopologyChange objects.
     */
    std::list<const TopologyChange *>::const_iterator firstChange() const;

    /** \brief Provides an iterator on the last element in the list of TopologyChange objects.
     */
    std::list<const TopologyChange *>::const_iterator lastChange() const;

    /** \brief Provides an iterator on the first element in the list of StateChange objects.
     */
    std::list<const TopologyChange *>::const_iterator firstStateChange() const;

    /** \brief Provides an iterator on the last element in the list of StateChange objects.
     */
    std::list<const TopologyChange *>::const_iterator lastStateChange() const;


    /** \brief Free each Topology changes in the list and remove them from the list
    *
    */
    void resetTopologyChangeList();

    /** \brief Free each State changes in the list and remove them from the list
    *
    */
    void resetStateChangeList();

private:
    /// Array of topology modifications that have already occured (addition) or will occur next (deletion).
    std::list<const TopologyChange *> m_changeList;

    /// Array of state modifications that have already occured (addition) or will occur next (deletion).
    std::list<const TopologyChange *> m_stateChangeList;
};

} // namespace topology

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif // SOFA_CORE_BASICTOPOLOGY_H
