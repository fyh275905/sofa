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

#ifndef SOFA_CUDA_TETRAHEDRON_TLED_FORCEFIELD_H
#define SOFA_CUDA_TETRAHEDRON_TLED_FORCEFIELD_H

#include "CudaTypes.h"
#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/component/topology/MeshTopology.h>

// Total Lagrangian Explicit Dynamics algorithm from
// @InProceedings{Comas2008ISBMS,
// author = {Comas, O. and Taylor, Z. and Allard, J. and Ourselin, S. and Cotin, S. and Passenger, J.},
// title = {Efficient nonlinear FEM for soft tissue modelling and its GPU implementation within the open source framework SOFA},
// booktitle = {In Proceedings of ISBMS 2008},
// year = {2008},
// month = {July 7-8},
// address = {London, United Kingdom}
// }

namespace sofa
{

namespace gpu
{

namespace cuda
{

using namespace sofa::defaulttype;

class CudaTetrahedronTLEDForceField : public core::componentmodel::behavior::ForceField<CudaVec3fTypes>
{
public:
    typedef CudaVec3fTypes::Real Real;
    typedef CudaVec3fTypes::Coord Coord;
    typedef component::topology::MeshTopology::Tetra Element;
    typedef component::topology::MeshTopology::SeqTetras VecElement;

    /** Static data associated with each element
    */
    struct GPUElement
    {
        /// @name index of the 8 connected vertices
        /// @{
        int v[4];
        /// @}
    };

    CudaVector<GPUElement> elems;

    /// Varying data associated with each element
    struct GPUElementState
    {
        int dummy[4];
    };

    CudaVector<GPUElementState> state;

    int nbVertex; ///< number of vertices to process to compute all elements
    int nbElementPerVertex; ///< max number of elements connected to a vertex
    /// Index of elements attached to each points (layout per bloc of NBLOC vertices, with first element of each vertex, then second
    /// element, etc). Note that each integer is actually equat the the index of the element * 8 + the index of this vertex inside
    /// the element.
    CudaVector<int> velems;

    /// Material properties
    Data<Real> poissonRatio;
    Data<Real> youngModulus;
    /// Lame coefficients
    float Lambda, Mu;

    /// TLED configuration
    Data<Real> timestep;
    Data<unsigned int> viscoelasticity;
    Data<unsigned int> anisotropy;

    CudaTetrahedronTLEDForceField();
    virtual ~CudaTetrahedronTLEDForceField();
    void init();
    void reinit();
    void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& /*v*/);
    void addDForce (VecDeriv& df, const VecDeriv& dx);
    double getPotentialEnergy(const VecCoord&) { return 0.0; }

    /// Compute lambda and mu based on the Young modulus and Poisson ratio
    void updateLameCoefficients();

    /// Compute element volumes for tetrahedral elements
    float CompElVolTetra( const Element& e, const VecCoord& x );
    /// Compute shape function global derivatives for tetrahedral elements
    void ComputeDhDxTetra(const Element& e, const VecCoord& x, float DhDr[4][3], float DhDx[4][3]);

protected:

    void init(int nbe, int nbv, int nbelemperv)
    {
        elems.resize(nbe);
        state.resize(nbe);
        nbVertex = nbv;
        nbElementPerVertex = nbelemperv;
//         int nbloc = (nbVertex+BSIZE-1)/BSIZE;
//         velems.resize(nbloc*nbElementPerVertex*BSIZE);
//         for (unsigned int i=0; i<velems.size(); i++)
//         {
//             velems[i] = 0;
//         }
    }

//     void setV(int vertex, int num, int index)
//     {
//         int bloc = vertex/BSIZE;
//         int b_x  = vertex%BSIZE;
//         velems[ bloc*BSIZE*nbElementPerVertex // start of the bloc
//               + num*BSIZE                     // offset to the element
//               + b_x                           // offset to the vertex
//               ] = index+1;
//     }

    void setE(int i, const Element& indices)
    {
        GPUElement& e = elems[i];
        for (unsigned int j=0; j<indices.size(); j++)
        {
            e.v[j] = indices[j];
        }
    }
};

} // namespace cuda

} // namespace gpu

} // namespace sofa

#endif
