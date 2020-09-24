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

#include <SofaNonUniformFem/config.h>

#include <string>


#include <SofaNonUniformFem/SparseGridRamificationTopology.h>


namespace sofa::component::topology
{

/**
Build a SparseGridTopology for several given Triangular meshes.
A stiffness coefficient has to be assigned for each mesh. The last found stiffness coefficient is used for an element shared by several meshes => The mesh ordering is important, and so, more specific stiffness informations must appear in last.
*/
class SOFA_SOFANONUNIFORMFEM_API SparseGridMultipleTopology : public SparseGridRamificationTopology
{
public :
    SOFA_CLASS(SparseGridMultipleTopology,SparseGridRamificationTopology);
protected:
    SparseGridMultipleTopology( bool _isVirtual=false );
public:
    void init() override
    {
        if(_computeRamifications.getValue())
            SparseGridRamificationTopology::init(  );
        else
            SparseGridTopology::init(  );
    }

    void buildAsFinest() override;
    void buildFromFiner() override
    {
        if(_computeRamifications.getValue())
            SparseGridRamificationTopology::buildFromFiner(  );
        else
            SparseGridTopology::buildFromFiner(  );
    }
    void buildVirtualFinerLevels() override;


    index_type findCube(const Vector3 &pos, SReal &fx, SReal &fy, SReal &fz) override
    {
        if(_computeRamifications.getValue())
            return SparseGridRamificationTopology::findCube( pos,fx,fy,fz  );
        else
            return SparseGridTopology::findCube( pos,fx,fy,fz );
    }

    index_type findNearestCube(const Vector3& pos, SReal& fx, SReal &fy, SReal &fz) override
    {
        if(_computeRamifications.getValue())
            return SparseGridRamificationTopology::findNearestCube( pos,fx,fy,fz );
        else
            return SparseGridTopology::findNearestCube( pos,fx,fy,fz );
    }



protected :


    Data< helper::vector< std::string > > _fileTopologies; ///< All topology filenames
    Data< helper::vector< float > > _dataStiffnessCoefs; ///< A stiffness coefficient for each topology filename
    Data< helper::vector< float > > _dataMassCoefs; ///< A mass coefficient for each topology filename
    Data<bool> _computeRamifications; ///< Are ramifications wanted?
    Data<bool> _erasePreviousCoef; ///< Does a new stiffness/mass coefficient replace the previous or blend half/half with it?




    void buildFromTriangleMesh(helper::io::Mesh*, unsigned fileIdx);
    helper::vector< RegularGridTopology::SPtr > _regularGrids;
    helper::vector< helper::vector<Type> > _regularGridTypes;
    void assembleRegularGrids(helper::vector<Type>& regularGridTypes,helper::vector< float >& regularStiffnessCoefs,helper::vector< float >& regularMassCoefs);
};

} // namespace sofa::component::topology

