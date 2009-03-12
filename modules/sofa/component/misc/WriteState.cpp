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
#include <sofa/component/misc/WriteState.inl>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace misc
{

SOFA_DECL_CLASS(WriteState)

using namespace defaulttype;



int WriteStateClass = core::RegisterObject("Write State vectors to file at each timestep")
        .add< WriteState >();




//Create a Write State component each time a mechanical state is found
simulation::Visitor::Result WriteStateCreator::processNodeTopDown( simulation::Node* gnode)
{
    sofa::core::componentmodel::behavior::BaseMechanicalState * mstate = dynamic_cast<sofa::core::componentmodel::behavior::BaseMechanicalState *>( gnode->getMechanicalState());
    if (!mstate)   return simulation::Visitor::RESULT_CONTINUE;
    //We have a mechanical state
    addWriteState(mstate, gnode);
    return simulation::Visitor::RESULT_CONTINUE;
}


void WriteStateCreator::addWriteState(sofa::core::componentmodel::behavior::BaseMechanicalState *ms, simulation::Node* gnode)
{
    sofa::core::objectmodel::BaseContext* context = gnode->getContext();
    sofa::core::BaseMapping *mapping;
    context->get(mapping);
    if ( createInMapping || mapping == NULL)
    {
        sofa::component::misc::WriteState *ws;
        context->get(ws, core::objectmodel::BaseContext::Local);
        if ( ws == NULL )
        {
            ws = new sofa::component::misc::WriteState(); gnode->addObject(ws);
            ws->f_writeX.setValue(recordX);
            ws->f_writeV.setValue(recordV);
        }

        std::ostringstream ofilename;
        ofilename << sceneName << "_" << counterWriteState << "_" << ms->getName()  << "_mstate.txt" ;

        ws->f_filename.setValue(ofilename.str()); ws->init(); ws->f_listening.setValue(true);  //Activated at init

        ++counterWriteState;
    }
}



//if state is true, we activate all the write states present in the scene.
simulation::Visitor::Result WriteStateActivator::processNodeTopDown( simulation::Node* gnode)
{
    sofa::component::misc::WriteState *ws = gnode->get< sofa::component::misc::WriteState >();
    if (ws != NULL) { changeStateWriter(ws);}
    return simulation::Visitor::RESULT_CONTINUE;
}

void WriteStateActivator::changeStateWriter(sofa::component::misc::WriteState*ws)
{
    if (!state) ws->reset();
    ws->f_listening.setValue(state);
}






} // namespace misc

} // namespace component

} // namespace sofa
