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
#ifndef SOFA_COMPONENT_MISC_TOPOLOGICALCHANGEPROCESSOR_H
#define SOFA_COMPONENT_MISC_TOPOLOGICALCHANGEPROCESSOR_H

#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/core/componentmodel/topology/BaseTopology.h>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/objectmodel/Event.h>

#include <sofa/simulation/common/AnimateBeginEvent.h>
#include <sofa/simulation/common/AnimateEndEvent.h>

#include <sofa/defaulttype/DataTypeInfo.h>
#include <sofa/simulation/common/Visitor.h>
#include <sofa/component/component.h>

#ifdef SOFA_HAVE_ZLIB
#include <zlib.h>
#endif

#include <fstream>

namespace sofa
{

namespace component
{

namespace misc
{

/** Read file containing topological modification. Or apply input modifications
 * A timestep has to be established for each modification.
 *
 * SIMPLE METHODE FOR THE MOMENT. DON'T HANDLE MULTIPLE TOPOLOGIES
*/
class SOFA_COMPONENT_MISC_API TopologicalChangeProcessor: public core::objectmodel::BaseObject
{
public:
    SOFA_CLASS(TopologicalChangeProcessor,core::objectmodel::BaseObject);

    sofa::core::objectmodel::DataFileName m_filename;
    Data < helper::vector< helper::vector <unsigned int> > > m_listChanges;
    Data < double > m_interval;
    Data < double > m_shift;
    Data < bool > m_loop;


protected:
    core::componentmodel::topology::BaseMeshTopology* m_topology;

    std::ifstream* infile;
#ifdef SOFA_HAVE_ZLIB
    gzFile gzfile;
#endif
    double nextTime;
    double lastTime;
    double loopTime;

public:
    TopologicalChangeProcessor();

    virtual ~TopologicalChangeProcessor();

    virtual void init();

    virtual void reinit();

    virtual void reset();

    virtual void handleEvent(sofa::core::objectmodel::Event* event);

    void setTime(double time);

    void processTopologicalChanges();
    void processTopologicalChanges(double time);

    bool readNext(double time, std::vector<std::string>& lines);



    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MeshTopology.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        if (dynamic_cast<core::componentmodel::topology::BaseMeshTopology*>(context->getMeshTopology()) == NULL)
            return false;

        return BaseObject::canCreate(obj, context, arg);
    }

};



} // namespace misc

} // namespace component

} // namespace sofa

#endif
