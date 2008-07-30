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
#ifndef SOFA_COMPONENT_TOPOLOGY_QUAD2TRIANGLETOPOLOGICALMAPPING_H
#define SOFA_COMPONENT_TOPOLOGY_QUAD2TRIANGLETOPOLOGICALMAPPING_H

#include <sofa/core/componentmodel/topology/TopologicalMapping.h>

#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>

#include <sofa/component/topology/TriangleSetTopology.h>
#include <sofa/component/topology/QuadSetTopology.h>

#include <sofa/defaulttype/Vec.h>
#include <map>

#include <sofa/core/BaseMapping.h>

namespace sofa
{

namespace component
{

namespace topology
{


using namespace sofa::defaulttype;

using namespace sofa::component::topology;
using namespace sofa::core::componentmodel::topology;

using namespace sofa::core;

/**
 * This class, called Quad2TriangleTopologicalMapping, is a specific implementation of the interface TopologicalMapping where :
 *
 * INPUT TOPOLOGY = QuadSetTopology
 * OUTPUT TOPOLOGY = TriangleSetTopology, as the constitutive elements of the INPUT TOPOLOGY
 *
 * Quad2TriangleTopologicalMapping class is templated by the pair (INPUT TOPOLOGY, OUTPUT TOPOLOGY)
 *
*/

class Quad2TriangleTopologicalMapping : public TopologicalMapping
{
public:
    /// Input Topology
    typedef BaseMeshTopology In;
    /// Output Topology
    typedef BaseMeshTopology Out;

protected:
    /// Input source BaseTopology
    In* fromModel;
    /// Output target BaseTopology
    Out* toModel;

    Data< std::string > object1;
    Data< std::string > object2;

public:

    /** \brief Constructor.
     *
     * @param from the topology issuing TopologyChange objects (the "source").
     * @param to   the topology for which the TopologyChange objects must be translated (the "target").
     */
    Quad2TriangleTopologicalMapping(In* from=NULL, Out* to=NULL);

    /** \brief Destructor.
     *
     * Does nothing.
     */
    virtual ~Quad2TriangleTopologicalMapping();

    /// Specify the input and output topologies.
    virtual void setModels(In* from, Out* to);

    /// Return the pointer to the input topology.
    In* getFromModel();
    /// Return the pointer to the output topology.
    Out* getToModel();

    /// Return the pointer to the input topology.
    objectmodel::BaseObject* getFrom();
    /// Return the pointer to the output topology.
    objectmodel::BaseObject* getTo();

    /** \brief Initializes the target BaseTopology from the source BaseTopology.
     */
    virtual void init();


    /** \brief Translates the TopologyChange objects from the source to the target.
     *
     * Translates each of the TopologyChange objects waiting in the source list so that they have a meaning and
     * reflect the effects of the first topology changes on the second topology.
     *
     */
    virtual void updateTopologicalMapping();

    virtual unsigned int getFromIndex(unsigned int ind);

    /// Pre-construction check method called by ObjectFactory.
    ///
    /// This implementation read the object1 and object2 attributes and check
    /// if they are compatible with the input and output topology types of this
    /// mapping.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        if (arg->findObject(arg->getAttribute("object1","../..")) == NULL)
            std::cerr << "Cannot create "<<className(obj)<<" as object1 is missing.\n";
        if (arg->findObject(arg->getAttribute("object2","..")) == NULL)
            std::cerr << "Cannot create "<<className(obj)<<" as object2 is missing.\n";


        if (arg->findObject(arg->getAttribute("object1","../..")) == NULL || arg->findObject(arg->getAttribute("object2","..")) == NULL)
            return false;

        BaseMeshTopology* topoIn;
        BaseMeshTopology* topoOut;
        (dynamic_cast<sofa::core::objectmodel::BaseObject*>(arg->findObject(arg->getAttribute("object1","../.."))))->getContext()->get(topoIn);
        (dynamic_cast<sofa::core::objectmodel::BaseObject*>(arg->findObject(arg->getAttribute("object2",".."))))->getContext()->get(topoOut);

        if (dynamic_cast<In*>(topoIn) == NULL)
            return false;
        if (dynamic_cast<Out*>(topoOut) == NULL)
            return false;
        return BaseMapping::canCreate(obj, context, arg);
    }

    /// Construction method called by ObjectFactory.
    ///
    /// This implementation read the object1 and object2 attributes to
    /// find the input and output topologies of this mapping.
    template<class T>
    static void create(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        BaseMeshTopology* topoIn=NULL;
        BaseMeshTopology* topoOut=NULL;
        if (arg)
        {
            if (arg->findObject(arg->getAttribute("object1","../..")) != NULL)
                (dynamic_cast<sofa::core::objectmodel::BaseObject*>(arg->findObject(arg->getAttribute("object1","../.."))))->getContext()->get(topoIn);
            if (arg->findObject(arg->getAttribute("object2","..")) != NULL)
                (dynamic_cast<sofa::core::objectmodel::BaseObject*>(arg->findObject(arg->getAttribute("object2",".."))))->getContext()->get(topoOut);
        }

        obj = new T(
            (arg?dynamic_cast<In*>(topoIn):NULL),
            (arg?dynamic_cast<Out*>(topoOut):NULL));
        if (context) context->addObject(obj);
        if ((arg) && (arg->getAttribute("object1")))
        {
            obj->object1.setValue( arg->getAttribute("object1") );
            arg->removeAttribute("object1");
        }
        if ((arg) && (arg->getAttribute("object2")))
        {
            obj->object2.setValue( arg->getAttribute("object2") );
            arg->removeAttribute("object2");
        }
        if (arg) obj->parse(arg);
    }

};

} // namespace topology

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENT_TOPOLOGY_QUAD2TRIANGLETOPOLOGICALMAPPING_H
