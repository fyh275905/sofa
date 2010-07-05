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

#include <sofa/component/constraintset/LMConstraintDirectSolver.h>
#include <sofa/component/constraintset/ContactDescription.h>
#include <sofa/core/ObjectFactory.h>
#include <Eigen/QR>
#include <Eigen/SVD>


namespace sofa
{

namespace component
{

namespace constraintset
{

LMConstraintDirectSolver::LMConstraintDirectSolver():
    solverAlgorithm(initData(&solverAlgorithm, "solverAlgorithm", "Algorithm used to solve the system W.Lambda=c"))
{
    sofa::helper::OptionsGroup algo(2,"SVD","QR");
    solverAlgorithm.setValue(algo);
}


bool LMConstraintDirectSolver::buildSystem(double dt, VecId id, core::behavior::BaseConstraintSet::ConstOrder order)
{
    bool sucess=LMConstraintSolver::buildSystem(dt,id,order);

    return sucess;
}

bool LMConstraintDirectSolver::solveSystem(double dt, VecId id, core::behavior::BaseConstraintSet::ConstOrder order)
{
    //First, do n iterations of Gauss Seidel
    bool success=LMConstraintSolver::solveSystem(dt,id,order);


    if (order != core::behavior::BaseConstraintSet::VEL) return success;


    //Then process to a direct solution of the system

    //We need to find all the constraint related to contact
    // 1. extract the information about the state of the contact and build the new L, L^T matrices
    // 2. build the full system
    // 3. solve


    //------------------------------------------------------------------
    // extract the information about the state of the contact
    //------------------------------------------------------------------

    //************************************************************
#ifdef SOFA_DUMP_VISITOR_INFO
    sofa::simulation::Visitor::printNode("AnalyseConstraints");
#endif
    const helper::vector< BaseLMConstraint* > &LMConstraints=LMConstraintVisitor.getConstraints();

    JacobianRows rowsL ; rowsL.reserve(numConstraint);
    JacobianRows rowsLT; rowsLT.reserve(numConstraint);
    helper::vector< unsigned int > rightHandElements;

    analyseConstraints(LMConstraints, order,
            rowsL, rowsLT, rightHandElements);

#ifdef SOFA_DUMP_VISITOR_INFO
    sofa::simulation::Visitor::printCloseNode("AnalyseConstraints");
#endif
    if (rowsL.empty() || rowsLT.empty()) return success;





#ifdef SOFA_DUMP_VISITOR_INFO
    sofa::simulation::Visitor::printNode("BuildFullSystem");
#endif
    //------------------------------------------------------------------
    // build c: right hand term
    //------------------------------------------------------------------
    VectorEigen previousC(c);
    //TODO: change newC by c
    c=VectorEigen::Zero(rowsL.size());
    unsigned int idx=0;
    for (helper::vector<unsigned int >::const_iterator it=rightHandElements.begin(); it!=rightHandElements.end(); ++it)
        c[idx++]=previousC[*it];

    //------------------------------------------------------------------
    // build the L and LT matrices
    //------------------------------------------------------------------


    DofToMatrix LMatricesDirectSolver;
    DofToMatrix LTMatricesDirectSolver;
    for (DofToMatrix::iterator it=LMatrices.begin(); it!=LMatrices.end(); ++it)
    {
        //------------------------------------------------------------------
        const SparseMatrixEigen& matrix= it->second;
        //Init the manipulator with the full matrix
        linearsolver::LMatrixManipulator manip;
        manip.init(matrix);


        //------------------------------------------------------------------
        SparseMatrixEigen  L (rowsL.size(),  matrix.cols());
        L.startFill(rowsL.size()*matrix.cols());
        manip.buildLMatrix(rowsL ,L);
        L.endFill();
        LMatricesDirectSolver.insert (std::make_pair(it->first,L ));



        //------------------------------------------------------------------
        SparseMatrixEigen  LT(rowsLT.size(), matrix.cols());
        LT.startFill(rowsLT.size()*matrix.cols());
        manip.buildLMatrix(rowsLT,LT);
        LT.endFill();
        LTMatricesDirectSolver.insert(std::make_pair(it->first,LT));
    }



    //------------------------------------------------------------------
    // build the full system
    //------------------------------------------------------------------
    const  int rows=rowsL.size();
    const  int cols=rowsLT.size();
    SparseColMajorMatrixEigen Wsparse(rows,cols);
    buildLeftRectangularMatrix(invMassMatrix, LMatricesDirectSolver, LTMatricesDirectSolver, Wsparse,invMass_Ltrans);


    //------------------------------------------------------------------
    // conversion from sparse to dense matrix
    //------------------------------------------------------------------
    Lambda=VectorEigen::Zero(rows);

    W=MatrixEigen::Zero(rows,cols);

    SparseMatrixEigen Wresult(Wsparse);
    for (int k=0; k<Wresult.outerSize(); ++k)
        for (SparseMatrixEigen::InnerIterator it(Wresult,k); it; ++it) W(it.row(),it.col()) = it.value();



#ifdef SOFA_DUMP_VISITOR_INFO
    sofa::simulation::Visitor::printCloseNode("BuildFullSystem");
#endif

    //------------------------------------------------------------------
    // Solve the system
    //------------------------------------------------------------------
    const std::string &algo=solverAlgorithm.getValue().getSelectedItem() ;
#ifdef SOFA_DUMP_VISITOR_INFO
    simulation::Visitor::TRACE_ARGUMENT arg1;
    arg1.push_back(std::make_pair("Algorithm", algo));
    arg1.push_back(std::make_pair("Dimension", printDimension(W)));
    sofa::simulation::Visitor::printNode("DirectSolveSystem", "",arg1);
#endif
    if (algo == "QR")
    {
        Eigen::QR< MatrixEigen > solverQR(W);
        if (this->f_printLog.getValue()) sout << printDimension(W) << " is W invertible? " << std::boolalpha << solverQR.isInvertible()  << sendl;
        if (solverQR.isInvertible())
            solverQR.solve(c, &Lambda);
        else
        {
            if (this->f_printLog.getValue()) sout << "Fallback on SVD decomposition" << sendl;
            Eigen::SVD< MatrixEigen > solverSVD(W);
            solverSVD.solve(c, &Lambda);
        }

    }
    else if(algo == "SVD")
    {
        Eigen::SVD< MatrixEigen > solverSVD(W);
        solverSVD.solve(c, &Lambda);
    }

    if (this->f_printLog.getValue())
    {
        sout << "W" <<  printDimension(W) <<  "  Lambda" << printDimension(Lambda) << "  c" << printDimension(c) << sendl;
        sout << "\nW     ===============================================\n" << W
                <<  "\nLambda===============================================\n" << Lambda
                <<  "\nc     ===============================================\n" << c << sendl;
    }

#ifdef SOFA_DUMP_VISITOR_INFO
    sofa::simulation::Visitor::printCloseNode("DirectSolveSystem");
#endif
    return success;

}


void LMConstraintDirectSolver::analyseConstraints(const helper::vector< BaseLMConstraint* > &LMConstraints, core::behavior::BaseConstraintSet::ConstOrder order,
        JacobianRows &rowsL,JacobianRows &rowsLT, helper::vector< unsigned int > &rightHandElements) const
{
    //Iterate among all the Sofa LMConstraint
    for (unsigned int componentConstraint=0; componentConstraint<LMConstraints.size(); ++componentConstraint)
    {
        BaseLMConstraint *constraint=LMConstraints[componentConstraint];
        //Find the constraint dealing with contact
        if (ContactDescriptionHandler* contactDescriptor=dynamic_cast<ContactDescriptionHandler*>(constraint))
        {
            const helper::vector< BaseLMConstraint::ConstraintGroup* > &constraintOrder=constraint->getConstraintsOrder(order);
            //Iterate among all the contacts
            for (helper::vector< BaseLMConstraint::ConstraintGroup* >::const_iterator itGroup=constraintOrder.begin(); itGroup!=constraintOrder.end(); ++itGroup)
            {
                const BaseLMConstraint::ConstraintGroup* group=*itGroup;
                const ContactDescription& contact=contactDescriptor->getContactDescription(group);

                const unsigned int idxEquation=group->getConstraint(0).idx;

                switch(contact.state)
                {
                case VANISHING:
                {
//                    serr <<"Constraint " << idxEquation << " VANISHING" << sendl;
                    //0 equation
                    break;
                }
                case STICKING:
                {
//                    serr << "Constraint " <<idxEquation << " STICKING" << sendl;
                    const unsigned int i=rowsL.size();
                    rowsL.push_back(linearsolver::LLineManipulator().addCombination(idxEquation  ));
                    rowsL.push_back(linearsolver::LLineManipulator().addCombination(idxEquation+1));
                    rowsL.push_back(linearsolver::LLineManipulator().addCombination(idxEquation+2));

                    //3 equations
                    rowsLT.push_back(rowsL[i  ]);
                    rowsLT.push_back(rowsL[i+1]);
                    rowsLT.push_back(rowsL[i+2]);

                    rightHandElements.push_back(idxEquation  );
                    rightHandElements.push_back(idxEquation+1);
                    rightHandElements.push_back(idxEquation+2);
                    break;
                }
                case SLIDING:
                {
//                    serr << "Constraint " <<idxEquation << " SLIDING" << sendl;
                    rowsL.push_back(linearsolver::LLineManipulator().addCombination(idxEquation  ));
                    rowsL.push_back(linearsolver::LLineManipulator().addCombination(idxEquation+1));
                    rowsL.push_back(linearsolver::LLineManipulator().addCombination(idxEquation+2));


                    //1 equation with the response force along the Coulomb friction cone
                    rowsLT.push_back(linearsolver::LLineManipulator()
                            .addCombination(idxEquation  ,contact.coeff[0])
                            .addCombination(idxEquation+1,contact.coeff[1])
                            .addCombination(idxEquation+2,contact.coeff[2]));

                    rightHandElements.push_back(idxEquation  );
                    rightHandElements.push_back(idxEquation+1);
                    rightHandElements.push_back(idxEquation+2);
                    break;
                }
                }
            }
        }
        else
        {
            //Non contact constraints: we add all the equations
            const helper::vector< BaseLMConstraint::ConstraintGroup* > &constraintOrder=constraint->getConstraintsOrder(order);
            for (helper::vector< BaseLMConstraint::ConstraintGroup* >::const_iterator itGroup=constraintOrder.begin(); itGroup!=constraintOrder.end(); ++itGroup)
            {
                const BaseLMConstraint::ConstraintGroup* group=*itGroup;
                std::pair< BaseLMConstraint::ConstraintGroup::EquationConstIterator,BaseLMConstraint::ConstraintGroup::EquationConstIterator> range=group->data();
                for ( BaseLMConstraint::ConstraintGroup::EquationConstIterator it=range.first; it!=range.second; ++it)
                {
                    rowsL.push_back(linearsolver::LLineManipulator().addCombination(it->idx));
                    rowsLT.push_back(rowsL.back());
                    rightHandElements.push_back(it->idx);
                }
            }
        }
    }
}




void LMConstraintDirectSolver::buildLeftRectangularMatrix(const DofToMatrix& invMassMatrix,
        DofToMatrix& LMatrix, DofToMatrix& LTMatrix,
        SparseColMajorMatrixEigen &LeftMatrix, DofToMatrix &invMass_Ltrans) const
{
    invMass_Ltrans.clear();
    for (SetDof::const_iterator itDofs=setDofs.begin(); itDofs!=setDofs.end(); ++itDofs)
    {
        const sofa::core::behavior::BaseMechanicalState* dofs=*itDofs;
        const SparseMatrixEigen &invMass=invMassMatrix.find(dofs)->second;
        SparseMatrixEigen &L =LMatrix[dofs];  L.endFill();
        SparseMatrixEigen &LT=LTMatrix[dofs]; LT.endFill();

        const SparseMatrixEigen &invMass_LT=invMass.marked<Eigen::SelfAdjoint|Eigen::UpperTriangular>()*LT.transpose();
        invMass_Ltrans.insert(std::make_pair(dofs, invMass_LT));
        const SparseColMajorMatrixEigen& temp=L*invMass_LT;
        LeftMatrix += temp;
    }
}
int LMConstraintDirectSolverClass = core::RegisterObject("A Direct Constraint Solver working specifically with LMConstraint based components")
        .add< LMConstraintDirectSolver >();

SOFA_DECL_CLASS(LMConstraintDirectSolver);


} // namespace constraintset

} // namespace component

} // namespace sofa
