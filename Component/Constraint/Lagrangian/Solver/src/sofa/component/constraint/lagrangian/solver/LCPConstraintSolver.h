/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once
#include <sofa/component/constraint/lagrangian/solver/config.h>

#include <sofa/component/constraint/lagrangian/solver/ConstraintSolverImpl.h>
#include <sofa/core/behavior/BaseConstraintCorrection.h>
#include <sofa/core/behavior/BaseConstraint.h>

#include <sofa/simulation/MechanicalVisitor.h>
#include <sofa/simulation/fwd.h>

#include <sofa/linearalgebra/FullMatrix.h>
#include <sofa/linearalgebra/SparseMatrix.h>

#include <sofa/helper/set.h>
#include <sofa/helper/map.h>
#include <sofa/helper/LCPcalc.h>

namespace sofa::component::constraint::lagrangian::solver
{

class LCPConstraintProblem : public ConstraintProblem
{
public:
    double mu;

    void solveTimed(double tolerance, int maxIt, double timeout) override;
};

class SOFA_COMPONENT_CONSTRAINT_LAGRANGIAN_SOLVER_API LCPConstraintSolver : public ConstraintSolverImpl
{
public:
    SOFA_CLASS(LCPConstraintSolver, ConstraintSolverImpl);

    typedef std::vector<core::behavior::BaseConstraintCorrection*> list_cc;
    typedef sofa::core::MultiVecId MultiVecId;

protected:
    /**
    * @brief Default Constructor
    */
    LCPConstraintSolver();

    /**
    * @brief Default Destructor
    */
    ~LCPConstraintSolver() override;
public:
    void init() override;

    void cleanup() override;

    bool prepareStates(const core::ConstraintParams * /*cParams*/, MultiVecId res1, MultiVecId res2=MultiVecId::null()) override;
    bool buildSystem(const core::ConstraintParams * /*cParams*/, MultiVecId res1, MultiVecId res2=MultiVecId::null()) override;
    bool solveSystem(const core::ConstraintParams * /*cParams*/, MultiVecId res1, MultiVecId res2=MultiVecId::null()) override;
    bool applyCorrection(const core::ConstraintParams * /*cParams*/, MultiVecId res1, MultiVecId res2=MultiVecId::null()) override;

    void draw(const core::visual::VisualParams* vparams) override;


    Data<bool> displayDebug; ///< Display debug information.
    Data<bool> initial_guess; ///< activate LCP results history to improve its resolution performances.
    Data<bool> build_lcp; ///< LCP is not fully built to increase performance in some case.
    Data<double> tol; ///< residual error threshold for termination of the Gauss-Seidel algorithm
    Data<int> maxIt; ///< maximal number of iterations of the Gauss-Seidel algorithm
    Data<double> mu; ///< Friction coefficient
    Data<double> minW; ///< If not zero, constraints whose self-compliance (i.e. the corresponding value on the diagonal of W) is smaller than this threshold will be ignored
    Data<double> maxF; ///< If not zero, constraints whose response force becomes larger than this threshold will be ignored
    Data<bool> multi_grid; ///< activate multi_grid resolution (NOT STABLE YET)
    Data<int> multi_grid_levels; ///< if multi_grid is active: how many levels to create (>=2)
    Data<int> merge_method; ///< if multi_grid is active: which method to use to merge constraints (0 = compliance-based, 1 = spatial coordinates)
    Data<int> merge_spatial_step; ///< if merge_method is 1: grid size reduction between multigrid levels
    Data<int> merge_local_levels; ///< if merge_method is 1: up to the specified level of the multigrid, constraints are grouped locally, i.e. separately within each contact pairs, while on upper levels they are grouped globally independently of contact pairs.

    Data < std::set<int> > constraintGroups; ///< list of ID of groups of constraints to be handled by this solver.

    Data<std::map < std::string, sofa::type::vector<double> > > f_graph; ///< Graph of residuals at each iteration

    Data<int> showLevels; ///< Number of constraint levels to display
    Data<double> showCellWidth; ///< Distance between each constraint cells
    Data<type::Vector3> showTranslation; ///< Position of the first cell
    Data<type::Vector3> showLevelTranslation; ///< Translation between levels

    ConstraintProblem* getConstraintProblem() override;
    void lockConstraintProblem(sofa::core::objectmodel::BaseObject* from, ConstraintProblem* p1, ConstraintProblem* p2=nullptr) override; ///< Do not use the following LCPs until the next call to this function. This is used to prevent concurent access to the LCP when using a LCPForceFeedback through an haptic thread

    void removeConstraintCorrection(core::behavior::BaseConstraintCorrection *s) override;

private:
    std::vector<core::behavior::BaseConstraintCorrection*> constraintCorrections;
	std::vector<char> constraintCorrectionIsActive; // for each constraint correction, a boolean that is false if the parent node is sleeping
    void computeInitialGuess();
    void keepContactForcesValue();

    unsigned int _numConstraints;
    double _mu;

    /// for built lcp ///
    void build_LCP();
    LCPConstraintProblem lcp1, lcp2, lcp3; // Triple buffer for LCP.
    LCPConstraintProblem *lcp, *last_lcp; /// use of last_lcp allows several LCPForceFeedback to be used in the same scene
    sofa::linearalgebra::LPtrFullMatrix<double>  *_W;

    /// multi-grid approach ///
    void MultigridConstraintsMerge();
    void MultigridConstraintsMerge_Compliance();
    void MultigridConstraintsMerge_Spatial();
    void build_Coarse_Compliance(std::vector<int> &/*constraint_merge*/, int /*sizeCoarseSystem*/);
    sofa::linearalgebra::LPtrFullMatrix<double>  _Wcoarse;

    std::vector< std::vector< int > > hierarchy_contact_group;
    std::vector< std::vector< int > > hierarchy_constraint_group;
    std::vector< std::vector< double > > hierarchy_constraint_group_fact;
    std::vector< unsigned int > hierarchy_num_group;


    /// common built-unbuilt
    sofa::core::objectmodel::BaseContext *context;
    sofa::linearalgebra::FullVector<double> *_dFree, *_result;
    ///

    /// for unbuilt lcp ///
    void build_problem_info();
    int lcp_gaussseidel_unbuilt(double *dfree, double *f, std::vector<double>* residuals = nullptr);
    int nlcp_gaussseidel_unbuilt(double *dfree, double *f, std::vector<double>* residuals = nullptr);
    int gaussseidel_unbuilt(double *dfree, double *f, std::vector<double>* residuals = nullptr);

    sofa::linearalgebra::SparseMatrix<double> *_Wdiag;
    std::vector<core::behavior::BaseConstraintCorrection*> _cclist_elem1;
    std::vector<core::behavior::BaseConstraintCorrection*> _cclist_elem2;

    typedef core::behavior::BaseConstraint::ConstraintBlockInfo ConstraintBlockInfo;
    typedef core::behavior::BaseConstraint::PersistentID PersistentID;
    typedef core::behavior::BaseConstraint::ConstCoord ConstCoord;
    typedef core::behavior::BaseConstraint::ConstDeriv ConstDeriv;
    typedef core::behavior::BaseConstraint::ConstArea ConstArea;

    typedef core::behavior::BaseConstraint::VecConstraintBlockInfo VecConstraintBlockInfo;
    typedef core::behavior::BaseConstraint::VecPersistentID VecPersistentID;
    typedef core::behavior::BaseConstraint::VecConstCoord VecConstCoord;
    typedef core::behavior::BaseConstraint::VecConstDeriv VecConstDeriv;
    typedef core::behavior::BaseConstraint::VecConstArea VecConstArea;

    class ConstraintBlockBuf
    {
    public:
        std::map<PersistentID,int> persistentToConstraintIdMap;
        int nbLines; ///< how many dofs (i.e. lines in the matrix) are used by each constraint
    };

    std::map<core::behavior::BaseConstraint*, ConstraintBlockBuf> _previousConstraints;
    type::vector< double > _previousForces;

    type::vector< VecConstraintBlockInfo > hierarchy_constraintBlockInfo;
    type::vector< VecPersistentID > hierarchy_constraintIds;
    type::vector< VecConstCoord > hierarchy_constraintPositions;
    type::vector< VecConstDeriv > hierarchy_constraintDirections;
    type::vector< VecConstArea > hierarchy_constraintAreas;

    // for gaussseidel_unbuilt
    type::vector< helper::LocalBlock33 > unbuilt_W33;
    type::vector< double > unbuilt_d;

    type::vector< double > unbuilt_W11;

    bool isActive;
};

} //namespace sofa::component::constraint::lagrangian::solver
