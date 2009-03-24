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
#include <sofa/component/misc/ReadState.inl>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace misc
{

SOFA_DECL_CLASS(ReadState)

using namespace defaulttype;

int ReadStateClass = core::RegisterObject("Read State vectors from file at each timestep")
        .add< ReadState >();

ReadStateCreator::ReadStateCreator()
    : sceneName("")
#ifdef SOFA_HAVE_ZLIB
    , extension(".txt.gz")
#else
    , extension(".txt")
#endif
    , createInMapping(false)
    , init(true)
    , counterReadState(0)
{
}

ReadStateCreator::ReadStateCreator(const std::string &n, bool _createInMapping, bool i, int c)
    : sceneName(n)
#ifdef SOFA_HAVE_ZLIB
    , extension(".txt.gz")
#else
    , extension(".txt")
#endif
    , createInMapping(_createInMapping)
    , init(i)
    , counterReadState(c)
{
}

//Create a Read State component each time a mechanical state is found
simulation::Visitor::Result ReadStateCreator::processNodeTopDown( simulation::Node* gnode)
{
    using namespace sofa::defaulttype;
    sofa::core::componentmodel::behavior::BaseMechanicalState * mstate = dynamic_cast<sofa::core::componentmodel::behavior::BaseMechanicalState *>( gnode->getMechanicalState());
    if (!mstate)   return Visitor::RESULT_CONTINUE;
    //We have a mechanical state
    addReadState(mstate, gnode);
    return simulation::Visitor::RESULT_CONTINUE;
}

void ReadStateCreator::addReadState(sofa::core::componentmodel::behavior::BaseMechanicalState *ms, simulation::Node* gnode)
{
    sofa::core::objectmodel::BaseContext* context = gnode->getContext();
    sofa::core::BaseMapping *mapping; context->get(mapping);
    if (createInMapping || mapping== NULL)
    {
        sofa::component::misc::ReadState *rs; context->get(rs, core::objectmodel::BaseContext::Local);
        if (  rs == NULL )
        {
            rs = new sofa::component::misc::ReadState(); gnode->addObject(rs);
        }

        std::ostringstream ofilename;
        ofilename << sceneName << "_" << counterReadState << "_" << ms->getName()  << "_mstate" << extension ;

        rs->f_filename.setValue(ofilename.str());  rs->f_listening.setValue(false); //Desactivated only called by extern functions
        if (init) rs->init();

        ++counterReadState;
    }
}

///if state is true, we activate all the write states present in the scene.
simulation::Visitor::Result ReadStateActivator::processNodeTopDown( simulation::Node* gnode)
{
    sofa::component::misc::ReadState *rs = gnode->get< sofa::component::misc::ReadState >();
    if (rs != NULL) { changeStateReader(rs);}

    return simulation::Visitor::RESULT_CONTINUE;
}

void ReadStateActivator::changeStateReader(sofa::component::misc::ReadState* rs)
{
    rs->reset();
    rs->f_listening.setValue(state);
}


//if state is true, we activate all the write states present in the scene. If not, we activate all the readers.
simulation::Visitor::Result ReadStateModifier::processNodeTopDown( simulation::Node* gnode)
{
    using namespace sofa::defaulttype;

    sofa::component::misc::ReadState*rs = gnode->get< sofa::component::misc::ReadState>();
    if (rs != NULL) {changeTimeReader(rs);}

    return simulation::Visitor::RESULT_CONTINUE;
}

} // namespace misc

} // namespace component

} // namespace sofa
