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
// Author: Hadrien Courtecuisse
//
// Copyright: See COPYING file that comes with this distribution
#include <sofa/component/linearsolver/SparseTAUCSSolver.h>
#include <sofa/core/ObjectFactory.h>
#include <iostream>
#include "sofa/helper/system/thread/CTime.h"
#include <sofa/core/objectmodel/BaseContext.h>
#include <sofa/core/behavior/LinearSolver.h>
#include <math.h>
#include <sofa/helper/system/thread/CTime.h>
#include <sofa/component/linearsolver/CompressedRowSparseMatrix.h>

namespace sofa
{

namespace component
{

namespace linearsolver
{

using namespace sofa::defaulttype;
using namespace sofa::core::behavior;
using namespace sofa::simulation;
using namespace sofa::core::objectmodel;
using sofa::helper::system::thread::CTime;
using sofa::helper::system::thread::ctime_t;
using std::cerr;
using std::endl;

template<class TMatrix, class TVector>
SparseTAUCSSolver<TMatrix,TVector>::SparseTAUCSSolver()
    : f_options( initData(&f_options,"options","TAUCS unified solver list of space-separated options") )
    , f_symmetric( initData(&f_symmetric,true,"symmetric","Consider the system matrix as symmetric") )
    , f_verbose( initData(&f_verbose,false,"verbose","Dump system state at each iteration") )

{
}

template<class T>
int get_taucs_flags();

template<>
int get_taucs_flags<double>() { return TAUCS_DOUBLE; }

template<>
int get_taucs_flags<float>() { return TAUCS_SINGLE; }

template<class TMatrix, class TVector>
void SparseTAUCSSolver<TMatrix,TVector>::invert(Matrix& M)
{
    M.compress();

    SparseTAUCSSolverInvertData * data = (SparseTAUCSSolverInvertData *) M.getMatrixInvertData();
    if (data==NULL)
    {
        M.setMatrixInvertData(new SparseTAUCSSolverInvertData());
        data = (SparseTAUCSSolverInvertData *) M.getMatrixInvertData();
    }

    if (f_symmetric.getValue())
    {
        data->Mfiltered.copyUpperNonZeros(M);
        sout << "Filtered upper part of M, nnz = " << data->Mfiltered.getRowBegin().back() << sendl;
    }
    else
    {
        data->Mfiltered.copyNonZeros(M);
        sout << "Filtered M, nnz = " << data->Mfiltered.getRowBegin().back() << sendl;
    }
    data->Mfiltered.fullRows();
    data->matrix_taucs.n = data->Mfiltered.rowSize();
    data->matrix_taucs.m = data->Mfiltered.colSize();
    data->matrix_taucs.flags = get_taucs_flags<Real>();
    if (f_symmetric.getValue())
    {
        data->matrix_taucs.flags |= TAUCS_SYMMETRIC;
        data->matrix_taucs.flags |= TAUCS_LOWER; // Upper on row-major is actually lower on column-major transposed matrix
    }
    data->matrix_taucs.colptr = (int *) &(data->Mfiltered.getRowBegin()[0]);
    data->matrix_taucs.rowind = (int *) &(data->Mfiltered.getColsIndex()[0]);
    data->matrix_taucs.values.d = (double*) &(data->Mfiltered.getColsValue()[0]);
    helper::vector<char*> opts;
    const helper::vector<std::string>& options = f_options.getValue();
    for (unsigned int i=0; i<options.size(); ++i)
        opts.push_back((char*)options[i].c_str());
    opts.push_back(NULL);
    if (this->f_printLog.getValue())
        taucs_logfile((char*)"stdout");
    if (data->factorization) taucs_linsolve(NULL, &data->factorization, 0, NULL, NULL, NULL, NULL);
    int rc = taucs_linsolve(&data->matrix_taucs, &data->factorization, 0, NULL, NULL, &(opts[0]), NULL);
    if (this->f_printLog.getValue())
        taucs_logfile((char*)"none");
    if (rc != TAUCS_SUCCESS)
    {
        const char* er = "";
        switch(rc)
        {
        case TAUCS_SUCCESS: er = "SUCCESS"; break;
        case TAUCS_ERROR  : er = "ERROR"; break;
        case TAUCS_ERROR_NOMEM: er = "NOMEM"; break;
        case TAUCS_ERROR_BADARGS: er = "BADARGS"; break;
        case TAUCS_ERROR_MAXDEPTH: er = "MAXDEPTH"; break;
        case TAUCS_ERROR_INDEFINITE: er = "INDEFINITE"; break;
        }
        serr << "TAUCS factorization failed: " << er << sendl;
    }

}

template<class TMatrix, class TVector>
void SparseTAUCSSolver<TMatrix,TVector>::solve (Matrix& M, Vector& z, Vector& r)
{
    SparseTAUCSSolverInvertData * data = (SparseTAUCSSolverInvertData *) M.getMatrixInvertData();
    if (data==NULL)
    {
        z = r;
        return;
    }

    helper::vector<char*> opts;
    const helper::vector<std::string>& options = f_options.getValue();
    for (unsigned int i=0; i<options.size(); ++i)
        opts.push_back((char*)options[i].c_str());
    //opts.push_back((char*)"taucs.factor.symbolic=false");
    //opts.push_back((char*)"taucs.factor.numeric=false");
    opts.push_back((char*)"taucs.factor=false");
    opts.push_back(NULL);
    if (this->f_printLog.getValue())
        taucs_logfile((char*)"stdout");
    int rc = taucs_linsolve(&data->matrix_taucs, &data->factorization, 1, z.ptr(), r.ptr(), &(opts[0]), NULL);
    if (this->f_printLog.getValue())
        taucs_logfile((char*)"none");
    if (rc != TAUCS_SUCCESS)
    {
        const char* er = "";
        switch(rc)
        {
        case TAUCS_SUCCESS: er = "SUCCESS"; break;
        case TAUCS_ERROR  : er = "ERROR"; break;
        case TAUCS_ERROR_NOMEM: er = "NOMEM"; break;
        case TAUCS_ERROR_BADARGS: er = "BADARGS"; break;
        case TAUCS_ERROR_MAXDEPTH: er = "MAXDEPTH"; break;
        case TAUCS_ERROR_INDEFINITE: er = "INDEFINITE"; break;
        }
        serr << "TAUCS solve failed: " << er << sendl;
    }
}


SOFA_DECL_CLASS(SparseTAUCSSolver)

int SparseTAUCSSolverClass = core::RegisterObject("Direct linear solvers implemented with the TAUCS library")
        .add< SparseTAUCSSolver< CompressedRowSparseMatrix<double>,FullVector<double> > >(true)
        .addAlias("TAUCSSolver")
        ;

} // namespace linearsolver

} // namespace component

} // namespace sofa

