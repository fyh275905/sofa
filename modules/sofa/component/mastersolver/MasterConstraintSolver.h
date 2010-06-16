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
#ifndef SOFA_COMPONENT_MASTERSOLVER_MASTERCONSTRAINTSOLVER_H
#define SOFA_COMPONENT_MASTERSOLVER_MASTERCONSTRAINTSOLVER_H

#include <sofa/core/behavior/OdeSolver.h>
#include <sofa/simulation/common/MasterSolverImpl.h>
#include <sofa/simulation/common/Node.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/core/behavior/BaseConstraintCorrection.h>
#include <sofa/core/behavior/OdeSolver.h>
#include <sofa/component/odesolver/OdeSolverImpl.h>
#include <sofa/component/linearsolver/FullMatrix.h>

#include <vector>

namespace sofa
{

namespace component
{

namespace mastersolver
{

using namespace sofa::defaulttype;
using namespace sofa::component::linearsolver;
using namespace helper::system::thread;

class SOFA_COMPONENT_MASTERSOLVER_API MechanicalGetConstraintResolutionVisitor : public simulation::MechanicalVisitor
{
public:
    MechanicalGetConstraintResolutionVisitor(std::vector<core::behavior::ConstraintResolution*>& res, unsigned int offset = 0)
        : _res(res),_offset(offset)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
        //serr<<"creation of the visitor"<<sendl;
    }

    virtual Result fwdConstraintSet(simulation::Node* node, core::behavior::BaseConstraintSet* cSet)
    {
        //serr<<"fwdConstraint called on "<<c->getName()<<sendl;

        if (core::behavior::BaseConstraint *c=dynamic_cast<core::behavior::BaseConstraint*>(cSet))
        {
            ctime_t t0 = begin(node, c);
            c->getConstraintResolution(_res, _offset);
            end(node, c, t0);
        }
        return RESULT_CONTINUE;
    }

#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
private:
    std::vector<core::behavior::ConstraintResolution*>& _res;
    unsigned int _offset;
};

class SOFA_COMPONENT_MASTERSOLVER_API MechanicalSetConstraint : public simulation::MechanicalVisitor
{
public:
    MechanicalSetConstraint(unsigned int &_contactId)
        :contactId(_contactId)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdConstraintSet(simulation::Node* node, core::behavior::BaseConstraintSet* c)
    {
        ctime_t t0 = begin(node, c);
//		  unsigned int temp = contactId;
        c->buildConstraintMatrix(contactId);
//		  std::cout << node->getName() << " : " << contactId - temp << std::endl;
        end(node, c, t0);
        return RESULT_CONTINUE;
    }


    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalSetConstraint"; }

    virtual bool isThreadSafe() const
    {
        return false;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif

protected:
    unsigned int &contactId;
};

class SOFA_COMPONENT_MASTERSOLVER_API MechanicalAccumulateConstraint2 : public simulation::MechanicalVisitor
{
public:
    MechanicalAccumulateConstraint2()
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual void bwdMechanicalMapping(simulation::Node* node, core::behavior::BaseMechanicalMapping* map)
    {
        ctime_t t0 = begin(node, map);
        map->accumulateConstraint();
        end(node, map, t0);
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAccumulateConstraint2"; }

    virtual bool isThreadSafe() const
    {
        return false;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};



class SOFA_COMPONENT_MASTERSOLVER_API ConstraintProblem
{
private:
    LPtrFullMatrix<double> _W;
    FullVector<double> _dFree, _force, _d, _df;              // cf. These Duriez + _df for scheme correction
    std::vector<core::behavior::ConstraintResolution*> _constraintsResolutions;
    double _tol;
    int _dim;
    CTime *_timer;

public:
    ConstraintProblem();
    ~ConstraintProblem();
    void clear(int dim, const double &tol);

    inline int getSize(void) {return _dim;};
    inline LPtrFullMatrix<double>* getW(void) {return &_W;};
    inline FullVector<double>* getDfree(void) {return &_dFree;};
    inline FullVector<double>* getD(void) {return &_d;};
    inline FullVector<double>* getF(void) {return &_force;};
    inline FullVector<double>* getdF(void) {return &_df;};
    inline std::vector<core::behavior::ConstraintResolution*>& getConstraintResolutions(void) {return _constraintsResolutions;};
    inline double *getTolerance(void) {return &_tol;};

    void gaussSeidelConstraintTimed(double &timeout, int numItMax);

};
class SOFA_COMPONENT_MASTERSOLVER_API MasterConstraintSolver : public sofa::simulation::MasterSolverImpl
{
public:
    SOFA_CLASS(MasterConstraintSolver, sofa::simulation::MasterSolverImpl);

    MasterConstraintSolver();
    virtual ~MasterConstraintSolver();
    // virtual const char* getTypeName() const { return "MasterSolver"; }

    void step(double dt);

    //virtual void propagatePositionAndVelocity(double t, VecId x, VecId v);

    virtual void init();

    Data<bool> displayTime;
    Data<double> _tol;
    Data<int> _maxIt;
    Data<bool> doCollisionsFirst;
    Data<bool> doubleBuffer;
    Data<bool> scaleTolerance;
    Data<bool> _allVerified;
    Data<double> _sor;
    Data<bool> schemeCorrection;

    Data<std::map < std::string, sofa::helper::vector<double> > > _graphErrors, _graphConstraints;

    ConstraintProblem *getConstraintProblem(void) {return (bufCP1 == true) ? &CP1 : &CP2;};

private:
    void launchCollisionDetection();
    void freeMotion(simulation::Node *context, double &dt);
    void setConstraintEquations(simulation::Node *context);
    void correctiveMotion(simulation::Node *context);
    void debugWithContact(int numConstraints);

    ///  Specific procedures that are called for setting the constraints:

    /// 1.calling resetConstraint & setConstraint & accumulateConstraint visitors
    /// and resize the constraint problem that will be solved
    void writeAndAccumulateAndCountConstraintDirections(simulation::Node *context, unsigned int &numConstraints);

    /// 2.calling GetConstraintValueVisitor: each constraint provides its present violation
    /// for a given state (by default: free_position TODO: add VecId to make this method more generic)
    void getIndividualConstraintViolations(simulation::Node *context);

    /// 3.calling getConstraintResolution: each constraint provides a method that is used to solve it during GS iterations
    void getIndividualConstraintSolvingProcess(simulation::Node *context);

    /// 4.calling getCompliance projected in the contact space => getDelassusOperator(_W) = H*C*Ht
    void computeComplianceInConstraintSpace();


    /// method for predictive scheme:
    void computePredictiveForce(int dim, double* force, std::vector<core::behavior::ConstraintResolution*>& res);



    void gaussSeidelConstraint(int dim, double* dfree, double** w, double* force, double* d, std::vector<core::behavior::ConstraintResolution*>& res, double* df);

    std::vector<core::behavior::BaseConstraintCorrection*> constraintCorrections;


    bool bufCP1;
    ConstraintProblem CP1, CP2;

    CTime *timer;
    double timeScale, time ;
    bool debug;

    unsigned int numConstraints;




};

} // namespace mastersolver

} // namespace component

} // namespace sofa

#endif
