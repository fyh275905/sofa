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
//
// C++ Interface: SpatialGridContainer
//
// Description:
//
//
// Author: The SOFA team <http://www.sofa-framework.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef SOFA_GPU_CUDA_CUDASPATIALGRIDCONTAINER_H
#define SOFA_GPU_CUDA_CUDASPATIALGRIDCONTAINER_H

#include <sofa/component/container/SpatialGridContainer.h>
#include <sofa/gpu/cuda/CudaTypes.h>
#include <sofa/defaulttype/Vec.h>

namespace sofa
{

namespace component
{

namespace container
{

using namespace sofa::defaulttype;

template<class TCoord, class TDeriv, class TReal>
class SpatialGrid< SpatialGridTypes < gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> > >
{
public:
    typedef SpatialGridTypes < gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> > DataTypes;
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::CellData CellData;
    typedef typename DataTypes::GridData GridData;
    //typedef typename DataTypes::NeighborListener NeighborListener;
    typedef typename DataTypes::ParticleField ParticleField;

public:
    SpatialGrid(Real cellWidth);

    void update(const VecCoord& x);

    void draw();

    template<class NeighborListener>
    void findNeighbors(NeighborListener* dest, Real dist);

    void computeField(ParticleField* field, Real dist);

    /// Change particles ordering inside a given cell have contiguous indices
    ///
    /// Fill the old2new and new2old arrays giving the permutation to apply
    void reorderIndices(helper::vector<unsigned int>* old2new, helper::vector<unsigned int>* new2old);
#if 0
    enum { GRIDDIM_LOG2 = DataTypes::GRIDDIM_LOG2, GRIDDIM = 1<<GRIDDIM_LOG2 };
    enum { NCELL = GRIDDIM*GRIDDIM*GRIDDIM };
    enum { DX = 1, DY = GRIDDIM, DZ = GRIDDIM*GRIDDIM };

    class Entry
    {
    public:
        int index;
        Coord pos;
        Entry(int i, const Coord& p) : index(i), pos(p) {}
    };

    class Cell
    {
    public:
        std::list<Entry> plist;
        CellData data;
        void clear()
        {
            plist.clear();
            data.clear();
        }
    };
#endif

    GridData data;

    Real getCellWidth() const { return cellWidth; }
    Real getInvCellWidth() const { return invCellWidth; }

protected:
    const Real cellWidth;
    const Real invCellWidth;
    int cellBits, nbCells;
    sofa::gpu::cuda::CudaVector< sofa::helper::fixed_array<int,2> > particleHash, sortTmp;
    sofa::gpu::cuda::CudaVector<int> cellStart;
    sofa::gpu::cuda::CudaVector< sofa::gpu::cuda::Vec3f1 > sortedPos;
    const VecCoord* lastX;

    static void kernel_updateGrid(int cellBits, float cellWidth, int nbPoints, void* particleHash, void* sortTmp, void* cellStart, const void* x);
    static void kernel_reorderData(int nbPoints, const void* particleHash, void* sorted, const void* x);

};

} // namespace container

} // namespace component

} // namespace sofa

#endif
