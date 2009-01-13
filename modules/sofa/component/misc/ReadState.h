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
#ifndef SOFA_COMPONENT_MISC_READSTATE_H
#define SOFA_COMPONENT_MISC_READSTATE_H

#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalState.h>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/objectmodel/Event.h>
#include <sofa/simulation/common/AnimateBeginEvent.h>
#include <sofa/simulation/common/AnimateEndEvent.h>
#include <sofa/simulation/common/Visitor.h>
#include <sofa/component/component.h>

#include <fstream>

namespace sofa
{

namespace component
{

namespace misc
{

/** Read State vectors from file at each timestep
*/
class SOFA_COMPONENT_MISC_API ReadState: public core::objectmodel::BaseObject
{
public:

    Data < std::string > f_filename;
    Data < double > f_interval;
    Data < double > f_shift;

protected:
    core::componentmodel::behavior::BaseMechanicalState* mmodel;
    std::ifstream* infile;
    double nextTime;
    double lastTime;
public:
    ReadState();

    virtual ~ReadState();

    virtual void init();

    virtual void reset();

    void setTime(double time);

    virtual void handleEvent(sofa::core::objectmodel::Event* event);

    void processReadState();
    void processReadState(double time);

    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MechanicalState.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        if (dynamic_cast<core::componentmodel::behavior::BaseMechanicalState*>(context->getMechanicalState()) == NULL)
            return false;
        return BaseObject::canCreate(obj, context, arg);
    }


};


///Create ReadState component in the graph each time needed
class SOFA_COMPONENT_MISC_API ReadStateCreator: public Visitor
{
public:
    ReadStateCreator():sceneName(""), createInMapping(false), counterReadState(0) {}
    ReadStateCreator(std::string &n, bool _createInMapping, bool i=true, int c=0 ): sceneName(n), createInMapping(_createInMapping), init(i) , counterReadState(c) {}
    virtual Result processNodeTopDown( simulation::Node*  );

    void setSceneName(std::string &n) { sceneName = n;}
    void setCounter(int c) {counterReadState = c;};
    void setCreateInMapping(bool b) {createInMapping=b;}
protected:
    void addReadState(sofa::core::componentmodel::behavior::BaseMechanicalState *ms, simulation::Node* gnode);
    std::string sceneName;
    bool createInMapping;
    bool init;
    int counterReadState; //avoid to have two same files if two mechanical objects has the same name
};

class SOFA_COMPONENT_MISC_API ReadStateActivator: public Visitor
{
public:
    ReadStateActivator( bool active):state(active) {}
    virtual Result processNodeTopDown( simulation::Node*  );

    bool getState() const {return state;};
    void setState(bool active) {state=active;};
protected:
    void changeStateReader(sofa::component::misc::ReadState *ws);

    bool state;
};

class SOFA_COMPONENT_MISC_API ReadStateModifier: public simulation::Visitor
{
public:
    ReadStateModifier( double _time):time(_time) {}
    virtual Result processNodeTopDown( simulation::Node*  );

    double getTime() const { return time; }
    void setTime(double _time) { time=_time; }
protected:
    void changeTimeReader(sofa::component::misc::ReadState *rs) { rs->processReadState(time); }

    double time;
};

} // namespace misc

} // namespace component

} // namespace sofa

#endif
