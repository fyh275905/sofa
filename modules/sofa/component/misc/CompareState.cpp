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

#ifndef SOFA_COMPONENT_MISC_COMPARESTATE_INL
#define SOFA_COMPONENT_MISC_COMPARESTATE_INL

#include <sofa/component/misc/CompareState.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/UpdateMappingVisitor.h>

#include <sstream>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace misc
{



int CompareStateClass = core::RegisterObject("Compare State vectors from a reference frame to the associated Mechanical State")
        .add< CompareState >();

CompareState::CompareState(): ReadState()
{
    totalError_X=0.0;
    totalError_V=0.0;
    dofError_X=0.0;
    dofError_V=0.0;
}


//-------------------------------- handleEvent-------------------------------------------
void CompareState::handleEvent(sofa::core::objectmodel::Event* event)
{
    if (/* simulation::AnimateBeginEvent* ev = */ dynamic_cast<simulation::AnimateBeginEvent*>(event))
    {
        processCompareState();
    }
    if (/* simulation::AnimateEndEvent* ev = */ dynamic_cast<simulation::AnimateEndEvent*>(event))
    {
    }
}

//-------------------------------- processCompareState------------------------------------
void CompareState::processCompareState()
{
    double time = getContext()->getTime() + f_shift.getValue();
    time += getContext()->getDt() * 0.001;
    //lastTime = time+0.00001;
    std::vector<std::string> validLines;
    if (!this->readNext(time, validLines)) return;
    for (std::vector<std::string>::iterator it=validLines.begin(); it!=validLines.end(); ++it)
    {
        std::istringstream str(*it);
        std::string cmd;
        str >> cmd;
        double currentError=0;
        if (cmd == "X=")
        {
            currentError = mmodel->compareX(str);
            totalError_X +=currentError;
            dofError_X +=currentError/(double)this->mmodel->getSize();
        }
        else if (cmd == "V=")
        {
            currentError = mmodel->compareV(str);
            totalError_V +=currentError;
            dofError_V += currentError/(double)this->mmodel->getSize();
        }
    }
}






CompareStateCreator::CompareStateCreator()
    : sceneName("")
#ifdef SOFA_HAVE_ZLIB
    , extension(".txt.gz")
#else
    , extension(".txt")
#endif
    , createInMapping(false)
    , init(true)
    , counterCompareState(0)
{
}

CompareStateCreator::CompareStateCreator(const std::string &n, bool i, int c)
    : sceneName(n)
#ifdef SOFA_HAVE_ZLIB
    , extension(".txt.gz")
#else
    , extension(".txt")
#endif
    , createInMapping(false)
    , init(i)
    , counterCompareState(c)
{
}

//Create a Compare State component each time a mechanical state is found
simulation::Visitor::Result CompareStateCreator::processNodeTopDown( simulation::Node* gnode)
{
    using namespace sofa::defaulttype;
    sofa::core::behavior::BaseMechanicalState * mstate = gnode->mechanicalState;
    if (!mstate)   return simulation::Visitor::RESULT_CONTINUE;
    //We have a mechanical state
    addCompareState(mstate, gnode);
    return simulation::Visitor::RESULT_CONTINUE;
}



void CompareStateCreator::addCompareState(sofa::core::behavior::BaseMechanicalState *ms, simulation::Node* gnode)
{
    sofa::core::objectmodel::BaseContext* context = gnode->getContext();
    sofa::core::BaseMapping *mapping; context->get(mapping);
    if (createInMapping || mapping== NULL)
    {
        sofa::component::misc::CompareState *rs; context->get(rs, core::objectmodel::BaseContext::Local);
        if (  rs == NULL )
        {
            rs = new sofa::component::misc::CompareState(); gnode->addObject(rs);
        }

        std::ostringstream ofilename;
        ofilename << sceneName << "_" << counterCompareState << "_" << ms->getName()  << "_mstate" << extension ;

        rs->f_filename.setValue(ofilename.str());  rs->f_listening.setValue(false); //Deactivated only called by extern functions
        if (init) rs->init();

        ++counterCompareState;
    }
}



//Create a Compare State component each time a mechanical state is found
simulation::Visitor::Result CompareStateResult::processNodeTopDown( simulation::Node* gnode)
{
    sofa::component::misc::CompareState *cv;
    gnode->get(cv);
    if (!cv)   return simulation::Visitor::RESULT_CONTINUE;
    //We have a mechanical state
    error += cv->getTotalError();
    errorByDof += cv->getErrorByDof();
    numCompareState++;
    return simulation::Visitor::RESULT_CONTINUE;
}



} // namespace misc

} // namespace component

} // namespace sofa

#endif
