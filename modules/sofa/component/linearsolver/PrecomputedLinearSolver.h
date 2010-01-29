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
#ifndef SOFA_COMPONENT_LINEARSOLVER_PrecomputedLinearSolver_H
#define SOFA_COMPONENT_LINEARSOLVER_PrecomputedLinearSolver_H

#include <sofa/core/componentmodel/behavior/LinearSolver.h>
#include <sofa/component/linearsolver/MatrixLinearSolver.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/component/linearsolver/SparseMatrix.h>
#include <sofa/component/linearsolver/FullMatrix.h>
#include <sofa/helper/map.h>
#include <math.h>

namespace sofa
{

namespace component
{

namespace linearsolver
{

using namespace sofa::core;
using namespace sofa::core::componentmodel;
using namespace sofa::defaulttype;

using namespace sofa::core::componentmodel::behavior;
using namespace sofa::simulation;
using namespace sofa::core::objectmodel;

using namespace sofa::component::linearsolver;

using sofa::helper::system::thread::CTime;
using sofa::helper::system::thread::ctime_t;
using std::cerr;
using std::endl;

template<class TMatrix, class TVector>
class PrecomputedLinearSolverInternalData
{
public :
    typedef typename TMatrix::Real Real;

    FullMatrix<Real> JMinv;
};

/// Linear system solver using the conjugate gradient iterative algorithm
template<class TMatrix, class TVector>
class PrecomputedLinearSolver : public sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(PrecomputedLinearSolver,TMatrix,TVector),SOFA_TEMPLATE2(sofa::component::linearsolver::MatrixLinearSolver,TMatrix,TVector));

    typedef sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector> Inherit;
    typedef sofa::core::componentmodel::behavior::BaseMechanicalState::VecId VecId;
    typedef typename TMatrix::Real Real;

    Data<bool> f_verbose;
    Data<bool> use_file;
    Data <std::string> solverName;
    Data<int> init_MaxIter;
    Data<double> init_Tolerance;
    Data<double> init_Threshold;
    Data<std::map < std::string, sofa::helper::vector<double> > > f_graph;

    PrecomputedLinearSolver();
    void solve (TMatrix& M, TVector& x, TVector& b);
    void invert(TMatrix& M);
    void setSystemMBKMatrix(double mFact=0.0, double bFact=0.0, double kFact=0.0);
    void loadMatrix();
    void loadMatrixWithCSparse();
    void loadMatrixWithSolver();
    bool addJMInvJt(defaulttype::BaseMatrix* result, defaulttype::BaseMatrix* J, double fact);


    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MechanicalState.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        return BaseObject::canCreate(obj, context, arg);
    }

    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const PrecomputedLinearSolver<TMatrix,TVector>* = NULL)
    {
        return TMatrix::Name();
    }

    TMatrix * getSystemMatrixInv()
    {
        if (!this->systemMatrix) this->systemMatrix = new TMatrix();
        return this->systemMatrix;
    }

protected :
    template<class JMatrix>
    void ComputeResult(defaulttype::BaseMatrix * result,JMatrix& J, float fact);

    PrecomputedLinearSolverInternalData<TMatrix,TVector> internalData;

private :
    double init_mFact;
    double init_bFact;
    double init_kFact;
    bool first;
    unsigned systemSyze;
    double dt;
    double factInt;

};

} // namespace linearsolver

} // namespace component

} // namespace sofa

#endif
