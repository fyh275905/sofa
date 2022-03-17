#pragma once

#include <sofa/component/linearsolver/iterative/MatrixLinearSolver.h>

namespace sofa::component::linearsolver
{
void CSR_to_adj(int n,int * M_colptr,int * M_rowind,type::vector<int>& adj,type::vector<int>& xadj,type::vector<int>& t_adj, type::vector<int>& t_xadj, type::vector<int>& tran_countvec ); // compute the adjency matrix in CSR format from the matrix given in CSR format
}

