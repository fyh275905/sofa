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
#ifndef SOFA_COMPONENT_ODESOLVER_CUDAMASTERCONTACTSOLVER_H
#define SOFA_COMPONENT_ODESOLVER_CUDAMASTERCONTACTSOLVER_H

#include <sofa/core/componentmodel/behavior/OdeSolver.h>
#include <sofa/simulation/common/MasterSolverImpl.h>
#include <sofa/simulation/common/Node.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/core/componentmodel/behavior/BaseConstraintCorrection.h>
#include <sofa/core/componentmodel/behavior/OdeSolver.h>
#include <sofa/component/odesolver/OdeSolverImpl.h>
#include <sofa/component/linearsolver/FullMatrix.h>
#include <sofa/helper/set.h>
#include <sofa/gpu/cuda/CudaLCP.h>
#include "CudaTypesBase.h"

//#define CHECK 0.01
#define DISPLAY_TIME

namespace sofa
{

namespace component
{

namespace odesolver
{

using namespace sofa::defaulttype;
using namespace sofa::component::linearsolver;
using namespace helper::system::thread;
using namespace sofa::gpu::cuda;

class MechanicalResetContactForceVisitor : public simulation::MechanicalVisitor
{
public:
    VecId force;
    MechanicalResetContactForceVisitor()
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* ms)
    {
        ms->resetContactForce();
        return RESULT_CONTINUE;
    }

    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* ms)
    {
        ms->resetForce();
        return RESULT_CONTINUE;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};

/* ACTION 2 : Apply the Contact Forces on mechanical models & Compute displacements */
class MechanicalApplyContactForceVisitor : public simulation::MechanicalVisitor
{
public:
    VecId force;
    MechanicalApplyContactForceVisitor(double *f):_f(f)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* ms)
    {
        ms->applyContactForce(_f);
        return RESULT_CONTINUE;
    }

    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* ms)
    {
        ms->applyContactForce(_f);
        return RESULT_CONTINUE;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif

private:
    double *_f; // vector of contact forces from lcp //
    // to be multiplied by constraint direction in mechanical models //

};

/* ACTION 3 : gets the vector of constraint values */
/* ACTION 3 : gets the vector of constraint values */
template<class real>
class MechanicalGetConstraintValueVisitor : public simulation::MechanicalVisitor
{
public:

    MechanicalGetConstraintValueVisitor(BaseVector * v)  // , _numContacts(numContacts)
    {
        real * data = ((CudaBaseVector<real> *) v)->getCudaVector().hostWrite();
        _v = new FullVector<real>(data,0);
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        //sout << c->getName()<<"->getConstraintValue()"<<sendl;
        c->getConstraintValue(_v /*, _numContacts*/);
        return RESULT_CONTINUE;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
private:
    FullVector<real> * _v; // vector for constraint values
    // unsigned int &_numContacts; // we need an offset to fill the vector _v if differents contact class are created
};

class CudaMechanicalGetConstraintValueVisitor : public simulation::MechanicalVisitor
{
public:
    CudaMechanicalGetConstraintValueVisitor(defaulttype::BaseVector * v): _v(v)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdConstraint(simulation::Node*,core::componentmodel::behavior::BaseConstraint* c)
    {
        c->getConstraintValue(_v);
        return RESULT_CONTINUE;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
private:
    defaulttype::BaseVector * _v;
};

class MechanicalGetConstraintInfoVisitor : public simulation::MechanicalVisitor
{
public:
    typedef core::componentmodel::behavior::BaseConstraint::PersistentID PersistentID;
    typedef core::componentmodel::behavior::BaseConstraint::ConstCoord ConstCoord;
    typedef core::componentmodel::behavior::BaseConstraint::ConstraintGroupInfo ConstraintGroupInfo;

    MechanicalGetConstraintInfoVisitor(std::vector<ConstraintGroupInfo>& groups, std::vector<PersistentID>& ids, std::vector<ConstCoord>& positions)
        : _groups(groups), _ids(ids), _positions(positions)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdConstraint(simulation::Node* node, core::componentmodel::behavior::BaseConstraint* c)
    {
        ctime_t t0 = begin(node, c);
        c->getConstraintInfo(_groups, _ids, _positions);
        end(node, c, t0);
        return RESULT_CONTINUE;
    }

#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
private:
    std::vector<ConstraintGroupInfo>& _groups;
    std::vector<PersistentID>& _ids;
    std::vector<ConstCoord>& _positions;
};


template<class real>
class CudaMasterContactSolver : public sofa::simulation::MasterSolverImpl
{
public:
    SOFA_CLASS(CudaMasterContactSolver,sofa::simulation::MasterSolverImpl);
    Data<int> useGPU_d;
#ifdef DISPLAY_TIME
    Data<bool> print_info;
#endif

    Data<bool> initial_guess;
    Data < double > tol;
    Data < int > maxIt;
    Data < double > mu;

    Data < helper::set<int> > constraintGroups;

    CudaMasterContactSolver();
    // virtual const char* getTypeName() const { return "MasterSolver"; }

    void step (double dt);

    //virtual void propagatePositionAndVelocity(double t, VecId x, VecId v);

    virtual void init();
    //LCP* getLCP(void) {return (lcp == &lcp1) ? &lcp2 : &lcp1;};

private:
    std::vector<core::componentmodel::behavior::BaseConstraintCorrection*> constraintCorrections;
    void computeInitialGuess();
    void keepContactForcesValue();

    void build_LCP();

    unsigned int _numConstraints;
    unsigned int _realNumConstraints;
    double _mu;
    simulation::Node *context;

    CudaBaseMatrix<real> _W;
    CudaBaseVector<real> _dFree, _f;
#ifdef CHECK
    CudaBaseVector<real> f_check;
#endif


    typedef core::componentmodel::behavior::BaseConstraint::PersistentID PersistentID;
    typedef core::componentmodel::behavior::BaseConstraint::ConstCoord ConstCoord;
    typedef core::componentmodel::behavior::BaseConstraint::ConstraintGroupInfo ConstraintGroupInfo;

    class ConstraintGroupBuf
    {
    public:
        std::map<PersistentID,int> persistentToConstraintIdMap;
        int nbLines; ///< how many dofs (i.e. lines in the matrix) are used by each constraint
    };

    std::map<core::componentmodel::behavior::BaseConstraint*, ConstraintGroupBuf> _previousConstraints;
    helper::vector< double > _previousForces;

    helper::vector<ConstraintGroupInfo> _constraintGroupInfo;
    helper::vector<PersistentID> _constraintIds;
    helper::vector<ConstCoord> _constraintPositions;


    helper::vector<unsigned> constraintRenumbering,constraintReinitialize;
};

} // namespace odesolver

} // namespace component

} // namespace sofa

#endif
