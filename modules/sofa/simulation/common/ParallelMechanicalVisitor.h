/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef SOFA_SMP_PARALLELMECHANICALACTION_H
#define SOFA_SMP_PARALLELMECHANICALACTION_H
//#define SOFA_SUPPORT_MAPPED_MASS
#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif


#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalMapping.h>
#include <sofa/core/componentmodel/behavior/Mass.h>
#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/behavior/InteractionForceField.h>
#include <sofa/core/componentmodel/behavior/InteractionConstraint.h>
#include <sofa/core/componentmodel/behavior/Constraint.h>
#include <sofa/defaulttype/SharedTypes.h>

//#include <sofa/defaulttype/BaseMatrix.h>
//#include <sofa/defaulttype/BaseVector.h>
#include <iostream>
using std::cerr;
using std::endl;

namespace sofa
{

namespace simulation
{

using namespace sofa::defaulttype::SharedTypes;

using namespace sofa::defaulttype;
using namespace sofa::simulation;

/** Base class for easily creating new actions for mechanical simulation.

During the first traversal (top-down), method processNodeTopDown(Node*) is applied to each Node. Each component attached to this node is processed using the appropriate method, prefixed by fwd.

During the second traversal (bottom-up), method processNodeBottomUp(Node*) is applied to each Node. Each component attached to this node is processed using the appropriate method, prefixed by bwd.

The default behavior of the fwd* and bwd* is to do nothing. Derived actions typically overload these methods to implement the desired processing.

*/



/** Perform a vector operation v=a-b*f
*/
class ParallelMechanicalVOpMecVisitor : virtual public MechanicalVisitor
{
public:
    VecId v;
    VecId a;
    VecId b;
    double f;
    Shared<double> *fSh;


#ifdef SOFA_DUMP_VISITOR_INFO
    virtual void setReadWriteVectors() {}
#endif
    ParallelMechanicalVOpMecVisitor(VecId v, VecId a = VecId::null(),Shared<double> *fSh=NULL)
        : v(v), a(a),fSh(fSh)
    {}

    virtual Result fwdMechanicalState(Node* /*node*/, sofa::core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(Node* /*node*/,sofa::core::componentmodel::behavior::BaseMechanicalState* /*mm*/);

    //virtual void processNodeBottomUp(simulation::Node* node);

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Perform a vector operation v=a+b*f
*/
class ParallelMechanicalVOpVisitor : virtual public MechanicalVisitor, virtual public Visitor
{
public:
    VecId v;
    VecId a;
    VecId b;
    double f;
    Shared<double> *fSh;

#ifdef SOFA_DUMP_VISITOR_INFO
    virtual void setReadWriteVectors() {}
#endif

    virtual const char* getClassName() const
    {
        return "ParallelMechanicalVOpVisitor";
    }

    ParallelMechanicalVOpVisitor(VecId v, VecId a = VecId::null(), VecId b = VecId::null(), double f=1.0,Shared<double> *fSh=NULL)
        : v(v), a(a), b(b), f(f),fSh(fSh)
    {}

    Result fwdMechanicalState(Node* /*node*/, sofa::core::componentmodel::behavior::BaseMechanicalState* mm);
    Result fwdMappedMechanicalState(Node* /*node*/, sofa::core::componentmodel::behavior::BaseMechanicalState* /*mm*/);

    //virtual void processNodeBottomUp(simulation::Node* node);

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};


/** Compute the dot product of two vectors */
/** Compute the dot product of two vectors */
class ParallelMechanicalVDotVisitor : public MechanicalVisitor
{
public:
    VecId a;
    VecId b;
    double* total;
    Shared<double>* totalSh;

#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors() {}
#endif


    ParallelMechanicalVDotVisitor(Shared<double>* t,VecId a, VecId b) : a(a), b(b), totalSh(t)
    {}
    /// Sequential code
    Result fwdMechanicalState(simulation::Node* /*node*/, sofa::core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    const char* getClassName() const
    {
        return "ParallelMechanicalVDotVisitor";
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};





} // namespace simulation

} // namespace sofa

#endif
